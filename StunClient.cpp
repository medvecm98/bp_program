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

void StunClient::reset_connection_timer(QTcpSocket* socket) {
    if (!stun_connection_timer) {
        stun_connection_timer = new QTimer(this);
    }
    else {
        stun_connection_timer->disconnect();
    }

    QObject::connect(
        stun_connection_timer, &QTimer::timeout,
        socket, &QTcpSocket::disconnectFromHost
    );
    QObject::connect(
        stun_connection_timer, &QTimer::timeout,
        this, &StunClient::connection_timeout
    );
}

pk_t StunClient::extract_final_destination(stun_header_ptr stun_message ) {
    if (stun_message->stun_method == StunMethodEnum::send && stun_message->stun_class == StunClassEnum::request) {
        RelayedPublicIdentifierAttribute* final_dest_id = NULL;
        for (auto&& attr : stun_message->attributes) {
            if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) {
                final_dest_id = (RelayedPublicIdentifierAttribute*) attr.get();
            }
        }
        if (!final_dest_id) {
            throw other_error("Something went wrong with send request.");
        }
        return final_dest_id->get_public_identifier();
    }
    return 0;
}

void StunClient::send_stun_message(stun_header_ptr stun_message, pk_t public_id) {
    pk_t final_destination = extract_final_destination(stun_message);
    if (final_destination != 0) {
        IpWrapper& final_destination_wrapper = networking_->ip_map().get_wrapper_ref(final_destination);
        final_destination_wrapper.begin_relay_stun_server_tracking();
    }
    IpWrapper& wrapper = networking_->ip_map_.get_wrapper_ref(public_id);
    if (networking_->stun_sending_in_progress) {
        networking_->stun_messages_waiting_for_connection.push_back(
            std::make_tuple(wrapper.ipv4, wrapper.stun_port, stun_message, public_id, false)
        );
    }
    else {
        QTcpSocket* socket;

        if (!networking_->ip_map_.have_ip4(public_id)) {
            throw std::logic_error("Missing public_id of provided STUN server.");
        }

        IpWrapper& wrapper = networking_->ip_map().get_wrapper_ref(public_id);
        auto addr = wrapper.ipv4;
        auto port = wrapper.stun_port;
        
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

            reset_connection_timer(socket);

            QObject::connect(socket, &QIODevice::readyRead, this, &StunClient::receive_msg);
            QObject::connect(socket, &QAbstractSocket::errorOccurred, this, &StunClient::error);
            QObject::connect(socket, &QAbstractSocket::disconnected, socket, &QObject::deleteLater);
            QObject::connect(socket, &QAbstractSocket::disconnected, networking_, &Networking::peer_process_disconnected_users);
            QObject::connect(socket, &QAbstractSocket::connected, this, &StunClient::host_connected);

            networking_->stun_messages_waiting_for_connection.push_front(
                std::make_tuple(wrapper.ipv4, wrapper.stun_port, stun_message, public_id, true)
            );

            std::cout << "Connecting to: " << addr.toString().toStdString() << " port: " << port << " id " << public_id << std::endl;

            socket->connectToHost(addr, port);
            stun_connection_timer->start(2000);
        }
    }
}

void StunClient::send_stun_message_transport_address(stun_header_ptr stun_message, QHostAddress address, std::uint16_t port) {
    if (networking_->stun_sending_in_progress) {
        networking_->stun_messages_waiting_for_connection.push_back(
            std::make_tuple(address, port, stun_message, 0, false)
        );
    }
    else {
        networking_->stun_sending_in_progress = true;
        QTcpSocket* socket = new QTcpSocket(this);
        reset_connection_timer(socket);
        connect(socket, &QAbstractSocket::errorOccurred, this, &StunClient::error);
        connect(socket, &QIODevice::readyRead, this, &StunClient::receive_msg);
        QObject::connect(socket, &QAbstractSocket::disconnected, socket, &QObject::deleteLater);
        QObject::connect(
            socket, &QAbstractSocket::disconnected, 
            networking_, &Networking::peer_process_disconnected_users
        );
        QObject::connect(socket, &QAbstractSocket::connected, this, &StunClient::host_connected);

        networking_->stun_messages_waiting_for_connection.push_front(
            std::make_tuple(address, port, stun_message, 0, false)
        );

        socket->connectToHost(address, port);
        stun_connection_timer->start(2000);
    }
}

