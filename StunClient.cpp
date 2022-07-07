#include "StunClient.hpp"

StunClient::StunClient(Networking* networking) {
    tcp_socket_ = new QTcpSocket(this);

    in.setDevice(tcp_socket_);
    in.setVersion(QDataStream::Qt_5_0);

    connect(tcp_socket_, &QIODevice::readyRead, this, &StunClient::receive_msg);
    connect(tcp_socket_, &QAbstractSocket::errorOccurred, this, &StunClient::error);
    connect(tcp_socket_, &QAbstractSocket::disconnected, this, &QObject::deleteLater);

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

    stun_server.first = QHostAddress(QString("10.19.160.208"));
    stun_server.second = STUN_PORT;
    networking_ = networking;
}

StunClient::StunClient(Networking* networking, QHostAddress address, std::uint16_t port_stun) : StunClient::StunClient(networking) {
    stun_server.first = address;
    stun_server.second = port_stun;
}

void printQByteArray(QByteArray& a) {
    std::cout << "0          1          2          3 " << std::endl;
    std::cout << "01234567 89012345 67890123 45678901" << std::endl;
    short counter = 0;
    for (char c : a) {
        std::bitset<8> b(c);
        std::cout << b << ' ';
        counter++;
        if (counter >= 4) {
            std::cout << std::endl;
            counter = 0;
        }
    }
}

void StunClient::send_stun_message(stun_header_ptr stun_message, pk_t public_id) {
    std::cout << "STUN client: sending STUN message" << std::endl;

    if (!networking_->ip_map_.have_ip4(public_id)) {
        throw std::logic_error("Missing public_id of provided STUN server.");
    }

    auto addr = networking_->ip_map_.get_ip4(public_id);
    auto port = networking_->ip_map_.get_port(public_id);
    
    if (networking_->ip_map_.get_tcp_socket(public_id) && networking_->ip_map_.get_tcp_socket(public_id)->isOpen()) {
        tcp_socket_ = networking_->ip_map_.get_tcp_socket(public_id);
    }
    else {
        tcp_socket_->abort();
        tcp_socket_->connectToHost(addr, port);

        if (!tcp_socket_->waitForConnected(10000))
            throw std::logic_error("Connection to STUN server timed-out. (10 seconds)");
    }

    if (stun_message->stun_class == StunClassEnum::request && stun_message->stun_method == StunMethodEnum::allocate) {
        tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
        networking_->ip_map_.set_tcp_socket(public_id, tcp_socket_);
    }

    QByteArray block;
    QDataStream out_stream(&block, QIODevice::WriteOnly);
    out_stream.setVersion(QDataStream::Qt_5_0);
    stun_message->write_stun_message(out_stream);
    //stun_message->print_message();
    //printQByteArray(block);
    int b = 0;
    if ((b = tcp_socket_->write(block)) == -1)
        std::cout << "SC: Error occured while writing the block" << std::endl;
    else
        std::cout << "SC: bytes written: " << b << std::endl;

    
}

void StunClient::send_stun_message_transport_address(stun_header_ptr stun_message, QHostAddress address, std::uint16_t port) {
    tcp_socket_->abort();
        tcp_socket_->connectToHost(address, port);

        if (!tcp_socket_->waitForConnected(10000))
            throw std::logic_error("Connection to STUN server timed-out. (10 seconds)");
    QByteArray block;
    QDataStream out_stream(&block, QIODevice::WriteOnly);
    out_stream.setVersion(QDataStream::Qt_5_0);
    stun_message->write_stun_message(out_stream);
    //stun_message->print_message();
    //printQByteArray(block);
    int b = 0;
    if ((b = tcp_socket_->write(block)) == -1)
        std::cout << "SC: Error occured while writing the block" << std::endl;
    else
        std::cout << "SC: bytes written: " << b << std::endl;
}

/**
 * @brief Handles received STUN message.
 * 
 * @param stun_message_header Message received.
 */
