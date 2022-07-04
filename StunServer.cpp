#include "StunServer.hpp"

StunServer::StunServer() {
    init_server(QHostAddress(QString("127.0.0.1")), STUN_PORT);

    connect(tcp_server_.get(), &QTcpServer::newConnection, 
            this, &StunServer::new_connection);
}

StunServer::StunServer(QHostAddress address, std::uint16_t port) {
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
                auto mp = MPProcess<CRequestTag, MBindingTag>(stun_message, stun_new, tcp_socket_, unknown_cr_attributes);
                MessageProcessor<CRequestTag, MBindingTag>::process(mp);
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

        send_stun_message(tcp_socket_, stun_new);
    }
    catch (invalid_stun_message_format_error de) {
        in_stream.abortTransaction();
        std::cout << de.what() << std::endl;
    }
    catch (unknown_comprehension_required_attribute_error u) {
        send_error(420, tcp_socket_, tcp_socket_->peerAddress(), tcp_socket_->peerPort());
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