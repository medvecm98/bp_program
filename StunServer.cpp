#include "StunServer.hpp"

StunServer::StunServer(Networking* n) : networking_(n) {
    init_server(QHostAddress(QString("127.0.0.1")), STUN_PORT);

    connect(tcp_server_.get(), &QTcpServer::newConnection, 
            this, &StunServer::new_connection);
}

StunServer::StunServer(Networking* n, QHostAddress address, std::uint16_t port) : networking_(n) {
    init_server(address, port);

    connect(tcp_server_.get(), &QTcpServer::newConnection, 
            this, &StunServer::new_connection);
}

void StunServer::init_server(QHostAddress address, std::uint16_t port) {
    tcp_server_ = std::make_shared<QTcpServer>(this);

    if (!tcp_server_->listen(address, (quint16) port)) {
        std::cout << "STUN Server failed to start" << std::endl;
        return;
    }

    std::cout << "STUN Server is running on IP: " 
        << address.toString().toStdString() 
        << " and port: " << port << std::endl;

    /* ATTRIBUTE FACTORIES ARE INITIALIZED HERE */
    stun_attribute_factories.emplace(STUN_ATTR_XOR_MAPPED_ADDRESS, std::make_shared<XorMappedAddressAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_ERROR_CODE, std::make_shared<ErrorCodeAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_UNKNOWN_ATTRIBUTES, std::make_shared<UnknownAttributesAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_DATA, std::make_shared<DataAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_LIFETIME, std::make_shared<LifetimeAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_PUBLIC_IDENTIFIER, std::make_shared<PublicIdentifierAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_XOR_RELAYED_ADDRESS, std::make_shared<XorRelayedAddressAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_REQUESTED_TRANSPORT, std::make_shared<RequestedTransportAttributeFactory>());
}

void StunServer::display_error() {
    std::cout << "STUN server somehow got into an error..." << std::endl;
}

void StunServer::new_connection() {
    tcp_socket_ = tcp_server_->nextPendingConnection();
    tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    //std::cout << "SS: tcp_socket_ pointer: " << tcp_socket_ << std::endl;

    in_stream.setDevice(tcp_socket_);
    in_stream.setVersion(QDataStream::Qt_5_0);
    QObject::connect(tcp_socket_, &QIODevice::readyRead, this, &StunServer::reply);
    QObject::connect(tcp_socket_, &QAbstractSocket::disconnected, tcp_socket_, &QObject::deleteLater);
	QObject::connect(tcp_socket_, &QAbstractSocket::errorOccurred, this, &StunServer::display_error);
}

void StunServer::reply() {
    QTcpSocket* socket = (QTcpSocket*) QObject::sender();
    std::cout << "STUN server: replying to STUN message" << std::endl;

    stun_header_ptr stun_message = std::make_shared<StunMessageHeader>();
    stun_header_ptr stun_new = std::make_shared<StunMessageHeader>();

    //std::cout << "SS: tcp_socket_ pointer: " << tcp_socket_ << std::endl;
    //std::cout << "SS: socket state: " << tcp_socket_->state() << std::endl;

    stun_attr_type_vec unknown_cr_attributes;
    in_stream.startTransaction();

    try {
        stun_message->read_message_header(in_stream);
        stun_message->read_attributes(in_stream, stun_attribute_factories);
        in_stream.commitTransaction();

        if (stun_message->stun_class == StunClassEnum::request) {
            if (stun_message->stun_method == StunMethodEnum::binding) {
                auto mp = MPProcess<CRequestTag, MBindingTag>(stun_message, stun_new, socket, unknown_cr_attributes);
                MessageProcessor<CRequestTag, MBindingTag>::process(mp);
            }
            else if (stun_message->stun_method == StunMethodEnum::allocate) {
                process_request_allocate(stun_message, stun_new, socket);
            }
            else if (stun_message->stun_method == StunMethodEnum::identify) {
                process_request_identify(stun_message, stun_new);
            }
        }
        else if (stun_message->stun_class == StunClassEnum::indication) {
            if (stun_message->stun_method == StunMethodEnum::binding) {
                //no work here
            }
        }
        else {
            throw invalid_stun_message_format_error("Invalid message received.");
        }

        send_stun_message(socket, stun_new);
    }
    catch (invalid_stun_message_format_error de) {
        in_stream.abortTransaction();
        std::cout << de.what() << std::endl;
    }
    catch (unknown_comprehension_required_attribute_error u) {
        send_error(420, socket, socket->peerAddress(), socket->peerPort());
    }
}