void StunClient::handle_received_message(stun_header_ptr stun_message_header) {
    if (stun_message_header->stun_class == StunClassEnum::response_success) {
        if (stun_message_header->stun_method == StunMethodEnum::binding) {
            QString client_IP("lol");
            MPProcess<CResponseSuccessTag, MBindingTag> mpps(stun_message_header, client_IP);
            MessageProcessor<CResponseSuccessTag, MBindingTag>::process(mpps);
            std::cout << "IP: " << client_IP.toStdString() << ", port: " << mpps.port << std::endl;
        }
        else if (stun_message_header->stun_method == StunMethodEnum::allocate) {
            MPProcess<CResponseSuccessTag, MAllocateTag> mp(stun_message_header, &networking_->ip_map_);
            MessageProcessor<CResponseSuccessTag, MAllocateTag>::process(mp);
        }
        else if (stun_message_header->stun_method == StunMethodEnum::identify) {
            process_response_success_identify(stun_message_header);
        }
    }
    else if (stun_message_header->stun_class == StunClassEnum::response_error) {
        if (stun_message_header->stun_method == StunMethodEnum::binding) {
            MPProcess<CResponseErrorTag, MBindingTag> mppe(stun_message_header);
            MessageProcessor<CResponseErrorTag, MBindingTag>::process(mppe);
        }
        else if (stun_message_header->stun_method == StunMethodEnum::identify) {
            process_response_error_identify(stun_message_header);
        }
    }
    else if (stun_message_header->stun_class == StunClassEnum::indication) {
        if (stun_message_header->stun_method == StunMethodEnum::send) {
            std::string np2ps_message;
            MPProcess<CIndicationTag, MSendTag> mpp(stun_message_header);
            MessageProcessor<CIndicationTag, MSendTag>::process(mpp);
            QByteArray msg(np2ps_message.c_str(), np2ps_message.length());
            networking_->pass_message_to_receiver(msg, QHostAddress(mpp.from_address), mpp.from_port);
        }
    }
    else {
        throw invalid_stun_message_format_error("Client received an invalid class type...");
    }
}

/**
 * @brief Receives STUN messsage from network.
 */
void StunClient::receive_msg() {
    std::cout << "SC: message received" << std::endl;
    QDataStream in_stream;
    in_stream.setVersion(QDataStream::Qt_5_0);
    in_stream.setDevice(tcp_socket_);
    in_stream.startTransaction();
    
    stun_header_ptr stun_message_header = std::make_shared<StunMessageHeader>();
    try {
        stun_message_header->read_message_header(in_stream);
        stun_message_header->read_attributes(in_stream, stun_attribute_factories);
    }
    catch(invalid_stun_message_format_error& i) {
        std::cout << i.what() << std::endl;
        in_stream.abortTransaction();
    }
    in_stream.commitTransaction();
    handle_received_message(stun_message_header);
}

/**
 * @brief Creates and sends binding request message to STUN server.
 * 
 */
void StunClient::binding_request() {
    stun_header_ptr msg = std::make_shared<StunMessageHeader>();
    create_binding_request(msg);
    send_stun_message_transport_address(msg, stun_server.first, stun_server.second);

    //receive_msg();
}

void StunClient::allocate_request(pk_t where) {
    stun_header_ptr msg = std::make_shared<StunMessageHeader>();
    create_request_allocate(msg, 600, networking_->get_peer_public_id());
    send_stun_message(msg, where);
}

void StunClient::create_binding_request(stun_header_ptr stun_msg) {
    MPCreate<CRequestTag, MBindingTag> mpc(rng, stun_msg);
    MessageProcessor<CRequestTag, MBindingTag>::create(mpc);
}

void StunClient::accept() {
    in.startTransaction();

    stun_header_ptr stun_reply = std::make_shared<StunMessageHeader>();

    try {
        stun_reply->read_message_header(in);
        stun_reply->read_attributes(in, stun_attribute_factories);
    }
    catch (invalid_stun_message_format_error de) {
        in.abortTransaction();
        std::cout << de.what() << std::endl;
    }
}

void StunClient::error(QAbstractSocket::SocketError socketError) {

}

void StunClient::init_client(QHostAddress address, std::uint16_t port) {

}

void StunClient::create_request_identify(stun_header_ptr stun_message, pk_t who) {
    stun_message->stun_class = StunClassEnum::request;
    stun_message->stun_method = StunMethodEnum::identify;

    auto pia = std::make_shared<PublicIdentifierAttribute>();

    pia->initialize(who, stun_message.get());

    stun_message->append_attribute(pia);
    stun_message->generate_tid(rng);
}

void StunClient::process_response_success_identify(stun_header_ptr stun_message) {
    PublicIdentifierAttribute* pia;
    XorRelayedAddressAttribute* xraa;
    RelayedPublicIdentifierAttribute* ria;
    PublicKeyAttribute* pka;
    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::xor_relayed_address) {
            xraa = (XorRelayedAddressAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) {
            ria = (RelayedPublicIdentifierAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::public_key) {
            pka = (PublicKeyAttribute*)attr.get();
        }
    }
    networking_->ip_map_.update_ip(pia->get_public_identifier(), QHostAddress(xraa->get_address()), xraa->get_port());
    networking_->ip_map_.get_wrapper_for_pk(pia->get_public_identifier())->second.preferred_stun_server = ria->get_public_identifier();
    networking_->ip_map_.update_rsa_public(pia->get_public_identifier(), pka->get_value());

    auto find_it_waiting_ip = networking_->waiting_ip.find(pia->get_public_identifier());

    if (find_it_waiting_ip != networking_->waiting_ip.end()) {
        auto msg = find_it_waiting_ip->second;
        networking_->waiting_ip.erase(find_it_waiting_ip);
        networking_->send_message(msg);
    }
}