void StunClient::host_connected() {
    stun_connection_timer->stop();
    QTcpSocket *socket = (QTcpSocket*) QObject::sender();

    if (networking_->stun_messages_waiting_for_connection.empty()) {
        return;
    }

    auto waiting_stun_message = networking_->stun_messages_waiting_for_connection.front();

    QHostAddress address_waiting_to_connect = std::get<0>(waiting_stun_message);
    uint16_t port_waiting_to_connect = std::get<1>(waiting_stun_message);
    stun_header_ptr header_waiting_to_connect = std::get<2>(waiting_stun_message);
    bool save_socket = std::get<4>(waiting_stun_message);
    pk_t connecting_to = std::get<3>(waiting_stun_message);

    networking_->stun_messages_waiting_for_connection.pop_front();

    if (save_socket) {
        if (header_waiting_to_connect->stun_class == StunClassEnum::request && 
            header_waiting_to_connect->stun_method == StunMethodEnum::allocate)
        {
            socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
            networking_->ip_map_.set_tcp_socket(connecting_to, socket);
        }
    }

    QByteArray block;
    QDataStream out_stream(&block, QIODevice::WriteOnly);
    out_stream.setVersion(QDataStream::Qt_5_0);
    header_waiting_to_connect->write_stun_message(out_stream);
    if (socket->write(block) == -1)
        std::cout << "SC: Error occured while writing the block" << std::endl;

    /* send next message in queue */

    networking_->stun_sending_in_progress = false;
    if (!networking_->stun_messages_waiting_for_connection.empty()) {
        waiting_stun_message = networking_->stun_messages_waiting_for_connection.front();

        address_waiting_to_connect = std::get<0>(waiting_stun_message);
        port_waiting_to_connect = std::get<1>(waiting_stun_message);
        header_waiting_to_connect = std::get<2>(waiting_stun_message);
        save_socket = std::get<4>(waiting_stun_message);
        connecting_to = std::get<3>(waiting_stun_message);

        networking_->stun_messages_waiting_for_connection.pop_front();

        if (connecting_to != 0) {
            send_stun_message(header_waiting_to_connect, connecting_to);
        }
        else {
            send_stun_message_transport_address(header_waiting_to_connect, address_waiting_to_connect, port_waiting_to_connect);
        }
    }
    // failed_connections = 0;
    // clean_failed_connection_for_server(get_stun_server_front());
}

