#ifndef PROGRAM_NETWORKING_H
#define PROGRAM_NETWORKING_H

#include "IpMap.h"
#include "Message.h"
#include <queue>
#include <memory>
#include "protobuf_source/messages.pb.h"
#include "GlobalUsing.h"
#include "NewspaperEntry.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include "cryptopp/rsa.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/osrng.h"
#include "StunClient.hpp"
#include "StunServer.hpp"

#include <QObject>
#include <QApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtCore>

using reader_database = std::unordered_multimap<hash_t, PeerInfo*>;
using MFW = MessageFactoryWrapper;
using msg_queue = std::queue< shared_ptr_message>;
using msg_queue_ptr = std::shared_ptr< msg_queue>;
using msg_map = std::unordered_map< std::size_t, shared_ptr_message>;

class StunClient;
class StunServer;

static constexpr quint16 ENCRYPTED_MESSAGE = 0x0000;
static constexpr quint16 PLAIN_MESSAGE = 0x0001;

static constexpr quint16 VERSION = 0x0001;

/**
 * @brief Wrapper for encrypted messages where peer don't have symmetric key stored locally, or when public key is unknown.
 * 
 * Messages are stored here in string and will be decrypted once the symmetric key is known, or when public key is known.
 */
struct EncryptedMessageWrapper {
	EncryptedMessageWrapper() = default;

	EncryptedMessageWrapper(const std::string& encrypted_message, const CryptoPP::SecByteBlock& initialization_vector, pk_t public_identifier, int type) {
		this->encrypted_message_or_symmetric_key = encrypted_message;
		this->initialization_vector_or_signature = initialization_vector;
		this->public_identifier = public_identifier;
		this->type = type;
	}

	int type; // uses same numbers as message class identifier (NORMAL or KEY)
	std::string encrypted_message_or_symmetric_key;
	CryptoPP::SecByteBlock initialization_vector_or_signature;
	pk_t public_identifier;
};

using encrypted_message_map = std::unordered_multimap< pk_t, EncryptedMessageWrapper>;

class PeerReceiver;
class PeerSender;
class Networking;

using networking_ptr = std::shared_ptr<Networking>;

/**
 * @brief Class for implementing Receiver part of NP2PS Peer.
 * 
 * Runs server and accepts new connections.
 * 
 * processes new NP2PS messages either directly from network or from STUN
 * client
 */
class PeerReceiver : public QObject {
	Q_OBJECT

public:
	/**
	 * @brief Construct a new PeerReceiver object.
	 * 
	 * Server is NOT yet initialized.
	 * 
	 * @param net Pointer to networking that created this receiver.
	 */
	PeerReceiver(networking_ptr net);

public slots:
	/**
	 * @brief Starts server for given IP address.
	 * 
	 * Once started, can't be started again...
	 * 
	 * Port is set to value 14128.
	 * 
	 * @param address Address of server to start listening on.
	 */
	void start_server(QHostAddress address);

	/**
	 * @brief Receives message.
	 * 
	 * Starts transaction on QDataStream `in_`.
	 * 
	 */
	void message_receive(QTcpSocket* socket);

	/**
	 * @brief Processes a received NP2PS message.
	 * 
	 * To be used either directly with socket, or without, when message was
	 * received from STUN server via relaying.
	 * 
	 * @param msg Message received, stored in stream.
	 * @param socket Socket that received this message, or NULL, when received from STUN.
	 */
	void process_received_np2ps_message(QDataStream& msg, QTcpSocket* socket);

	/**
	 * @brief Sets up QDataStream for incoming data and creates QTcp Socket.
	 * 
	 * 
	 * Connects socket to its slots.
	 */
	void prepare_for_message_receive();

	/**
	 * @brief Used when message was received through already connected
	 * socket.
	 * 
	 * Passes message in stream form to `process_received_np2ps_message`.
	 */
	void message_receive_connected();

	/**
	 * @brief Prints error code.
	 * 
	 * @param e Error.
	 */
	void display_error(QAbstractSocket::SocketError e) {
		QTextStream(stderr) << "StunReceiver server error code: " << e << "\n";
		return;
	}

private:
	QTcpServer* tcp_server_ = nullptr;
	// QTcpSocket* tcp_socket_ = nullptr;
	QDataStream in_; //stream for incoming messages
	networking_ptr networking_;
	bool server_started = false;
};

