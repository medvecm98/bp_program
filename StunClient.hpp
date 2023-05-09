#ifndef PROGRAM_STUN_CLIENT_HPP
#define PROGRAM_STUN_CLIENT_HPP

#include "GlobalUsing.h"
#include "Networking.h"
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

/**
 * @brief Main class for managing STUN client.
 * 
 * Connects to STUN server and creates STUN requests.
 * 
 */
class StunClient : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Constructs StunClient.
     * 
     * Initializes all attribute factories.
     * 
     * @param networking Pointer to networking that is constructing this client.
     */
    StunClient(Networking*  networking);

    /**
     * @brief Creates allocate request for given receiver.
     * 
     * @param where Where to send allocate request.
     */
    void allocate_request(pk_t where);

    /**
     * @brief Identify given peer.
     * 
     * @param who Whom to identify.
     */
    void identify(pk_t who);

    /**
     * @brief Identify given peer.
     * 
     * @param who Whom to identify.
     * @param where Where to ask.
     */
    void identify(pk_t who, pk_t where);

    /**
     * @brief Identify given peer with provided IP address.
     * 
     * Only to be used when contacting new newspaper.
     * 
     * @param address Address to use when sending.
     */
    void identify(QHostAddress& address);

    /**
     * @brief Sends provided STUN message to provided STUN client.
     * 
     * Either new connection is established, or existing connection is used, if 
     * it exists.
     * 
     * Selects next working STUN server.
     * 
     * @param stun_message Message to send.
     */
    void send_stun_message(stun_header_ptr stun_message);

    /**
     * @brief Sends provided STUN message to provided STUN client.
     * 
     * Either new connection is established, or existing connection is used, if 
     * it exists.
     * 
     * @param stun_message Message to send.
     * @param public_id Receiver of the STUN message.
     */
    void send_stun_message(stun_header_ptr stun_message, pk_t public_id);

    /**
     * @brief Sends provided STUN message to provided STUN client, via given transport address.
     * 
     * Tries to connect to host. Success of failure are managed by appropriate slots.
     * 
     * @param stun_message Message to send.
     * @param address Address to use.
     * @param port Port to use.
     */
    void send_stun_message_transport_address(stun_header_ptr stun_message, QHostAddress address, std::uint16_t port);

    /**
     * @brief Adds STUN server to IpMap.    
     * 
     * @param tcp_socket_ TCP socket of the server.
     * @param pid Public identifier of the server.
     */
    void add_stun_server(QTcpSocket* tcp_socket_, pk_t pid);

    /**
     * @brief Adds STUN server to STUN client database.
     * 
     * @param pid Public identifier of the server.
     */
    void add_stun_server(pk_t pid);

    /**
     * @brief Failed connection to STUN server handler.
     * 
     */
    void stun_server_connection_error();

    /**
     * @brief Process succesful allocate response.
     * 
     * Sets the IP and port reflected by the server.
     * Adds STUN server to the IpMap.
     * Add new newspaper to the list.
     * 
     * @param tcp_socket Socket of the server connection.
     * @param message_orig STUN message to process.
     */
    void process_response_success_allocate(QTcpSocket* tcp_socket, stun_header_ptr message_orig);
    
    /**
     * @brief Creates Send request STUN message.
     * 
     * @param stun_message Created message
     * @param msg Originial NP2PS message to encapsulate.
     * @param where Receiver of the relayed message.
     */
    void create_request_send(stun_header_ptr stun_message, QByteArray& msg, pk_t where);

signals:
    /**
     * @brief Newspapers were confirmed through response success allocate.
     * 
     * @param pid Public identifier of the newspaper.
     */
    void confirmed_newspaper(pk_t pid);

    void confirmed_newspaper_pk(pk_t pid, rsa_public_optional pk);

private slots:
    /**
     * @brief Error handler.
     */
    void error(QAbstractSocket::SocketError socketError);

    /**
     * @brief Receives new message when `readyRead` was emitted.
     * 
     */
    void receive_msg();

    void delete_disconnected_users();

private:

    /**
     * @brief Creates Binding request object.
     * 
     * @param stun_message 
     */
    void create_binding_request(stun_header_ptr stun_message);

    /**
     * @brief Initializes STUN client.
     * 
     * @param stun_message 
     */
    void init_client(QHostAddress address, std::uint16_t port = 3478);

    /**
     * @brief Handles received message.
     * 
     * Based on message type and class, message is processed and changes are
     * made to networking, peer and other objects, if neccesary.
     * 
     * @param stun_message STUN message to handle
     */
    void handle_received_message(stun_header_ptr stun_message_header, QTcpSocket* socket);

    /**
     * @brief Creates Identify request.
     * 
     * @param stun_message Ouput of the message.
     */
    void create_request_identify(stun_header_ptr& stun_message, pk_t who);

    /**
     * @brief Creates Identify request, without the public identifier of
     * the receiver.
     * 
     * @param stun_message Output of the message.
     */
    void create_request_identify_empty(stun_header_ptr& stun_message);

    /**
     * @brief Creates Identify request, but send it to specific transport
     * address.
     * 
     * Used when communicating with newspaper whose public ID we don't yet know.
     * 
     * @param stun_message Output of the message.
     */
    void create_request_identify_address(stun_header_ptr& stun_message, QHostAddress address);

    /**
     * @brief Creates Allocate request.
     * 
     * @param stun_message Output of the message.
     */
    void create_request_allocate(stun_header_ptr stun_message, std::uint32_t lifetime, pk_t public_id);

    /**
     * @brief Process the Identify success response.
     * 
     * Updates IpMap with received information.
     * 
     * @param stun_message Message to process.
     */
    void process_response_success_identify(stun_header_ptr stun_message);

    /**
     * @brief Process the Identify error response.
     * 
     * Prints the error.
     * 
     * @param stun_message Message to process.
     */
    void process_response_error_identify(stun_header_ptr stun_message);

    /**
     * @brief Process the Binding success response
     */
    void process_response_success_binding(stun_header_ptr stun_message, QTcpSocket* socket_);

    /**
     * @brief Process the Send indication.
     * 
     * Relays the message to PeerReceiver.
     * 
     * @param stun_message STUN message to process.
     * @param np2ps_message Relayed NP2PS message.
     */
    void process_indication_send(stun_header_ptr stun_message, std::string& np2ps_message);

    /**
     * @brief Process the Allocate response error.
     * 
     * Prints the error.
     * 
     * @param stun_message Message to process.
     */
    void process_response_error_allocate(stun_header_ptr stun_message);

    /**
     * @brief Host was successfuly connected.
     * 
     * Message sending may commence.
     * 
     */
    void host_connected();

    pk_t get_stun_server_any();
    pk_t get_stun_server_front();
    pk_t get_stun_server_next();

    void add_failed_connection_for_server(pk_t pid);
    void clean_failed_connection_for_server(pk_t pid);
    void clean_bad_stun_servers();

    QDataStream in;
    CryptoPP::AutoSeededRandomPool rng;
    std::queue<pk_t> stun_servers;
    factory_map stun_attribute_factories;
    Networking* networking_;

    QHostAddress address_waiting_to_connect;
    std::uint16_t port_waiting_to_connect;
    stun_header_ptr header_waiting_to_connect;
    pk_t connecting_to = 0;
    bool message_waiting_to_connect = false;
    bool save_socket = false;
    int failed_connections = 0;
    std::map<pk_t, int> failed_connections_per_server;

    pk_t stun_server_awaiting_confirmation;

    bool nat_active_flag = false;

    
};
using stun_client_ptr = std::shared_ptr<StunClient>;

#endif
