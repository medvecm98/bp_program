#ifndef PROGRAM_STUN_SERVER_HPP
#define PROGRAM_STUN_SERVER_HPP

#include <iostream>
#include <array>
#include <vector>
#include <set>

#include "GlobalUsing.h"
#include "MessageProcessor.hpp"
#include "TurnAllocation.hpp"
#include "Networking.h"

#include <QObject>
#include <QApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtCore>

class Networking;


class StunServer : public QObject {
    Q_OBJECT
public:
    StunServer(Networking* networking_);
    StunServer(Networking* networking_, QHostAddress address, std::uint16_t port = 3478);

private slots:
    void reply();
    void new_connection();
    void display_error();
    void send_error(std::size_t, QTcpSocket*, QHostAddress, quint16);
    void handle_binding_request() {}

private:
    void init_server(QHostAddress address, std::uint16_t port = 3478);
    bool check_attribute(quint16 attr);
    bool check_validity_all_attributes(stun_header_ptr stun_message, stun_attr_type_vec& output);
    void process_request_identify(stun_header_ptr message_orig, stun_header_ptr message_new);
    void process_request_allocate(stun_header_ptr message_orig, stun_header_ptr message_new, QTcpSocket* socket);
    void create_response_success_allocate(stun_header_ptr message_orig, stun_header_ptr message_new, std::uint32_t lifetime, QTcpSocket* socket);
    void create_response_success_identify(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t public_id, QHostAddress client_ipv4, std::uint16_t port, CryptoPP::RSA::PublicKey& key);
    void send_stun_message(QTcpSocket* socket , stun_header_ptr stun_message);
    void create_response_error_identify(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t public_id);
    void process_request_send(stun_header_ptr message_orig, pk_t& to);
    std::shared_ptr<QTcpServer> tcp_server_;
    QTcpSocket* tcp_socket_;
    QDataStream in_stream;
    stun_attr_type_vec unknown_cr_attributes;
    std::set<quint16> known_cr_attributes;
    factory_map stun_attribute_factories;
    //std::map<pk_t, TurnAllocation> allocations;
    Networking* networking_;
};

#endif