void StunServer::send_error(std::size_t error_no, QTcpSocket* socket, QHostAddress address, quint16 port) {
    stun_header_ptr stun_new = std::make_shared<StunMessageHeader>();

    if (error_no == 420) {
        std::shared_ptr<ErrorCodeAttribute> error_code_attr = std::make_shared<ErrorCodeAttribute>();
        error_code_attr->set_error_code(error_no);
        error_code_attr->set_reason();

        stun_new->append_attribute(error_code_attr);

        std::shared_ptr<UnknownAttributesAttribute> uua = std::make_shared<UnknownAttributesAttribute>();
        uua->copy_attribute_vector(unknown_cr_attributes);

        stun_new->append_attribute(uua);
        send_stun_message(socket, stun_new);
    }
}

void StunServer::send_stun_message(QTcpSocket* socket , stun_header_ptr stun_message) {
    QByteArray block;
    QDataStream out_stream(&block, QIODevice::WriteOnly);
    out_stream.setVersion(QDataStream::Qt_5_0);
    stun_message->write_stun_message(out_stream);
    socket->write(block);
}

void StunServer::process_request_identify(stun_header_ptr message_orig, stun_header_ptr message_new) {
    PublicIdentifierAttribute* pia;
    for (auto&& attr : message_orig->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*)attr.get();
        }
    }

    std::cout << "Request for pid: " << pia->get_public_identifier() << std::endl;

    QHostAddress address;
    std::uint16_t port;
    rsa_public_optional* rsa_public;

    if (pia->get_public_identifier() == networking_->get_peer_public_id()) {
        address = networking_->ip_map_.my_ip.ipv4;
        port = PORT;
        rsa_public = &networking_->ip_map_.my_ip.key_pair.first;
    }
    else {
        address = networking_->ip_map_.get_ip4(pia->get_public_identifier());
        port = networking_->ip_map_.get_port(pia->get_public_identifier());
        rsa_public = networking_->ip_map_.get_rsa_public(pia->get_public_identifier()).get();
    }

    if (!rsa_public->has_value()) {
        create_response_error_identify(message_orig, message_new, pia->get_public_identifier());
    }
    else {
        create_response_success_identify(message_orig, message_new, pia->get_public_identifier(), address, port, rsa_public->value());
    }
}

void StunServer::create_response_success_identify(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t public_id, QHostAddress client_ipv4, std::uint16_t client_port, CryptoPP::RSA::PublicKey& key) {
    message_new->stun_method = StunMethodEnum::identify;
    message_new->stun_class = StunClassEnum::response_success;

    auto xraa = std::make_shared<XorRelayedAddressAttribute>();
    auto pia = std::make_shared<PublicIdentifierAttribute>();
    auto pka = std::make_shared<PublicKeyAttribute>();
    auto ria = std::make_shared<RelayedPublicIdentifierAttribute>();

    pia->initialize(public_id, message_new.get());
    xraa->initialize(message_new.get(), STUN_IPV4, client_ipv4, client_port);
    pka->initialize(key, message_new.get());
    ria->initialize(networking_->get_peer_public_id(), message_new.get());

    message_new->append_attribute(pia);
    message_new->append_attribute(xraa);
    message_new->append_attribute(ria);
    message_new->append_attribute(pka);

    message_new->copy_tid(message_orig);
}

