#include "StunServer.hpp"

StunServer::StunServer(Networking* n) : networking_(n) {
    init_server();
}

void StunServer::init_server() {
   

    /* ATTRIBUTE FACTORIES ARE INITIALIZED HERE */
    stun_attribute_factories.emplace(STUN_ATTR_XOR_MAPPED_ADDRESS, std::make_shared<XorMappedAddressAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_ERROR_CODE, std::make_shared<ErrorCodeAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_UNKNOWN_ATTRIBUTES, std::make_shared<UnknownAttributesAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_DATA, std::make_shared<DataAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_LIFETIME, std::make_shared<LifetimeAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_PUBLIC_IDENTIFIER, std::make_shared<PublicIdentifierAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_XOR_RELAYED_ADDRESS, std::make_shared<XorRelayedAddressAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_REQUESTED_TRANSPORT, std::make_shared<RequestedTransportAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_PUBLIC_KEY, std::make_shared<PublicKeyAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_RELAYED_PUBLIC_IDENTIFIER, std::make_shared<RelayedPublicIdentifierAttributeFactory>());

}

void StunServer::display_error() {
}

void StunServer::new_connection() {
    tcp_socket_ = tcp_server_->nextPendingConnection();
    tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);


    in_stream.setVersion(QDataStream::Qt_5_0);
    QObject::connect(tcp_socket_, &QIODevice::readyRead, this, &StunServer::reply);
    QObject::connect(tcp_socket_, &QAbstractSocket::disconnected, tcp_socket_, &QObject::deleteLater);
    // QObject::connect(tcp_socket_, &QAbstractSocket::disconnected, networking_, &Networking::peer_process_disconnected_users);
	QObject::connect(tcp_socket_, &QAbstractSocket::errorOccurred, this, &StunServer::display_error);
}

void StunServer::reply() {
    QTcpSocket* socket = (QTcpSocket*) QObject::sender();

    stun_header_ptr stun_message = std::make_shared<StunMessageHeader>();

    stun_attr_type_vec unknown_cr_attributes;
    in_stream.setDevice(socket);
    in_stream.startTransaction();

    do {
        try {
            stun_message->read_message_header(in_stream);
            stun_message->read_attributes(in_stream, stun_attribute_factories);
            handle_received_message(stun_message, socket);
        }
        catch (invalid_stun_message_format_error de) {
            in_stream.abortTransaction();
        }
        catch (unknown_comprehension_required_attribute_error u) {
            send_error(STUN_ERR_UNKNOWN_COMPREHENSION_REQ_ATTR, socket, socket->peerAddress(), socket->peerPort());
        }
    } while (socket->bytesAvailable() > 0);
}

void print_stun_messageqwe(stun_header_ptr stun_message_header) {
    std::cout << "STUN server " << std::flush;
    if (stun_message_header->stun_class == StunClassEnum::request) {
        std::cout << "request " << std::flush;
    }
    else if (stun_message_header->stun_class == StunClassEnum::response_success) {
        std::cout << "success response " << std::flush;
    }
    else if (stun_message_header->stun_class == StunClassEnum::response_error) {
        std::cout << "error response " << std::flush;
    }
    else if (stun_message_header->stun_class == StunClassEnum::indication) {
        std::cout << "indication " << std::flush;
    }
    else {
        std::cout << "unknown class " << std::flush;
    }
    if (stun_message_header->stun_method == StunMethodEnum::binding) {
        std::cout << "binding " << std::flush;
    }
    else if (stun_message_header->stun_method == StunMethodEnum::allocate) {
        std::cout << "allocate " << std::flush;
    }
    else if (stun_message_header->stun_method == StunMethodEnum::identify) {
        std::cout << "identify " << std::flush;
    }
    else if (stun_message_header->stun_method == StunMethodEnum::send) {
        std::cout << "send " << std::flush;
    }
    else {
        std::cout << "unknown method " << std::flush;
    }
    std::cout << std::endl;
}