void StunClient::process_response_error_identify(stun_header_ptr stun_message) {
    PublicIdentifierAttribute* pia;
    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*)attr.get();
        }
    }
    std::cout << "Identifier: " << pia->get_public_identifier() << " was not found." << std::endl;
}

void StunClient::create_request_allocate(stun_header_ptr stun_message, std::uint32_t lifetime, pk_t public_id) {
    stun_message->stun_class = StunClassEnum::request;
    stun_message->stun_method = StunMethodEnum::allocate;

    std::shared_ptr<RequestedTransportAttribute> rta = std::make_shared<RequestedTransportAttribute>();
    rta->initialize(IANA_TCP, stun_message.get());
    stun_message->append_attribute(rta);

    auto pia = std::make_shared<PublicIdentifierAttribute>();
    pia->initialize(public_id, stun_message.get());
    stun_message->append_attribute(pia);

    if (lifetime != 600) {
        std::shared_ptr<LifetimeAttribute> la = std::make_shared<LifetimeAttribute>();
        la->initialize(lifetime, stun_message.get());
        stun_message->append_attribute(la);
    }

    stun_message->generate_tid(rng);
}

void StunClient::process_response_success_allocate(QTcpSocket* tcp_socket, stun_header_ptr message_orig) {
    XorMappedAddressAttribute* xma;
    LifetimeAttribute* la;
    PublicIdentifierAttribute* pia;
    for (auto&& attr : message_orig->attributes) {
        if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
            xma = (XorMappedAddressAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::lifetime) {
            la = (LifetimeAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*)attr.get();
        }
    }
    networking_->ip_map_.my_ip.ipv4 = QHostAddress(xma->get_address());
    networking_->ip_map_.my_ip.port = xma->get_port();
    add_stun_server(tcp_socket->peerAddress(), tcp_socket->peerPort(), pia->get_public_identifier());
    emit confirmed_newspaper(pia->get_public_identifier());
}

void StunClient::check_for_nat() {
    auto msg = std::make_shared<StunMessageHeader>();
    create_binding_request(msg);
    send_stun_message(msg, get_stun_server_any());
}

void StunClient::process_response_success_binding(stun_header_ptr stun_message, QTcpSocket* socket_) {
    XorMappedAddressAttribute* xmaa;
    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
            xmaa = (XorMappedAddressAttribute*) attr.get();
        }
    }
    if ((socket_->localAddress() == QHostAddress(xmaa->get_address())) && (socket_->localPort() == xmaa->get_port())) {}
    else {
        nat_active_flag = true;
    }
    networking_->ip_map_.my_ip.ipv4 = QHostAddress(xmaa->get_address());
    networking_->ip_map_.my_ip.port = xmaa->get_port();
}

void StunClient::identify(pk_t who) {
    auto msg = std::make_shared<StunMessageHeader>();
    create_request_identify(msg, who);
    auto preferred_stun_server = networking_->ip_map_.get_wrapper_for_pk(who)->second.preferred_stun_server;
    if (preferred_stun_server == 0) {
        send_stun_message(msg, get_stun_server_any());
    }
    else {
        send_stun_message(msg, preferred_stun_server);
    }
}

pk_t StunClient::get_stun_server_any() {
    return stun_servers[0];
}

void StunClient::stun_server_connected() {
    std::cout << "STUN server added and connected for: " << stun_server_awaiting_confirmation << std::endl;
    QTcpSocket* socket = (QTcpSocket*) QObject::sender();
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    networking_->ip_map_.set_tcp_socket(stun_server_awaiting_confirmation, socket);
    stun_servers.push_back(stun_server_awaiting_confirmation);
}

void StunClient::stun_server_connection_error() {
    std::cout << "STUN connection to " << stun_server_awaiting_confirmation << " failed." << std::endl;
}

void StunClient::add_stun_server(QHostAddress address, std::uint16_t port, pk_t pid) {
    networking_->ip_map_.update_ip(pid, address, port);
    QTcpSocket* tcp_socket = new QTcpSocket(this);

    QObject::connect(tcp_socket, &QIODevice::readyRead, this, &StunClient::receive_msg);
    QObject::connect(tcp_socket, &QAbstractSocket::disconnected, this, &QObject::deleteLater);

    stun_server_awaiting_confirmation = pid;

    auto c1 = QObject::connect(tcp_socket, &QAbstractSocket::errorOccurred, this, &StunClient::stun_server_connection_error);
    auto c2 = QObject::connect(tcp_socket, &QAbstractSocket::connected, this, &StunClient::stun_server_connected);

    tcp_socket->connectToHost(address, port);

    QObject::disconnect(c1);
    QObject::disconnect(c2);
}