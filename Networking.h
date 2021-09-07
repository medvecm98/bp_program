#ifndef PROGRAM_NETWORKING_H
#define PROGRAM_NETWORKING_H

/*
class Peer;
#include "Peer.h"
*/

#include "IpMap.h"
#include "Message.h"
#include <boost/asio.hpp>
#include <queue>
#include <memory>
#include "protobuf_source/messages.pb.h"
#include "GlobalUsing.h"
#include "NewspaperEntry.h"
#include <sstream>
#include <iomanip>
#include "cryptopp/rsa.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/osrng.h"

#include <QObject>
#include <QApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtCore>

using tcp = boost::asio::ip::tcp;

using MFW = MessageFactoryWrapper;
using msg_queue = std::queue< unique_ptr_message>;
using msg_queue_ptr = std::shared_ptr< msg_queue>;
using msg_map = std::unordered_map< std::size_t, unique_ptr_message>;

#define PORT 14128
static constexpr int NORMAL_MESSAGE = 10;
static constexpr int KEY_MESSAGE = 20;

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

class PeerReceiver : public QObject {
	Q_OBJECT

public:
	PeerReceiver(networking_ptr net);

public slots:
	void message_receive();

private slots:
	void display_error(QAbstractSocket::SocketError e) {
		//TODO: implement
		return;
	}

private:
	QTcpServer* tcp_server_ = nullptr;
	QTcpSocket* tcp_socket_ = nullptr;
	QDataStream in_;
	networking_ptr networking_;
};

/**
 * Client side of the P2P peer.
 */
class PeerSender : public QObject {
	Q_OBJECT

public:
	PeerSender(networking_ptr net);
	void message_send(unique_ptr_message msg, IpWrapper& ipw);

private slots:
	void display_error(QAbstractSocket::SocketError) {
		//TODO: implement
		return;
	}

private:
	QTcpSocket* tcp_socket_ = nullptr;
	CryptoPP::AutoSeededRandomPool prng_;
	networking_ptr networking_;
};

class Networking : std::enable_shared_from_this<Networking> {
public:
	Networking(const news_database& nd) 
		: receiver_(PeerReceiver(shared_from_this()))
		, sender_(PeerSender(shared_from_this()))
		, news_db(nd)
	{}

	//template<class MSG>
	bool enroll_message_to_be_sent(unique_ptr_message message);

	void send_message(tcp::socket&, unique_ptr_message, IpWrapper&);
	std::optional<seq_t> receive_message(QTcpSocket* tcp_socket);
	
	unique_ptr_message pop_message() {
		auto msg = std::move(received_msg.front());
		received_msg.pop();
		return std::move(msg);
	}

	void store_to_map(unique_ptr_message&& msg) {
		waiting_messages.insert( {msg->seq(), std::move(msg)} );
	}

	bool check_if_in_map(std::size_t seq) {
		return waiting_messages.find(seq) != waiting_messages.end();
	}

	unique_ptr_message load_from_map(std::size_t seq) {
		auto msg_iter = waiting_messages.find(seq);
		unique_ptr_message rv = std::move(msg_iter->second);
		waiting_messages.erase(msg_iter);
		return std::move(rv);
	}

	void add_to_received(unique_ptr_message msg) {
		received_msg.push(std::move(msg));
	}

	void add_to_messages_to_decrypt(pk_t pk_str, EncryptedMessageWrapper&& emw) {
		messages_to_decrypt.insert({pk_str, emw});
	}
	
	void sign_and_encrypt_key(std::stringstream& output, CryptoPP::SecByteBlock& key, pk_t sender, pk_t receiver);
	void generate_rsa_key_pair();


	IpMap ip_map_;
	std::map<hash_t, std::vector<pk_t>> soliciting_articles;

private:
	const int port_ = PORT;

	const news_database& news_db;
	msg_queue to_send_msg, received_msg;
	msg_map waiting_messages;
	encrypted_message_map messages_to_decrypt;
	CryptoPP::AutoSeededRandomPool prng_;
	PeerSender sender_;
	PeerReceiver receiver_;
	
	QDataStream in_;

	//boost::asio necessities
	boost::asio::io_context io_ctx;

};

#endif //PROGRAM_NETWORKING_H