/**
 * Client side of the NP2PS peer.
 */
class PeerSender : public QObject {
	Q_OBJECT

public:
	/**
	 * @brief Construct a new PeerSender object.
	 * 
	 * @param net 
	 */
	PeerSender(networking_ptr net);

	/**
	 * @brief Sends given NP2PS message.
	 * 
	 * Tries to connect to STUN server.
	 * 
	 * Message is then sent.
	 * 
	 * @param msg Message to send.
	 * @param ipw IpWrapper of the receiver of the message.
	 */
	void message_send(shared_ptr_message msg, IpWrapper& ipw);

	/**
	 * @brief Sends given NP2PS message.
	 * 
	 * Will directly send using provided socket.
	 * 
	 * Message is serialized and encrypted. When symmetric key is not yet
	 * present, message send is postponed and symmetric key is generated and
	 * exchanged.
	 * 
	 * @param socket Socket to use when sending.
	 * @param msg Messge to send.
	 * @param ipw IpWrapper of the receiver of the message.
	 * @param relay Should the message be relayed?
	 */
	void message_send(QTcpSocket* socket, shared_ptr_message msg, IpWrapper ipw, bool relay);

	/**
	 * @brief Tries to connect to receiver.
	 * 
	 * Tries to find active connection to peer at first, then tries to connect
	 * directly, then it will relay thorugh STUN server.
	 * 
	 * @param msg Message to send.
	 * @param ipw IpWrapper of the receiver.
	 */
	void try_connect(shared_ptr_message msg, IpWrapper& ipw);

	/**
	 * @brief Tries to connect to receiver.
	 * 
	 * Tries to find active connection to peer at first, then tries to connect
	 * directly, then it will relay thorugh STUN server.
	 * 
	 * @param msg Message to send.
	 * @param ipw IpWrapper of the receiver.
	 */
	void try_connect(QByteArray msg, IpWrapper& ipw);

public slots:
	/**
	 * @brief Displats PeerSender socket error.
	 * 
	 * @param e Error.
	 */
	void display_error(QAbstractSocket::SocketError e) {
		QTextStream(stderr) << "PeerSender socket error: " << e << "\n";
		return;
	}

	/**
	 * @brief Slot when host is connected to receiver.
	 * 
	 * Message waiting to be sent is now sent.
	 * 
	 * 
	 * 
	 */
	void host_connected();

	/**
	 * @brief Slot for when connection to host fails.
	 * 
	 * When connection fails, we will try to relay the message via STUN.
	 * 
	 */
	void handle_connection_error();

private:
	QTcpSocket* tcp_socket_ = nullptr;
	CryptoPP::AutoSeededRandomPool prng_;
	networking_ptr networking_;

	/* Message and IpWrapper when waiting to be connected. */
	shared_ptr_message message_waiting_for_connection;
	IpWrapper connectee_waiting_for_connection;
};


/**
 * @brief Central class for everything related to networking.
 * 
 * Holds pointers to PeerSender and PeerReceiver, StunClient and StunServer.
 * 
 */
class Networking : public QObject, public std::enable_shared_from_this<Networking> {
	Q_OBJECT

public:
	friend StunClient;

	Networking() = delete;

	/**
	 * @brief Construct a new Networking object.
	 * 
	 * StunClient and StunServer are initiliazed.
	 * 
	 * Connects signal `new_message_enrolled` to `send_message` slot.
	 */
	Networking(pk_t public_id) : ip_map_(public_id)
	{
		sender_receiver_initialized = false;

		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
		for (int i = 0; i < ipAddressesList.size(); ++i) {
			if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
				ipAddressesList.at(i).toIPv4Address())
			{
				auto address = ipAddressesList.at(i);
				add_to_ip_map(public_id, std::move(address));
				break;
			}
		}

		stun_server = std::make_shared<StunServer>(this);
		stun_client = std::make_shared<StunClient>(this);