void StunServer::create_response_error_identify(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t public_id) {
    message_new->stun_method = StunMethodEnum::identify;
    message_new->stun_class = StunClassEnum::response_error;

    auto pia = std::make_shared<PublicIdentifierAttribute>();

    pia->initialize(public_id, message_new.get());

    message_new->append_attribute(pia);

    message_new->copy_tid(message_orig);
}

void StunServer::process_request_allocate(stun_header_ptr message_orig, stun_header_ptr message_new, QTcpSocket* socket) {
        pk_t public_identifier;
        bool request_transport_found = false;
        std::uint32_t protocol;
        std::uint32_t lifetime = 600;
        CryptoPP::RSA::PublicKey pk;
        PublicKeyAttribute* pka;

        IpMap& ipm = networking_->ip_map_;

        for (auto&& attr : message_orig->attributes) {
            if (attr->attribute_type == StunAttributeEnum::requested_transport) {
                request_transport_found = true;

                protocol = ((RequestedTransportAttribute*)attr.get())->get_protocol();
            }
            if (attr->attribute_type == StunAttributeEnum::lifetime) {
                std::uint32_t temp;

                temp = ((LifetimeAttribute*)attr.get())->time;
                if (temp >= lifetime && temp <= MAX_TIME) {
                    lifetime = temp;
                }
                else if (temp > MAX_TIME) {
                    lifetime = MAX_TIME;
                }
            }
            if (attr->attribute_type == StunAttributeEnum::public_identifier) {
                public_identifier = ((PublicIdentifierAttribute*)attr.get())->get_public_identifier();
            }
            if (attr->attribute_type == StunAttributeEnum::public_key) {
                pka = (PublicKeyAttribute*)attr.get();
                pk = pka->get_value();
            }
        }

        if (!request_transport_found) {
            //TODO: error 400 (bad request)
        }

        if (protocol != IANA_TCP && protocol != IANA_UDP) {
            //TODO: error 442 (unsupported transport protocol)
        }

        /*auto address = networking_->ip_map_.get_ip4(public_identifier);
        auto port = networking_->ip_map_.get_port(public_identifier);
        auto rsa_public = networking_->ip_map_.get_rsa_public(public_identifier);*/

        if (networking_->ip_map_.have_ip4(public_identifier) && networking_->ip_map_.have_rsa_public(public_identifier)) {
            throw public_identifier_already_allocated("Sadly, this identifier is already allocated.");
            //TODO: send back error 437 (Allocation Mismatch)
        }
        else {
            networking_->ip_map_.update_ip(public_identifier, socket->peerAddress(), socket->peerPort());
            networking_->ip_map_.update_rsa_public(public_identifier, pk);

            networking_->ip_map_.set_tcp_socket(public_identifier, socket);
            std::cout << "Allocated: " << public_identifier << ", IP: " << ipm.get_ip4(public_identifier).toString().toStdString() << ", port: " << ipm.get_port(public_identifier) << std::endl;
        }

        create_response_success_allocate(message_orig, message_new, lifetime, socket);
}

void StunServer::create_response_success_allocate(stun_header_ptr message_orig, stun_header_ptr message_new, std::uint32_t lifetime, QTcpSocket* socket) {
    message_new->stun_class = StunClassEnum::response_success;
    message_new->stun_method = StunMethodEnum::allocate;
        
    message_new->copy_tid(message_orig);

    std::shared_ptr<XorMappedAddressAttribute> xma = std::make_shared<XorMappedAddressAttribute>();
    xma->initialize(message_new.get(), STUN_IPV4, socket);

    std::shared_ptr<LifetimeAttribute> la = std::make_shared<LifetimeAttribute>();
    la->initialize(lifetime, message_new.get());

    auto pia = std::make_shared<PublicIdentifierAttribute>();
    pia->initialize(networking_->get_peer_public_id(), message_new.get());

    message_new->append_attribute(xma);
    message_new->append_attribute(la);
    message_new->append_attribute(pia);

    
}