void StunServer::handle_received_message(stun_header_ptr stun_message, QTcpSocket* socket) {
    stun_header_ptr stun_new = std::make_shared<StunMessageHeader>();
    print_stun_messageqwe(stun_message);
    if (stun_message->stun_class == StunClassEnum::request) {
        if (stun_message->stun_method == StunMethodEnum::binding) {
            // auto mp = MPProcess<CRequestTag, MBindingTag>(stun_message, stun_new, socket, unknown_cr_attributes);
            // MessageProcessor<CRequestTag, MBindingTag>::process(mp);
        }
        else if (stun_message->stun_method == StunMethodEnum::allocate) {
            process_request_allocate(stun_message, stun_new, socket);
        }
        else if (stun_message->stun_method == StunMethodEnum::identify) {
            process_request_identify(stun_message, stun_new);
        }
        else if (stun_message->stun_method == StunMethodEnum::send) {
            pk_t to;
            process_request_send(stun_message, stun_new, to);
            try {
                auto socket_sender = socket;
                auto& wrapper = networking_->ip_map_.get_wrapper_ref(to);
                socket = wrapper.tcp_socket_;
                if (wrapper.relay_state == RelayState::Relayed && wrapper.has_relay_stun_servers()) {
                    // networking_->add_waiting_stun_message(to, stun_new);

                }
                else if (!socket) {
                    create_response_error_send(stun_message, stun_new, STUN_ERR_USE_OTHER_SERVER, to);
                    send_stun_message(socket_sender, stun_new);
                    return;
                }
                else if (!socket->isValid() && wrapper.relay_state == RelayState::Direct) {
                    create_response_error_send(stun_message, stun_new, STUN_ERR_PEER_OFFLINE, to);
                }
                else {
                    stun_header_ptr stun_new_success_send = std::make_shared<StunMessageHeader>();
                    create_response_success_send(stun_message, stun_new_success_send, to);
                    send_stun_message(socket_sender, stun_new_success_send);
                }
            }
            catch (user_not_found_in_database unfid) {
                create_response_error_send(stun_message, stun_new, STUN_ERR_USE_OTHER_SERVER, to);
            }
        }
    }
    else if (stun_message->stun_class == StunClassEnum::indication) {
        if (stun_message->stun_method == StunMethodEnum::binding) {
            //no work here
        }
        else if (stun_message->stun_method == StunMethodEnum::send) {
            networking_->get_stun_client()->handle_received_message(stun_message, socket);
            return;
        }
    }
    else {
        throw invalid_stun_message_format_error("Invalid message received.");
    }

    send_stun_message(socket, stun_new);
}

void StunServer::send_error(std::size_t error_no, QTcpSocket* socket, QHostAddress address, quint16 port) {
    stun_header_ptr stun_new = std::make_shared<StunMessageHeader>();

    if (error_no == STUN_ERR_UNKNOWN_COMPREHENSION_REQ_ATTR) {
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
    PublicIdentifierAttribute* pia = NULL;
    XorRelayedAddressAttribute* xraa = NULL;
    for (auto&& attr : message_orig->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*)attr.get();
        }
        else if (attr->attribute_type == StunAttributeEnum::xor_relayed_address) {
            xraa = (XorRelayedAddressAttribute*)attr.get();
        }
    }

    if (pia) {

        auto my_public_id = networking_->get_peer_public_id();
        auto peer_to_identify = pia->get_public_identifier();
    
        if (peer_to_identify == my_public_id) {
            if (networking_->ip_map_.my_ip().key_pair.first.has_value()) 
            {
                create_response_success_identify(
                    message_orig, 
                    message_new,
                    peer_to_identify,
                    networking_->ip_map().my_ip().ipv4,
                    networking_->ip_map().my_ip().port,
                    networking_->ip_map().my_ip().stun_port,
                    networking_->ip_map().my_ip().key_pair.first.value()
                );
            }
            else {
                create_response_error_identify(message_orig, message_new, peer_to_identify);
            }
        }
        else 
        {

            try {
                IpWrapper& wrapper = networking_->ip_map().get_wrapper_ref(peer_to_identify);
                if (wrapper.has_ipv4() &&
                    wrapper.has_rsa()) 
                {
                    auto rsa = wrapper.get_rsa();
                    create_response_success_identify(
                        message_orig,
                        message_new,
                        peer_to_identify,
                        wrapper.ipv4,
                        wrapper.port,
                        wrapper.stun_port,
                        rsa
                    );
                }
            }
            catch(user_not_found_in_database e) {
                create_response_error_identify(message_orig, message_new, peer_to_identify);
            }
        }
    }
    else {
        auto rsa = networking_->ip_map_.my_ip().get_rsa();
        IpWrapper& wrapper = networking_->ip_map().my_ip();
        create_response_success_identify(
            message_orig,
            message_new,
            networking_->get_peer_public_id(),
            QHostAddress(xraa->get_address()),
            wrapper.port,
            wrapper.stun_port,
            rsa
        );
    }
}

