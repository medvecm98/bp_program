#include "StunClient.hpp"

StunClient::StunClient() {
    tcp_socket_ = std::make_shared<QTcpSocket>();

    in.setDevice(tcp_socket_.get());
    in.setVersion(QDataStream::Qt_5_0);

    connect(tcp_socket_.get(), &QIODevice::readyRead, this, &StunClient::receive_msg);
    connect(tcp_socket_.get(), &QAbstractSocket::errorOccurred, this, &StunClient::error);

    /* ATTRIBUTE FACTORIES ARE INITIALIZED HERE */
    stun_attribute_factories.emplace(STUN_ATTR_XOR_MAPPED_ADDRESS, std::make_shared<XorMappedAddressAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_ERROR_CODE, std::make_shared<ErrorCodeAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_UNKNOWN_ATTRIBUTES, std::make_shared<UnknownAttributesAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_DATA, std::make_shared<DataAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_LIFETIME, std::make_shared<LifetimeAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_PUBLIC_IDENTIFIER, std::make_shared<PublicIdentifierAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_XOR_RELAYED_ADDRESS, std::make_shared<XorRelayedAddressAttributeFactory>());
    stun_attribute_factories.emplace(STUN_ATTR_REQUESTED_TRANSPORT, std::make_shared<RequestedTransportAttributeFactory>());

    stun_server.first = QHostAddress(QString("127.0.0.1"));
    stun_server.second = STUN_PORT;
}

StunClient::StunClient(std::shared_ptr<Networking> networking, QHostAddress address, std::uint16_t port_stun) : StunClient::StunClient() {
    stun_server.first = address;
    stun_server.second = port_stun;
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
    std::cout << "STUN client: sending STUN message" << std::endl;
    auto& [addr, port] = stun_server;

    tcp_socket_->abort();
    tcp_socket_->connectToHost(addr, port);

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
    }
    else if (stun_message_header->stun_class == StunClassEnum::response_error) {
        if (stun_message_header->stun_method == StunMethodEnum::binding) {
            MPProcess<CResponseErrorTag, MBindingTag> mppe(stun_message_header);
            MessageProcessor<CResponseErrorTag, MBindingTag>::process(mppe);
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
 * 
 * 
 */
void StunClient::receive_msg() {
    std::cout << "SC: message received" << std::endl;
    QDataStream in_stream;
    in_stream.setVersion(QDataStream::Qt_5_0);
    in_stream.setDevice(tcp_socket_.get());
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
    send_stun_message(msg);

    //receive_msg();
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