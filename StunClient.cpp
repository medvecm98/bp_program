#include "StunClient.hpp"

StunClient::StunClient(Networking* networking) {
    

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

    networking_ = networking;
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

void StunClient::send_stun_message(stun_header_ptr stun_message) {
    send_stun_message(stun_message, get_stun_server_front());
}

void StunClient::send_stun_message(stun_header_ptr stun_message, pk_t public_id) {
    QTcpSocket* socket;

    if (!networking_->ip_map_.have_ip4(public_id)) {
        throw std::logic_error("Missing public_id of provided STUN server.");
    }

    auto addr = networking_->ip_map_.get_ip4(public_id);
    auto port = STUN_PORT;
    
    if (networking_->ip_map_.get_tcp_socket(public_id) && networking_->ip_map_.get_tcp_socket(public_id)->isValid()) {
        socket = networking_->ip_map_.get_tcp_socket(public_id);

        QByteArray block;
        QDataStream out_stream(&block, QIODevice::WriteOnly);
        out_stream.setVersion(QDataStream::Qt_5_0);
        stun_message->write_stun_message(out_stream);
        if (socket->write(block) == -1)
            std::cout << "SC: Error occured while writing the block" << std::endl;

    }
    else {
        socket = new QTcpSocket(this);

        QObject::connect(socket, &QIODevice::readyRead, this, &StunClient::receive_msg);
        QObject::connect(socket, &QAbstractSocket::errorOccurred, this, &StunClient::error);
        QObject::connect(socket, &QAbstractSocket::disconnected, socket, &QObject::deleteLater);
        QObject::connect(socket, &QAbstractSocket::disconnected, networking_, &Networking::peer_process_disconnected_users);
        QObject::connect(socket, &QAbstractSocket::connected, this, &StunClient::host_connected);

        address_waiting_to_connect = addr;
        port_waiting_to_connect = port;
        header_waiting_to_connect = stun_message;
        connecting_to = public_id;
        save_socket = true;
        message_waiting_to_connect = true;

        socket->connectToHost(addr, port);
    }

    
}

void StunClient::send_stun_message_transport_address(stun_header_ptr stun_message, QHostAddress address, std::uint16_t port) {
    QTcpSocket* socket = new QTcpSocket(this);
    connect(socket, &QAbstractSocket::errorOccurred, this, &StunClient::error);
    connect(socket, &QIODevice::readyRead, this, &StunClient::receive_msg);
    QObject::connect(socket, &QAbstractSocket::disconnected, socket, &QObject::deleteLater);
    QObject::connect(socket, &QAbstractSocket::disconnected, networking_, &Networking::peer_process_disconnected_users);
    QObject::connect(socket, &QAbstractSocket::connected, this, &StunClient::host_connected);

    address_waiting_to_connect = address;
    port_waiting_to_connect = port;
    header_waiting_to_connect = stun_message;
    message_waiting_to_connect = true;
    
    socket->connectToHost(address, port);
}

void StunClient::host_connected() {
    auto socket = (QTcpSocket*) QObject::sender();

    failed_connections = 0;
    clean_failed_connection_for_server(get_stun_server_front());

    auto mtemp = header_waiting_to_connect;
    auto atemp = address_waiting_to_connect;

    if (save_socket) {
        auto pktemp = connecting_to;
        if (mtemp->stun_class == StunClassEnum::request && mtemp->stun_method == StunMethodEnum::allocate) {
            socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
            networking_->ip_map_.set_tcp_socket(pktemp, socket);
        }
        save_socket = false;
    }

    message_waiting_to_connect = false;


    QByteArray block;
    QDataStream out_stream(&block, QIODevice::WriteOnly);
    out_stream.setVersion(QDataStream::Qt_5_0);
    mtemp->write_stun_message(out_stream);
    if (socket->write(block) == -1)
        std::cout << "SC: Error occured while writing the block" << std::endl;
}

void print_stun_message(stun_header_ptr stun_message_header) {
    std::cout << "STUN " << std::flush;
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

/**
 * @brief Handles received STUN message.
 * 
 * @param stun_message_header Message received.
 */
void StunClient::handle_received_message(stun_header_ptr stun_message_header, QTcpSocket* socket) {
    print_stun_message(stun_message_header);
    if (stun_message_header->stun_class == StunClassEnum::response_success) {
        if (stun_message_header->stun_method == StunMethodEnum::binding) {
            // QString client_IP;
            // MPProcess<CResponseSuccessTag, MBindingTag> mpps(stun_message_header, client_IP);
            // MessageProcessor<CResponseSuccessTag, MBindingTag>::process(mpps);
            // std::cout << "IP: " << client_IP.toStdString() << ", port: " << mpps.port << std::endl;
        }
        else if (stun_message_header->stun_method == StunMethodEnum::allocate) {
            process_response_success_allocate(socket, stun_message_header);
        }
        else if (stun_message_header->stun_method == StunMethodEnum::identify) {
            process_response_success_identify(stun_message_header);
        }
    }
    else if (stun_message_header->stun_class == StunClassEnum::response_error) {
        if (stun_message_header->stun_method == StunMethodEnum::binding) {
            // MPProcess<CResponseErrorTag, MBindingTag> mppe(stun_message_header);
            // MessageProcessor<CResponseErrorTag, MBindingTag>::process(mppe);
        }
        else if (stun_message_header->stun_method == StunMethodEnum::identify) {
            process_response_error_identify(stun_message_header);
        }
        else if (stun_message_header->stun_method == StunMethodEnum::allocate) {
            process_response_error_allocate(stun_message_header);
        }
    }
    else if (stun_message_header->stun_class == StunClassEnum::indication) {
        if (stun_message_header->stun_method == StunMethodEnum::send) {
            std::string np2ps_message;
            pk_t relayer = process_indication_send(stun_message_header, np2ps_message);
            QByteArray msg(np2ps_message.c_str(), np2ps_message.length());
            QDataStream stream(msg);
            networking_->pass_message_to_receiver(stream, relayer);
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
    QTcpSocket* socket = (QTcpSocket*)QObject::sender();
    QDataStream in_stream;
    in_stream.setVersion(QDataStream::Qt_5_0);
    in_stream.setDevice(socket);
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
    handle_received_message(stun_message_header, socket);
}

void StunClient::allocate_request(pk_t where) {
    stun_header_ptr msg = std::make_shared<StunMessageHeader>();
    create_request_allocate(msg, 600, networking_->get_peer_public_id());
    send_stun_message(msg, where);
}

void StunClient::create_binding_request(stun_header_ptr stun_msg) {
    // MPCreate<CRequestTag, MBindingTag> mpc(rng, stun_msg);
    // MessageProcessor<CRequestTag, MBindingTag>::create(mpc);
}

void StunClient::add_failed_connection_for_server(pk_t pid) {
    if (failed_connections_per_server.count(pid) > 0) {
        failed_connections_per_server[pid]++;
    }
    else {
        failed_connections_per_server.emplace(pid, 1);
    }
}

void StunClient::clean_failed_connection_for_server(pk_t pid) {
    failed_connections_per_server.erase(pid);
}

void StunClient::error(QAbstractSocket::SocketError socket_error) {
    QTcpSocket* socket = (QTcpSocket*)QObject::sender();
    std::cout << "Connection to STUN server failed:" << socket->isValid() << std::endl;
    std::cout << "Error number: " << socket_error << std::endl;
    add_failed_connection_for_server(get_stun_server_front());
    get_stun_server_next();
    if (++failed_connections < stun_servers.size()) {
        std::cout << "Reconnecting to other STUN server" << std::endl;
        send_stun_message(header_waiting_to_connect);
    }
    else {
        std::cout << "Connection to all STUN servers failed." << std::endl;
    }
}

void StunClient::init_client(QHostAddress address, std::uint16_t port) {

}

void StunClient::create_request_identify(stun_header_ptr& stun_message, pk_t who) {
    stun_message->stun_class = StunClassEnum::request;
    stun_message->stun_method = StunMethodEnum::identify;

    auto pia = std::make_shared<PublicIdentifierAttribute>();

    pia->initialize(who, stun_message.get());

    stun_message->append_attribute(pia);
    stun_message->generate_tid(rng);
}

void StunClient::create_request_identify_empty(stun_header_ptr& stun_message) {
    stun_message->stun_class = StunClassEnum::request;
    stun_message->stun_method = StunMethodEnum::identify;

    stun_message->generate_tid(rng);
}

void StunClient::create_request_identify_address(stun_header_ptr& stun_message, QHostAddress address) {
    stun_message->stun_class = StunClassEnum::request;
    stun_message->stun_method = StunMethodEnum::identify;

    auto xmaa = std::make_shared<XorRelayedAddressAttribute>();
    xmaa->initialize(stun_message.get(), STUN_IPV4, address, 14128);
    stun_message->append_attribute(xmaa);

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
        else if (attr->attribute_type == StunAttributeEnum::xor_relayed_address) {
            xraa = (XorRelayedAddressAttribute*)attr.get();
        }
        else if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) {
            ria = (RelayedPublicIdentifierAttribute*)attr.get();
        }
        else if (attr->attribute_type == StunAttributeEnum::public_key) {
            pka = (PublicKeyAttribute*)attr.get();
        }
    }
    networking_->ip_map_.update_ip(pia->get_public_identifier(), QHostAddress(xraa->get_address()), xraa->get_port());
    networking_->ip_map_.get_wrapper_for_pk(pia->get_public_identifier())->second.preferred_stun_server = ria->get_public_identifier();
    networking_->ip_map_.update_rsa_public(pia->get_public_identifier(), pka->get_value());
    networking_->ip_map_.check_or_add_to_ip_map_relayed(
        pia->get_public_identifier(),
        ria->get_public_identifier()
    );

    auto find_it_waiting_ip = networking_->waiting_ip.find(pia->get_public_identifier());

    if (find_it_waiting_ip != networking_->waiting_ip.end()) { //found peer that needed to be identified
        auto msg = find_it_waiting_ip->second;
        networking_->waiting_ip.erase(find_it_waiting_ip);

        networking_->send_message_with_credentials(msg, true); //...send him the message
    }

    auto find_it_waiting_sym_key = networking_->waiting_symmetric_key_messages.find(pia->get_public_identifier());

    if (find_it_waiting_sym_key != networking_->waiting_symmetric_key_messages.end()) {
        networking_->ip_map_.update_ip(pia->get_public_identifier(), QHostAddress(xraa->get_address()), xraa->get_port());
        networking_->ip_map_.update_rsa_public(pia->get_public_identifier(), pka->get_value());
        auto msg  =find_it_waiting_sym_key->second;
        networking_->waiting_symmetric_key_messages.erase(find_it_waiting_sym_key);
        networking_->new_message_received(msg);
    }

    auto find_it_newspapers_awaiting_identification = networking_->newspapers_awaiting_identification.find(xraa->get_address());
    if (find_it_newspapers_awaiting_identification != networking_->newspapers_awaiting_identification.end()) {
        auto name = find_it_newspapers_awaiting_identification->second;
        auto ip = QHostAddress(find_it_newspapers_awaiting_identification->first).toString().toStdString();
        networking_->newspapers_awaiting_identification.erase(find_it_newspapers_awaiting_identification);
        emit networking_->newspaper_identified(ria->get_public_identifier(), name, ip);
        return;
    }


    emit confirmed_newspaper(pia->get_public_identifier());
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

void StunClient::create_request_send(stun_header_ptr stun_message, QByteArray& msg, pk_t where) {
    stun_message->stun_class = StunClassEnum::request;
    stun_message->stun_method = StunMethodEnum::send;

    auto pia = std::make_shared<PublicIdentifierAttribute>(); //me
    pia->initialize(networking_->get_peer_public_id(), stun_message.get());
    stun_message->append_attribute(pia);

    auto ria = std::make_shared<RelayedPublicIdentifierAttribute>();//other side
    ria->initialize(where, stun_message.get());
    stun_message->append_attribute(ria);

    auto data = std::make_shared<DataAttribute>();
    data->initialize(msg, stun_message.get());
    stun_message->append_attribute(data);
    
    stun_message->generate_tid(rng);
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

    auto pka = std::make_shared<PublicKeyAttribute>();
    pka->initialize(networking_->ip_map_.my_ip().key_pair.first.value(), stun_message.get());
    stun_message->append_attribute(pka);

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
    PublicKeyAttribute* pka = NULL;
    for (auto&& attr : message_orig->attributes) {
        if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
            xma = (XorMappedAddressAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::lifetime) {
            // la = (LifetimeAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::public_key) {
            pka = (PublicKeyAttribute*)attr.get();
        }
    }
    networking_->ip_map_.my_ip().ipv4 = QHostAddress(xma->get_address());
    networking_->ip_map_.my_ip().port = xma->get_port();
    add_stun_server(tcp_socket, pia->get_public_identifier());
    if (pka) {
        emit confirmed_newspaper_pk(pia->get_public_identifier(), rsa_public_optional(pka->get_value()));
    }
    else {
        emit confirmed_newspaper(pia->get_public_identifier());
    }
}

void StunClient::process_response_success_binding(stun_header_ptr stun_message, QTcpSocket* socket_) {
    XorMappedAddressAttribute* xmaa = NULL;
    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
            xmaa = (XorMappedAddressAttribute*) attr.get();
        }
    }
    if (xmaa) {
        if ((socket_->localAddress() == QHostAddress(xmaa->get_address())) && (socket_->localPort() == xmaa->get_port())) {}
        else {
            nat_active_flag = true;
        }
        networking_->ip_map_.my_ip().ipv4 = QHostAddress(xmaa->get_address());
        networking_->ip_map_.my_ip().port = xmaa->get_port();
    }
}

void StunClient::identify(pk_t who) {
    auto msg = std::make_shared<StunMessageHeader>();

    create_request_identify(msg, who);
    for (int i = 0; i < stun_servers.size(); i++) {
        stun_servers.push(stun_servers.front());
        send_stun_message(msg, stun_servers.front());
        stun_servers.pop();
    }
    // try {
    //     IpWrapper& ipw = networking_->ip_map().get_wrapper_ref(who);
    //     auto preferred_stun_server = ipw.preferred_stun_server;
    //     if (preferred_stun_server == 0) {
    //         send_stun_message(msg, get_stun_server_front());
    //     }
    //     else {
    //         send_stun_message(msg, preferred_stun_server);
    //     }
    // }
    // catch (user_not_found_in_database& e) {
    //     std::cout << "Failed to connect to peer "
    //               << who
    //               << " when attempting to establish STUN connection. "
    //               << e.what()
    //               << std::endl;
    //     std::cout << "Sending to another STUN server." << std::endl;
    //     send_stun_message(msg, get_stun_server_next());
    // }
}

void StunClient::identify(pk_t who, pk_t where) {
    auto msg = std::make_shared<StunMessageHeader>();
    create_request_identify(msg, who);
    if (networking_->ip_map_.get_wrapper_for_pk(where) != networking_->ip_map_.get_map_end()) {
        send_stun_message(msg, where);
    }
    else {
        std::cout << "Failed to connect to peer "
                  << where
                  << " when attempting to establish STUN connection."
                  << std::endl;
    }
}

void StunClient::identify(QHostAddress& address) {
    auto msg = std::make_shared<StunMessageHeader>();
    create_request_identify_address(msg, address);
    send_stun_message_transport_address(msg, address, STUN_PORT);
}

pk_t StunClient::get_stun_server_any() {
    return stun_servers.front();
}

pk_t StunClient::get_stun_server_front() {
    return stun_servers.front();
}

pk_t StunClient::get_stun_server_next() {
    pk_t first_stun_server = stun_servers.front();
    stun_servers.pop();
    stun_servers.push(first_stun_server);
    return first_stun_server;
}

void StunClient::stun_server_connection_error() {
    std::cout << "STUN connection to " << stun_server_awaiting_confirmation << " failed." << std::endl;
}

void StunClient::add_stun_server(QTcpSocket* tcp_socket_, pk_t pid) {
    networking_->ip_map_.update_stun_ip(pid, tcp_socket_->peerAddress(), tcp_socket_->peerPort());
    tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    networking_->ip_map_.set_tcp_socket(pid, tcp_socket_);
    stun_servers.push(pid);
}

pk_t StunClient::process_indication_send(stun_header_ptr stun_message, std::string& np2ps_message) {
    PublicIdentifierAttribute* pia;
    RelayedPublicIdentifierAttribute* ria;
    DataAttribute* data;

    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*) attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) { //server
            ria = (RelayedPublicIdentifierAttribute*) attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::data) {
            data = (DataAttribute*) attr.get();
        }
    }

    np2ps_message = data->get_np2ps_messsage();

    networking_->ip_map_.update_preferred_stun_server(
        pia->get_public_identifier(),
        ria->get_public_identifier()
    );
    networking_->ip_map_.get_wrapper_for_pk(
        pia->get_public_identifier()
    )->second.set_relay_state(true); // relaying

    return ria->get_public_identifier();
}