void StunServer::create_response_success_identify(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t public_id, QHostAddress client_ipv4, std::uint16_t client_port, port_t client_stun_port, CryptoPP::RSA::PublicKey& key) {
    message_new->stun_method = StunMethodEnum::identify;
    message_new->stun_class = StunClassEnum::response_success;

    auto xraa = std::make_shared<XorRelayedAddressAttribute>();
    auto xmaa = std::make_shared<XorMappedAddressAttribute>();
    auto pia = std::make_shared<PublicIdentifierAttribute>();
    auto pka = std::make_shared<PublicKeyAttribute>();
    auto ria = std::make_shared<RelayedPublicIdentifierAttribute>();

    pia->initialize(public_id, message_new.get());
    xraa->initialize(message_new.get(), STUN_IPV4, client_ipv4, client_port);
    pka->initialize(key, message_new.get());
    ria->initialize(networking_->get_peer_public_id(), message_new.get());
    xmaa->initialize(message_new.get(), STUN_IPV4, client_ipv4, client_stun_port);

    message_new->append_attribute(pia);
    message_new->append_attribute(xraa);
    message_new->append_attribute(ria);
    message_new->append_attribute(pka);
    message_new->append_attribute(xmaa);

    message_new->copy_tid(message_orig);
}

void StunServer::create_response_error_identify(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t public_id) {
    message_new->stun_method = StunMethodEnum::identify;
    message_new->stun_class = StunClassEnum::response_error;

    auto peer_to_identify = std::make_shared<PublicIdentifierAttribute>();
    auto relayer = std::make_shared<RelayedPublicIdentifierAttribute>();

    peer_to_identify->initialize(public_id, message_new.get());
    relayer->initialize(networking_->peer_public_id, message_new.get());

    message_new->append_attribute(peer_to_identify);
    message_new->append_attribute(relayer);

    message_new->copy_tid(message_orig);
}

void StunServer::process_request_allocate(stun_header_ptr message_orig, stun_header_ptr message_new, QTcpSocket* socket) {
    pk_t public_identifier;
    bool request_transport_found = false;
    std::uint32_t protocol;
    std::uint32_t lifetime = 600;
    CryptoPP::RSA::PublicKey public_key;
    PublicKeyAttribute* pka;
    XorMappedAddressAttribute* np2ps_address_port;
    XorRelayedAddressAttribute* stun_address_port;

    IpMap& ipm = networking_->ip_map_;

    for (auto&& attr : message_orig->attributes) { //process attributes
        if (attr->attribute_type == StunAttributeEnum::requested_transport) {
            // request_transport_found = true;
            // protocol = ((RequestedTransportAttribute*)attr.get())->get_protocol();
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
            public_key = pka->get_value();
        }
        if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
            np2ps_address_port = (XorMappedAddressAttribute*) attr.get(); 
        }
        if (attr->attribute_type == StunAttributeEnum::xor_relayed_address) {
            stun_address_port = (XorRelayedAddressAttribute*) attr.get();
        }
    }

    try {
        auto& wrapper = networking_->ip_map().get_wrapper_ref(public_identifier);
        if (wrapper.has_ipv4() && wrapper.has_rsa()) {
            if (CryptoUtils::instance().rsa_to_hex(public_key) == wrapper.get_rsa_hex_string()) {
                if (!GlobalMethods::ip_address_is_private(socket->peerAddress())) {
                    wrapper.ipv4 = socket->peerAddress();
                }
                else {
                    wrapper.ipv4 = QHostAddress(np2ps_address_port->get_address());
                }
                wrapper.port = np2ps_address_port->get_port();
                wrapper.stun_port = stun_address_port->get_port();
                networking_->ip_map_.set_tcp_socket(public_identifier, socket);
                create_response_success_allocate(message_orig, message_new, lifetime, wrapper);
                return;
            }
            else {
                create_response_error_allocate(message_orig, message_new);
                return;
            }
        }
    }
    catch (user_not_found_in_database& unfid) {
        networking_->ip_map().add_to_ip_map(
            public_identifier, 
            IpWrapper(
                socket->peerAddress(),
                np2ps_address_port->get_port(),
                stun_address_port->get_port()
            )
        );
    }

    IpWrapper& wrapper = networking_->ip_map().get_wrapper_ref(public_identifier);

    networking_->ip_map_.update_rsa_public(public_identifier, public_key);

    networking_->ip_map_.set_tcp_socket(public_identifier, socket);

    create_response_success_allocate(message_orig, message_new, lifetime, wrapper);
}

void StunServer::create_response_success_allocate(stun_header_ptr message_orig, stun_header_ptr message_new, std::uint32_t lifetime, IpWrapper& wrapper) {
    message_new->stun_class = StunClassEnum::response_success;
    message_new->stun_method = StunMethodEnum::allocate;
        
    message_new->copy_tid(message_orig);

    std::shared_ptr<XorMappedAddressAttribute> xma = std::make_shared<XorMappedAddressAttribute>(); //transport address of the sender
    xma->initialize(message_new.get(), STUN_IPV4, wrapper.ipv4, wrapper.port);

    std::shared_ptr<LifetimeAttribute> la = std::make_shared<LifetimeAttribute>();
    la->initialize(lifetime, message_new.get());

    auto pia = std::make_shared<PublicIdentifierAttribute>();
    pia->initialize(networking_->get_peer_public_id(), message_new.get());

    auto& news = networking_->news_db->at(networking_->get_peer_public_id());
    auto pka = std::make_shared<PublicKeyAttribute>();
    pka->initialize(news.get_newspaper_public_key_value(), message_new.get());

    message_new->append_attribute(xma);
    message_new->append_attribute(la);
    message_new->append_attribute(pia);
    message_new->append_attribute(pka);    
}

