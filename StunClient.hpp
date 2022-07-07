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
    StunClient(Networking*  networking);
    StunClient(Networking*  networking, QHostAddress address, std::uint16_t port_stun = 3478);
    void binding_request();
    void allocate_request(pk_t where);
    void identify(pk_t who);
    void send_stun_message(stun_header_ptr stun_message, pk_t public_id);
    void send_stun_message_transport_address(stun_header_ptr stun_message, QHostAddress address, std::uint16_t port);
    void add_stun_server(QHostAddress address, std::uint16_t port, pk_t pid);
    void stun_server_connected();
    void stun_server_connection_error();
    void process_response_success_allocate(QTcpSocket* tcp_socket, stun_header_ptr message_orig);

private slots:
    void accept();
    void error(QAbstractSocket::SocketError socketError);
    void receive_msg();

private:
    void check_for_nat();

    void create_binding_request(stun_header_ptr stun_message);
    void init_client(QHostAddress address, std::uint16_t port = 3478);
    void handle_received_message(stun_header_ptr stun_message_header);

    void create_request_identify(stun_header_ptr stun_message, pk_t who);
    void create_request_allocate(stun_header_ptr stun_message, std::uint32_t lifetime, pk_t public_id);

    void process_response_success_identify(stun_header_ptr stun_message);
    void process_response_error_identify(stun_header_ptr stun_message);
    void process_response_success_binding(stun_header_ptr stun_message, QTcpSocket* socket_);

    pk_t get_stun_server_any();

    QTcpSocket* tcp_socket_;
    QDataStream in;
    CryptoPP::AutoSeededRandomPool rng;
    stun_server_pair stun_server;
    std::vector<pk_t> stun_servers;
    factory_map stun_attribute_factories;
    Networking* networking_;

    pk_t stun_server_awaiting_confirmation;

    bool nat_active_flag = false;

    
};
using stun_client_ptr = std::shared_ptr<StunClient>;

#endif