void StunClient::process_response_success_send(stun_header_ptr stun_message) {
    PublicIdentifierAttribute* pia;
    RelayedPublicIdentifierAttribute* ria;

    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*) attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) { //server
            ria = (RelayedPublicIdentifierAttribute*) attr.get();
        }
    }

    networking_->ip_map().check_or_add_to_ip_map_relayed(
        pia->get_public_identifier(),
        ria->get_public_identifier()
    );

    networking_->stun_message_success(pia->get_public_identifier());
}

void StunClient::process_response_error_send(stun_header_ptr stun_message) {
    PublicIdentifierAttribute* pia;
    RelayedPublicIdentifierAttribute* ria;
    ErrorCodeAttribute* err;

    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            pia = (PublicIdentifierAttribute*) attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) { //server
            ria = (RelayedPublicIdentifierAttribute*) attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::error_code) {
            err = (ErrorCodeAttribute*) attr.get();
        }
    }

    networking_->ip_map().check_or_add_to_ip_map_relayed(
        pia->get_public_identifier(),
        ria->get_public_identifier()
    );

    switch (err->get_code()) {
        case STUN_ERR_USE_OTHER_SERVER:
            networking_->ip_map().check_and_remove_relayed(
                pia->get_public_identifier(),
                ria->get_public_identifier()
            );
            networking_->resend_stun_message(pia->get_public_identifier());
            break;
        case STUN_ERR_PEER_OFFLINE:
            break;
        default:
            break;
    }
}

void StunClient::delete_disconnected_users() {
    
}

void StunClient::process_response_error_allocate(stun_header_ptr stun_message) {
    RelayedPublicIdentifierAttribute* ria;

    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) { //server
            ria = (RelayedPublicIdentifierAttribute*) attr.get();
        }
    }

    std::cout << "Identifier was already allocated for " << ria->get_public_identifier() << std::endl;
}

void StunClient::add_stun_server(pk_t pid) {
    stun_servers.push(pid);
}

void StunClient::clean_bad_stun_servers() {
    for (int i = 0; i < stun_servers.size(); i++) {
        pk_t audited_server = stun_servers.front();
        stun_servers.pop();
        if (failed_connections_per_server.count(audited_server) > 0) {
            if (failed_connections_per_server[audited_server] <= 5) {
                stun_servers.push(audited_server);
            }
            else {
                failed_connections_per_server.erase(audited_server);
            }
        }
    }
}