void StunServer::process_request_send(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t& to) {
    RelayedPublicIdentifierAttribute* receiver;
    PublicIdentifierAttribute* sender;
    DataAttribute* data;

    for (auto&& attr : message_orig->attributes) {
        if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) {
            receiver = (RelayedPublicIdentifierAttribute*) attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            sender = (PublicIdentifierAttribute*) attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::data) {
            data = (DataAttribute*) attr.get();
        }
    }

    std::cout << "Request send from " << sender->get_public_identifier() << " to " << receiver->get_public_identifier() << std::endl;

    to = receiver->get_public_identifier();
    create_indication_send(message_orig, message_new, sender->get_public_identifier(), std::move(data->get_np2ps_messsage()));
}

void StunServer::create_indication_send(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t source_pk, std::string&& np2ps_message) {
    message_new->stun_class = StunClassEnum::indication;
    message_new->stun_method = StunMethodEnum::send;

    auto pia = std::make_shared<PublicIdentifierAttribute>();
    pia->initialize(source_pk, message_new.get());
    message_new->append_attribute(pia);

    auto ria = std::make_shared<RelayedPublicIdentifierAttribute>();
    ria->initialize(networking_->get_peer_public_id(), message_new.get());
    message_new->append_attribute(ria);

    auto data = std::make_shared<DataAttribute>();
    auto qa = QByteArray::fromStdString(np2ps_message);
    data->initialize(qa, message_new.get());
    message_new->append_attribute(data);

    message_new->copy_tid(message_orig);
}

void StunServer::create_response_error_send(stun_header_ptr message_orig, stun_header_ptr message_new, std::uint16_t code, pk_t target_pid) {
    message_new->stun_class = StunClassEnum::response_error;
    message_new->stun_method = StunMethodEnum::send;

    auto pia = std::make_shared<PublicIdentifierAttribute>();
    pia->initialize(target_pid, message_new.get());
    message_new->append_attribute(pia);

    auto ria = std::make_shared<RelayedPublicIdentifierAttribute>();
    ria->initialize(networking_->get_peer_public_id(), message_new.get());
    message_new->append_attribute(ria);

    auto err = std::make_shared<ErrorCodeAttribute>();
    err->initialize(0, message_new.get());
    err->set_error_code(code);
    err->set_reason();
    message_new->append_attribute(err);

    message_new->copy_tid(message_orig);
}

void StunServer::create_response_success_send(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t target_pid) {
    message_new->stun_class = StunClassEnum::response_success;
    message_new->stun_method = StunMethodEnum::send;

    auto pia = std::make_shared<PublicIdentifierAttribute>();
    pia->initialize(target_pid, message_new.get());
    message_new->append_attribute(pia);

    auto ria = std::make_shared<RelayedPublicIdentifierAttribute>();
    ria->initialize(networking_->get_peer_public_id(), message_new.get());
    message_new->append_attribute(ria);

    message_new->copy_tid(message_orig);
}

void StunServer::start_server(QHostAddress address, std::uint16_t port) {
    if (!tcp_server_) {
        tcp_server_ = new QTcpServer(this);
    }
    if (tcp_server_->isListening()) {
        tcp_server_->close();
    }

    if (!tcp_server_->listen(QHostAddress::AnyIPv4, (quint16) port)) {
        std::cout << "STUN Server failed to start" << std::endl;
        return;
    }
    else {
        std::cout << "STUN Server is running on IP: " 
            << address.toString().toStdString() 
            << " and port: " << port << std::endl;
    }

    tcp_server_->disconnect();
    QObject::connect(tcp_server_, &QTcpServer::newConnection, this, &StunServer::new_connection);
    server_started = true;

    if (port != STUN_PORT) {
		IpWrapper& my_wrapper = networking_->ip_map().my_ip();
		my_wrapper.stun_port = port;
	}
}

void StunServer::create_response_error_allocate(stun_header_ptr message_orig, stun_header_ptr message_new) {
    message_new->stun_class = StunClassEnum::response_error;
    message_new->stun_method = StunMethodEnum::allocate;

    auto ria = std::make_shared<RelayedPublicIdentifierAttribute>();
    ria->initialize(networking_->get_peer_public_id(), message_new.get());
    message_new->append_attribute(ria);

    message_new->copy_tid(message_orig);
}