void print_stun_message(stun_header_ptr stun_message_header) {
    std::cout << "STUN client " << std::flush;
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
    if (stun_message_header->stun_class == StunClassEnum::request) {
        if (stun_message_header->stun_method == StunMethodEnum::send) {
            networking_->stun_server->handle_received_message(stun_message_header, socket);
            return;
        }
    }
    else if (stun_message_header->stun_class == StunClassEnum::response_success) {
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
        else if (stun_message_header->stun_method == StunMethodEnum::send) {
            process_response_success_send(stun_message_header);
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
        else if (stun_message_header->stun_method == StunMethodEnum::send) {
            process_response_error_send(stun_message_header);
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
    
    do {
        stun_header_ptr stun_message_header = std::make_shared<StunMessageHeader>();
        try {
            stun_message_header->read_message_header(in_stream);
            stun_message_header->read_attributes(in_stream, stun_attribute_factories);
        }
        catch(invalid_stun_message_format_error& i) {
            std::cout << i.what() << std::endl;
            in_stream.abortTransaction();
        }
        handle_received_message(stun_message_header, socket);
    } while (socket->bytesAvailable() > 0);
    in_stream.commitTransaction();
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

shared_ptr_message decrypt_message_using_symmetric_key(
	std::string e_msg,
	CryptoPP::SecByteBlock iv,
	IpWrapper &ipw)
{
	symmetric_cipher::Decryption dec;

	// symmetric key present
	CryptoPP::SecByteBlock byte_block = CryptoUtils::instance().byte_queue_to_sec_byte_block(ipw.get_eax());

	std::cout << "Decrypting with symmetric key: " << CryptoUtils::instance().bq_to_hex(ipw.get_eax()) << std::endl;

	dec.SetKeyWithIV(byte_block, byte_block.size(), iv);
	std::string dec_msg; // decrypted message
	CryptoPP::StringSource s(
		e_msg,
		true,
		new StringDecoder(
			new CryptoPP::AuthenticatedDecryptionFilter(
				dec,
				new CryptoPP::StringSink(
					dec_msg))));

	// deserialize

	shared_ptr_message m = std::make_shared<proto_message>();
	m->ParseFromString(dec_msg);

	return std::move(m); // message now can be read in GPB format
}

void StunClient::check_message_side_effect(
    stun_header_ptr header_waiting_to_connect,
    IpWrapper& connectee_wrapper)
{
    DataAttribute* data;
    for (auto&& attr : header_waiting_to_connect->attributes) {
        if (attr->attribute_type == StunAttributeEnum::data) {
            data = (DataAttribute*)attr.get();
            break;
        }
    }
    std::string np2ps_message = data->get_np2ps_messsage();
    QByteArray msg(np2ps_message.c_str(), np2ps_message.length());
    QDataStream stream(msg);
    quint16 msg_version, msg_class;
    stream >> msg_version;
    stream >> msg_class;
    if (msg_class == ENCRYPTED_MESSAGE)
    {
        quint64 pid, iv_size, msg_size;
        stream >> pid;
        stream >> iv_size;
        QByteArray iv_array, msg_array;
        stream >> iv_array;
        stream >> msg_array;
        std::string message = msg_array.toStdString();
        std::string iv_str = iv_array.toStdString();
        CryptoPP::SecByteBlock iv(
            reinterpret_cast<const CryptoPP::byte *>(&iv_str[0]), iv_str.size()
        );
        shared_ptr_message np2ps_message = decrypt_message_using_symmetric_key(
            message, iv, connectee_wrapper
        );
        if (np2ps_message->msg_ctx() == np2ps::REQUEST &&
            np2ps_message->msg_type() == np2ps::ARTICLE_ALL) 
        {
            emit got_article_all_rejection(np2ps_message);
        }
    }
}

void StunClient::error(QAbstractSocket::SocketError socket_error) {
    stun_connection_timer->stop();
    QTcpSocket* socket = (QTcpSocket*)QObject::sender();
    std::cout << "Connection to STUN server failed: " << std::endl;
    std::cout << "Error number: " << socket->errorString().toStdString() << " " << socket_error << std::endl;

    if (networking_->stun_messages_waiting_for_connection.empty()) {
        /* This check is necesasry if remote peer disconnects -> no messages are waiting, but
         * this method is still called.
         */
        return;
    }
    std::cout << "a" << std::endl;
    auto waiting_stun_message = networking_->stun_messages_waiting_for_connection.front();

    QHostAddress address_waiting_to_connect = std::get<0>(waiting_stun_message);
    uint16_t port_waiting_to_connect = std::get<1>(waiting_stun_message);
    stun_header_ptr header_waiting_to_connect = std::get<2>(waiting_stun_message);
    pk_t connecting_to = std::get<3>(waiting_stun_message);
    bool save_socket = std::get<4>(waiting_stun_message);

    networking_->stun_messages_waiting_for_connection.pop_front();

    if (socket_error == QAbstractSocket::ConnectionRefusedError ||
        socket_error == QAbstractSocket::SocketTimeoutError ||
        socket_error == QAbstractSocket::NetworkError)
    {
        check_for_another_server(connecting_to, header_waiting_to_connect);
    }

    /* If remote host closed connection, message is just popped. */

    continue_message_sending(waiting_stun_message);
}

void StunClient::connection_timeout() {
    stun_connection_timer->stop();
    std::cout << "Connection to STUN server timed out." << std::endl;

    if (networking_->stun_messages_waiting_for_connection.empty()) {
        /* This check is necesasry if remote peer disconnects -> no messages are waiting, but
         * this method is still called.
         */
        return;
    }
    auto waiting_stun_message = networking_->stun_messages_waiting_for_connection.front();

    QHostAddress address_waiting_to_connect = std::get<0>(waiting_stun_message);
    uint16_t port_waiting_to_connect = std::get<1>(waiting_stun_message);
    stun_header_ptr header_waiting_to_connect = std::get<2>(waiting_stun_message);
    pk_t connecting_to = std::get<3>(waiting_stun_message);
    bool save_socket = std::get<4>(waiting_stun_message);

    networking_->stun_messages_waiting_for_connection.pop_front();

    check_for_another_server(connecting_to, header_waiting_to_connect);

    /* If remote host closed connection, message is just popped. */

    continue_message_sending(waiting_stun_message);
}

void StunClient::continue_message_sending(
    std::tuple<QHostAddress, uint16_t, stun_header_ptr, pk_t, bool> waiting_stun_message
)
{
    networking_->stun_sending_in_progress = false;
    if (!networking_->stun_messages_waiting_for_connection.empty()) {
        waiting_stun_message = networking_->stun_messages_waiting_for_connection.front();

        QHostAddress address_waiting_to_connect = std::get<0>(waiting_stun_message);
        uint16_t port_waiting_to_connect = std::get<1>(waiting_stun_message);
        stun_header_ptr header_waiting_to_connect = std::get<2>(waiting_stun_message);
        pk_t connecting_to = std::get<3>(waiting_stun_message);
        bool save_socket = std::get<4>(waiting_stun_message);

        networking_->stun_messages_waiting_for_connection.pop_front();

        if (connecting_to != 0) {
            send_stun_message(header_waiting_to_connect, connecting_to);
        }
        else {
            send_stun_message_transport_address(header_waiting_to_connect, address_waiting_to_connect, port_waiting_to_connect);
        }
    }
}

void StunClient::check_for_another_server(
    pk_t connecting_to,
    stun_header_ptr header_waiting_to_connect)
{
    if (connecting_to != 0) { // we need to know who are we connecting to
        std::cout << "b" << std::endl;
        IpWrapper& connectee_wrapper = networking_->ip_map().get_wrapper_ref(connecting_to);
        if (header_waiting_to_connect->stun_method == StunMethodEnum::send && 
            header_waiting_to_connect->stun_class == StunClassEnum::request)
        {
            pk_t final_dest_id = extract_final_destination(header_waiting_to_connect);
            IpWrapper& final_dest_wrapper = networking_->ip_map().get_wrapper_ref(
                final_dest_id
            );
            std::cout << "c" << std::endl;
            if (final_dest_wrapper.has_relay_stun_servers())
            {
                std::cout << "d" << std::endl;
                networking_->stun_sending_in_progress = false;
                try {
                    final_dest_wrapper.next_relay_stun_server();
                    std::cout << "More relay STUN servers left. " << final_dest_wrapper.get_relay_stun_server() << std::endl;
                    send_stun_message(header_waiting_to_connect, final_dest_wrapper.get_relay_stun_server());
                    return;
                }
                catch (no_more_relay_stun_servers e) {
                    std::cout << "No more relay STUN servers left." << std::endl;
                    final_dest_wrapper.end_relay_stun_server_tracking();
                    peers_waiting_for_relays.emplace(
                        final_dest_id,
                        header_waiting_to_connect
                    );
                    identify(final_dest_id);
                }
            }
            else {
                std::cout << "e" << std::endl;
                peers_waiting_for_relays.emplace(
                    final_dest_id,
                    header_waiting_to_connect
                );
                identify(final_dest_id);
            }
        }
        else if (header_waiting_to_connect->stun_method == StunMethodEnum::allocate && 
            header_waiting_to_connect->stun_class == StunClassEnum::request)
        {
            blocked_stun_servers.emplace(connecting_to);
        }
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
    PublicIdentifierAttribute* identified_peer;
    XorRelayedAddressAttribute* xraa;
    XorMappedAddressAttribute* stun_address_port;
    RelayedPublicIdentifierAttribute* relayer_or_identifier;
    PublicKeyAttribute* pka;
    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            identified_peer = (PublicIdentifierAttribute*)attr.get();
        }
        else if (attr->attribute_type == StunAttributeEnum::xor_relayed_address) {
            xraa = (XorRelayedAddressAttribute*)attr.get();
        }
        else if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) {
            relayer_or_identifier = (RelayedPublicIdentifierAttribute*)attr.get();
        }
        else if (attr->attribute_type == StunAttributeEnum::public_key) {
            pka = (PublicKeyAttribute*)attr.get();
        }
        else if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
            stun_address_port = (XorMappedAddressAttribute*)attr.get();
        }
    }
    networking_->ip_map_.update_ip(identified_peer->get_public_identifier(), QHostAddress(xraa->get_address()), xraa->get_port(), stun_address_port->get_port());
    networking_->ip_map_.get_wrapper_for_pk(identified_peer->get_public_identifier())->second.preferred_stun_server = relayer_or_identifier->get_public_identifier();
    networking_->ip_map_.update_rsa_public(identified_peer->get_public_identifier(), pka->get_value());
    if (identified_peer->get_public_identifier() != relayer_or_identifier->get_public_identifier()) {
        networking_->ip_map_.check_or_add_to_ip_map_relayed(
            identified_peer->get_public_identifier(),
            relayer_or_identifier->get_public_identifier(),
            RelayState::Unknown
        );
    }

    std::cout << "STUN PEER " << identified_peer->get_public_identifier() << " identified via " << relayer_or_identifier->get_public_identifier() << std::endl;

    auto find_it_waiting_ip = networking_->waiting_ip.find(identified_peer->get_public_identifier());

    if (find_it_waiting_ip != networking_->waiting_ip.end()) { //found peer that needed to be identified
        auto msg = find_it_waiting_ip->second;
        networking_->waiting_ip.erase(find_it_waiting_ip);

        networking_->send_message_with_credentials(msg, true); //...send him the message
    }

    auto find_it_waiting_sym_key = networking_->waiting_symmetric_key_messages.find(identified_peer->get_public_identifier());

    if (find_it_waiting_sym_key != networking_->waiting_symmetric_key_messages.end()) {
        networking_->ip_map_.update_ip(identified_peer->get_public_identifier(), QHostAddress(xraa->get_address()), xraa->get_port(), stun_address_port->get_port());
        networking_->ip_map_.update_rsa_public(identified_peer->get_public_identifier(), pka->get_value());
        auto msg = find_it_waiting_sym_key->second;
        networking_->waiting_symmetric_key_messages.erase(find_it_waiting_sym_key);
        networking_->new_message_received(msg);
    }

    auto [waiting_message_for_relay_it, waiting_message_for_relay_eit] = peers_waiting_for_relays.equal_range(identified_peer->get_public_identifier());
    while (waiting_message_for_relay_it != waiting_message_for_relay_eit) {
        send_stun_message(waiting_message_for_relay_it->second, relayer_or_identifier->get_public_identifier());
        waiting_message_for_relay_it++;
        return;
    }
    peers_waiting_for_relays.erase(identified_peer->get_public_identifier());

    auto find_it_newspapers_awaiting_identification = networking_->newspapers_awaiting_identification.find(xraa->get_address());
    if (find_it_newspapers_awaiting_identification != networking_->newspapers_awaiting_identification.end()) {
        auto news_tuple = find_it_newspapers_awaiting_identification->second;
        std::string name = std::get<0>(news_tuple);
        port_t np2ps_port = std::get<1>(news_tuple);
        port_t stun_port = std::get<2>(news_tuple);
        auto ip = QHostAddress(find_it_newspapers_awaiting_identification->first).toString().toStdString();
        networking_->newspapers_awaiting_identification.erase(find_it_newspapers_awaiting_identification);
        emit networking_->newspaper_identified(relayer_or_identifier->get_public_identifier(), name, ip, np2ps_port, stun_port);
        return;
    }


    emit confirmed_newspaper(identified_peer->get_public_identifier());
}

void StunClient::process_response_error_identify(stun_header_ptr stun_message) {
    PublicIdentifierAttribute* peer_to_identify;
    RelayedPublicIdentifierAttribute* relayer;
    for (auto&& attr : stun_message->attributes) {
        if (attr->attribute_type == StunAttributeEnum::public_identifier) {
            peer_to_identify = (PublicIdentifierAttribute*)attr.get();
        }
        if (attr->attribute_type == StunAttributeEnum::relayed_publid_identifier) {
            relayer = (RelayedPublicIdentifierAttribute*)attr.get();
        }
    }
    std::cout << "STUN PEER " << peer_to_identify->get_public_identifier() << " FAILED to identify via " << relayer->get_public_identifier() << std::endl;
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

    auto np2ps_address_port = std::make_shared<XorMappedAddressAttribute>();
    np2ps_address_port->initialize(
        stun_message.get(), STUN_IPV4, networking_->ip_map_.my_ip().ipv4, networking_->ip_map().my_ip().port
    );
    stun_message->append_attribute(np2ps_address_port);

    auto stun_address_port = std::make_shared<XorRelayedAddressAttribute>();
    stun_address_port->initialize(
        stun_message.get(), STUN_IPV4, networking_->ip_map_.my_ip().ipv4, networking_->ip_map().my_ip().stun_port
    );
    stun_message->append_attribute(stun_address_port);

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
    IpWrapper& my_wrapper = networking_->ip_map().my_ip();
    if (my_wrapper.ipv4 == QHostAddress::AnyIPv4 || GlobalMethods::ip_address_is_private(my_wrapper.ipv4) || !GlobalMethods::ip_address_is_private(QHostAddress(xma->get_address()))) {
        networking_->ip_map_.my_ip().ipv4 = QHostAddress(xma->get_address());
    }

    std::cout << "Response success allocate from: " << pia->get_public_identifier() << std::endl;

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

void StunClient::identify(pk_t who){
    auto msg = std::make_shared<StunMessageHeader>();
    create_request_identify(msg, who);
    for (pk_t stun_server : stun_servers) {
        send_stun_message(msg, stun_server);
    }
}

void StunClient::identify_and_wait(pk_t who, stun_header_ptr waiting_message) {
    peers_waiting_for_relays.emplace(who, waiting_message);
    identify(who);
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

void StunClient::identify(QHostAddress& address, std::uint16_t stun_port) {
    auto msg = std::make_shared<StunMessageHeader>();
    create_request_identify_address(msg, address);
    send_stun_message_transport_address(msg, address, stun_port);
}

void StunClient::stun_server_connection_error() {
    std::cout << "STUN connection to " << stun_server_awaiting_confirmation << " failed." << std::endl;
}

void StunClient::add_stun_server(QTcpSocket* tcp_socket_, pk_t pid) {
    networking_->ip_map_.update_stun_ip(pid, tcp_socket_->peerAddress(), tcp_socket_->peerPort());
    tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    networking_->ip_map_.set_tcp_socket(pid, tcp_socket_);
    stun_servers.emplace(pid);
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

    std::cout << "STUN Received send indication from: " << ria->get_public_identifier() << std::endl;

    np2ps_message = data->get_np2ps_messsage();

    networking_->ip_map().check_or_add_to_ip_map_relayed(
        pia->get_public_identifier(),
        ria->get_public_identifier(),
        RelayState::Relayed
    );

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

    std::cout << "process_response_success_send " << pia->get_public_identifier() << " " << ria->get_public_identifier() << std::endl;

    networking_->ip_map().check_or_add_to_ip_map_relayed(
        pia->get_public_identifier(),
        ria->get_public_identifier(),
        RelayState::Relayed
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
        ria->get_public_identifier(),
        RelayState::Relayed
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
    stun_servers.emplace(pid);
}

void StunClient::check_stun_server(pk_t pid) {
    try {
        IpWrapper& wrapper = networking_->ip_map().get_wrapper_ref(pid);
        if (wrapper.relay_state == RelayState::Direct &&
            stun_servers.count(pid) == 0 &&
            blocked_stun_servers.count(pid) == 0)
        {
            allocate_request(pid);
        }
    }
    catch (user_not_found_in_database e) {}
}
