#ifndef PROGRAM_STUN_SERVER_HPP
#define PROGRAM_STUN_SERVER_HPP

#include <iostream>
#include <array>
#include <vector>
#include <set>

#include "GlobalUsing.h"
#include "XorMappedAddressAttribute.hpp"
#include "ErrorCodeAttribute.hpp"
#include "UnknownAttributesAttribute.hpp"
#include "TurnAllocation.hpp"
#include "RequestedTransportAttribute.hpp"
#include "LifetimeAttribute.hpp"
#include "XorRelayedAddress.hpp"
#include "IpMap.h"
#include "PublicIdentifierAttribute.hpp"
#include "StunTags.hpp"
#include "TurnAllocation.hpp"
#include "Networking.h"

#include <QObject>
#include <QApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtCore>

class Networking;

/**
 * @brief Main class for STUN server.
 * 
 * Manages new connections from STUN clients, processes requests and indications,
 * creates responses and indications.
 * 
 */
class StunServer : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructor. Server is NOT initialized here.
     * 
     * @param networking_ Pointer to Networking that initializes this server.
     */
    StunServer(Networking* networking_);

public slots:
    /**
     * @brief Starts the STUN server with provided address.
     * 
     * Once started, it can't be started again. Port is forced to value of 3478
     * 
     * @param address Address to listen on.
     */
    void start_server(QHostAddress address);

private slots:
    /**
     * @brief Process received request or indication and create response or
     * indication.
     * 
     * Responses are sent using same socket they came.
     * 
     */
    void reply();

    /**
     * @brief Handler for new connections.
     * 
     */
    void new_connection();

    /**
     * @brief Prints error message.
     * 
     */
    void display_error();

    /**
     * @brief Sends the error message.
     * 
     * Sets the error code and reason. Only used for Binding method.
     * 
     */
    void send_error(std::size_t, QTcpSocket*, QHostAddress, quint16);

private:
    /**
     * @brief Initializes the server.
     * 
     * Attribute factories are initialized here.
     * 
     */
    void init_server();

    /**
     * @brief Process Identify request.
     * 
     * @param message_orig 
     * @param message_new 
     */
    void process_request_identify(stun_header_ptr message_orig, stun_header_ptr message_new);

    /**
     * @brief Process Allocate request.
     * 
     * @param message_orig 
     * @param message_new 
     */
    void process_request_allocate(stun_header_ptr message_orig, stun_header_ptr message_new, QTcpSocket* socket);

    /**
     * @brief Process Send request.
     * 
     * Creates Send indication and sends the message to receiver.
     * 
     * @param to Receiver of relayed message.
     * @param message_orig Original message to containing data to relay.
     * @param message_new New message to be sent.
     */
    void process_request_send(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t& to);

    /**
     * @brief Create a Allocate success response.
     * 
     * @param message_orig Allocate request.
     * @param message_new Allocate success response.
     * @param lifetime Lifetime of allocation.
     * @param socket Socket that received Allocate request.
     */
    void create_response_success_allocate(stun_header_ptr message_orig, stun_header_ptr message_new, std::uint32_t lifetime, QTcpSocket* socket);

    /**
     * @brief Create Identify success response.
     * 
     * @param message_orig Identify request.
     * @param message_new Identify success response.
     * @param public_id Public identifier of identification target.
     * @param client_ipv4 Server IPv4.
     * @param port Server port.
     * @param key Public key of identification target.
     */
    void create_response_success_identify(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t public_id, QHostAddress client_ipv4, std::uint16_t port, CryptoPP::RSA::PublicKey& key);

    /**
     * @brief Create Identify error response.
     * 
     * @param message_orig Identify request.
     * @param message_new Identify error response.
     * @param public_id Public ID that failed to identify.
     */
    void create_response_error_identify(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t public_id);

    /**
     * @brief Create a Allocate error response.
     * 
     * @param message_orig Allocate request.
     * @param message_new Allocate error response.
     */
    void create_response_error_allocate(stun_header_ptr message_orig, stun_header_ptr message_new);

    /**
     * @brief Create Send indication
     * 
     * @param message_orig Send request
     * @param message_new Send indication
     * @param source_pk Sender of request.
     * @param np2ps_message Message to relay.
     */
    void create_indication_send(stun_header_ptr message_orig, stun_header_ptr message_new, pk_t source_pk, std::string&& np2ps_message);    
    
    /**
     * @brief Sends the STUN message.
     * 
     * Uses provided socket to send the message. 
     * 
     * @param socket Socket to use to send.
     * @param stun_message STUN message to send.s
     */
    void send_stun_message(QTcpSocket* socket , stun_header_ptr stun_message);
    
    QTcpServer* tcp_server_;
    QTcpSocket* tcp_socket_;
    QDataStream in_stream;
    stun_attr_type_vec unknown_cr_attributes;
    std::set<quint16> known_cr_attributes;
    factory_map stun_attribute_factories;
    //std::map<pk_t, TurnAllocation> allocations;
    Networking* networking_;
    bool server_started = false;
};

#endif