		QObject::connect(this, &Networking::new_message_enrolled, this, &Networking::send_message);
	}

	Networking(const np2ps::IpMap& serialized_ip_map, pk_t public_id) : Networking(public_id) {
		ip_map_ = IpMap(serialized_ip_map);
	}

	/**
	 * @brief Enrolls message to be sent.
	 * 
	 * Message is sent afterwards enrolled via `send_message`.
	 * 
	 * @param message Message to send.
	 * @return true Always.
	 */
	bool enroll_message_to_be_sent(shared_ptr_message message);

	/**
	 * @brief Initializes PeerSender and PeerReceiver.
	 * 
	 * Pointer to news database is passed to receive and sender as well.
	 * 
	 * @param nd Pointer to peer news database.
	 */
	void init_sender_receiver(news_database* nd);

	IpMap& ip_map() {
		return ip_map_;
	}

	/**
	 * @brief Emits `new_message_received`.
	 * 
	 * Message is given to Peer for processing.
	 * 
	 * @param msg Just received message to process.
	 */
	void add_to_received(shared_ptr_message msg) {
		emit new_message_received(msg);
	}

	/**
	 * @brief Store message for later, when it can be unlocked with symmetric key.
	 * 
	 * @param pk_str Public identifier of the sender.
	 * @param emw Message wrapper to store message in.
	 */
	void add_to_messages_to_decrypt(pk_t pk_str, EncryptedMessageWrapper&& emw) {
		waiting_decrypt.emplace(pk_str, emw);
	}

	/**
	 * @brief Signs then encrypts given EAX + AES key.
	 * 
	 * Output is written into `std::stringstream`. Public and private keys will be generated, if weren't before.
	 * Protobuf is used as usual. Byte with value `20` is prepended, indicating a message containing symmetric key.
	 * 
	 * @param output Where to write the message.
	 * @param key Key to sign and encrypt.
	 * @param sender Sender of the message, creator of the symmetric key.
	 * @param receiver Receiver of the message.
	 */	
	shared_ptr_message sign_and_encrypt_key(CryptoPP::ByteQueue& key, pk_t sender, pk_t receiver);

	/**
	 * @brief Generates and stores private and public key for Peer.
	 * 
	 */
	void generate_rsa_key_pair();

	std::shared_ptr<StunClient> get_stun_client() {
		return stun_client;
	}

	/**
	 * @brief Get random number generator from Crpyto++.
	 * 
	 * @return CryptoPP::AutoSeededRandomPool random generator.
	 */
	CryptoPP::AutoSeededRandomPool& get_prng() {
		return prng_;
	}

	/**
	 * @brief Passes relayed STUN message to receiver.
	 * 
	 * @param msg Relayed message just received by STUN.
	 */
	void pass_message_to_receiver(QDataStream& msg) {
		receiver_->process_received_np2ps_message(msg, NULL);
	}

	/**
	 * @brief Return pointer to PeerReceiver object.
	 * 
	 * Useful for starting its server from MainWindow.
	 * 
	 * @return PeerReceiver pointer.
	 */
	PeerReceiver* get_peer_receiver() {
		return receiver_.get();
	}

	/**
	 * @brief Sets the pointers to maps.
	 * 
	 * Maps are stored in Peer and access is provided to Networking via
	 * pointers.
	 * 
	 * @param m User map pointer.
	 * @param n News database pointer.
	 * @param r Readers pointer.
	 * @param j Journalists pointer.
	 */
	void set_maps(user_level_map* m, news_database* n, reader_database* r, user_container* j) {
		user_map = m;
		news_db = n;
		readers_ = r;
		journalists_ = j;
	}

	/**
	 * @brief Set the public identifier of the Peer.
	 * 
	 * Called by Peer constructor.
	 * 
	 * @param pid Public identifier to set.
	 */
	void set_peer_public_id(pk_t pid);

	/**
	 * @brief Get the public identifier of the peer.
	 * 
	 * @return Peer's public idenitifer.
	 */
	pk_t get_peer_public_id();

	/**
	 * @brief Gets all available network interfaces and their IP addresses.
	 * 
	 */
	void get_network_interfaces();

	StunServer* get_stun_server() {
		return stun_server.get();
	}

	void enroll_new_peer(std::string ip, pk_t id) {
		QHostAddress ip4(QString::fromStdString(ip));
		ip_map_.add_to_map(id, ip4, PORT);
	}

	void start_servers_with_first_ip();

	bool add_to_ip_map(pk_t id, QHostAddress&& address);

	eax_optional get_or_create_eax(shared_ptr_message msg);
	IpWrapper& save_symmetric_key(pk_t save_to, CryptoPP::ByteQueue&& aes_key);

	shared_ptr_message generate_symmetric_key_message(shared_ptr_message msg);
	CryptoPP::ByteQueue generate_symmetric_key();
	void identify_peer_save_message(shared_ptr_message);

	IpMap ip_map_; //map of all IPs, ports and RSA public keys
	std::map<hash_t, std::vector<pk_t>> soliciting_articles; //articles waiting to be found in the network
	std::unordered_multimap<pk_t, shared_ptr_message> waiting_symmetric_exchange; //messages waiting to be sent while symmetric key is exchanged
	user_level_map* user_map;
	std::map<pk_t, shared_ptr_message> waiting_symmetric_key_messages;
	std::multimap<pk_t, shared_ptr_message> messages_waiting_for_credentials;
	std::map<quint32, std::string> newspapers_awaiting_identification;

