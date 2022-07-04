#ifndef PROGRAM_STUN_CLIENT_HPP
#define PROGRAM_STUN_CLIENT_HPP

#include "GlobalUsing.h"
#include "MessageProcessor.hpp"
#include "Networking.h"

#include <bitset>

#include <QObject>
#include <QApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtCore>

#include <cryptopp/osrng.h>

//IP and port
using stun_server_pair = std::pair<QHostAddress, std::uint16_t>;

class Networking;

class StunClient : public QObject {
    Q_OBJECT
public:
    StunClient();
    StunClient(std::shared_ptr<Networking> networking, QHostAddress address, std::uint16_t port_stun = 3478);
    void binding_request();
    void send_stun_message(stun_header_ptr stun_message);

private slots:
    void accept();
    void error(QAbstractSocket::SocketError socketError);
    void receive_msg();

private:
    void create_binding_request(stun_header_ptr stun_message);
    void init_client(QHostAddress address, std::uint16_t port = 3478);
    void handle_received_message(stun_header_ptr stun_message_header);

    std::shared_ptr<QTcpSocket> tcp_socket_;
    QDataStream in;
    CryptoPP::AutoSeededRandomPool rng;
    stun_server_pair stun_server;
    factory_map stun_attribute_factories;
    std::shared_ptr<Networking> networking_;
};
using stun_client_ptr = std::shared_ptr<StunClient>;

#endif