public slots:
	/**
	 * @brief Sends message into the network using provided TCP socket.
	 * 
	 * Message is encrypted almost all the time with EAX and AES. The only exception is when EAX + AES key is being
	 * exhanged, in which case the key is signed, and encrypted using RSA.
	 * 
	 * ```
	 * First two bytes:
	 * ...containing 0x0000 -> Standard message
	 * ...conatining 0x0001 -> Key exchange message
	 * ```
	 * 
	 * @param msg Pointer to message to send.
	 */
	void send_message(shared_ptr_message);

	/**
	 * @brief Decrypts all symmetric messages by given sender.
	 * 
	 * Uses messages from `waiting_decrypt` container.
	 * 
	 * @param symmetric_key_sender Original sender of the messages waiting to be decrypted.
	 */
	void decrypt_encrypted_messages(pk_t symmetric_key_sender);

	/**
	 * @brief Symmetric key exchanged with given peer.
	 * 
	 * Messages that are waiting to be sent are now sent.
	 * 
	 * @param other_peer Other peer that key needed to be exchanged with.
	 */
	void symmetric_exchanged(pk_t other_peer);

	/**
	 * @brief Deletes disconnected users from databases.
	 * 
	 */
	void peer_process_disconnected_users();

signals:
	/**
	 * @brief Emitted, when new message was received and read.
	 * 
	 */
	void new_message_enrolled(shared_ptr_message);

	/**
	 * @brief Emitted, when new message was received.
	 * 
	 */
	void new_message_received(shared_ptr_message);

	/**
	 * @brief Emitted, when newspaper identified themselves.
	 * 
	 * @param id ID of the chief editor of the newspaper
	 * @param newspaper_name Name of the newspaper
	 * @param newspaper_ip_domain IP of the newspaper
	 */
	void newspaper_identified(pk_t id, my_string newspaper_name, std::string newspaper_ip_domain);

	/**
	 * @brief Emitted, when all network interfaces and their addresses were gathered.
	 * 
	 * @param addresses_and_interfaces 
	 */
	void got_network_interfaces(address_vec_ptr addresses_and_interfaces);

private:
	const int port_ = PORT;

	news_database* news_db;
	reader_database* readers_;
	user_container* journalists_;

	encrypted_message_map waiting_decrypt; //<pk_t of other holder of symmetric key, message>
	std::unordered_multimap<pk_t, shared_ptr_message> waiting_ip; //<pk_t of receiver, message to send to receiver>
	CryptoPP::AutoSeededRandomPool prng_;
	bool sender_receiver_initialized;
	std::shared_ptr<PeerSender> sender_;
	std::shared_ptr<PeerReceiver> receiver_;
	pk_t peer_public_id;
	
	QDataStream in_;

	std::shared_ptr<StunClient> stun_client;
	std::shared_ptr<StunServer> stun_server;
};

#endif //PROGRAM_NETWORKING_H
