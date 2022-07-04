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

using tcp = boost::asio::ip::tcp;

using MFW = MessageFactoryWrapper;
using msg_queue = std::queue< unique_ptr_message>;
using msg_queue_ptr = std::shared_ptr< msg_queue>;
using msg_map = std::unordered_map< std::size_t, unique_ptr_message>;

class StunClient;

#define PORT 14128
static constexpr char NORMAL_MESSAGE = 'A';
static constexpr char KEY_MESSAGE = 'B';

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
	void restart_server(bool);

public slots:
	void message_receive();
	void process_received_np2ps_message(QByteArray& msg, QHostAddress ip, std::uint16_t port);
	void prepare_for_message_receive();
	void display_error(QAbstractSocket::SocketError e) {
		QTextStream(stderr) << "Code " << e << "\n";
		return;
	}

private:
	QTcpServer* tcp_server_ = nullptr;
	QTcpSocket* tcp_socket_ = nullptr;
	QDataStream in_;
	networking_ptr networking_;
};

enum class ConnectionStatus {
	connection_ok = 1,
	connection_fail = 2
};
using ConnectionMap = std::map<std::pair<std::uint32_t, std::uint16_t>, std::pair<unique_ptr_message, IpWrapper>>;

/**
 * Client side of the P2P peer.
 */
class PeerSender : public QObject {
	Q_OBJECT

public:
	PeerSender(networking_ptr net);
	void message_send(unique_ptr_message msg, IpWrapper& ipw);
	void message_send(QTcpSocket* socket, unique_ptr_message msg, IpWrapper& ipw, bool);
	void try_connect(unique_ptr_message msg, IpWrapper& ipw);

public slots:
	void display_error(QAbstractSocket::SocketError e) {
		QTextStream(stderr) << "Code " << e << "\n";
		return;
	}

	void host_connected();
	void handle_connection_error();

private:
	QTcpSocket* tcp_socket_ = nullptr;
	CryptoPP::AutoSeededRandomPool prng_;
	networking_ptr networking_;
	ConnectionMap connection_map;
};


class Networking : public QObject, public std::enable_shared_from_this<Networking> {
	Q_OBJECT

public:
	Networking() {
		sender_receiver_initialized = false;

		QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
		for (int i = 0; i < ipAddressesList.size(); ++i) {
			if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
				ipAddressesList.at(i).toIPv4Address()) {
				ip_map_.my_ip.ipv4 = ipAddressesList.at(i);
				break;
			}
		}

		stun_server = std::make_shared<StunServer>();
		stun_client = std::make_shared<StunClient>();

		std::cout << ip_map_.my_ip.ipv4.toString().toStdString() << '\n';

		QObject::connect(this, &Networking::new_message_enrolled,
						 this, &Networking::send_message);
	}

	bool enroll_message_to_be_sent(unique_ptr_message message);

	void init_sender_receiver(news_database* nd);


	std::optional<seq_t> receive_message(QTcpSocket* tcp_socket);

	void store_to_map(unique_ptr_message&& msg) {
		waiting_level.emplace(msg->seq(), std::move(msg));
	}

	bool check_if_in_map(std::size_t seq) {
		return waiting_level.find(seq) != waiting_level.end();
	}

	unique_ptr_message load_from_map(std::size_t seq) {
		auto msg_iter = waiting_level.find(seq);
		unique_ptr_message rv = std::move(msg_iter->second);
		waiting_level.erase(msg_iter);
		return std::move(rv);
	}

	void add_to_received(unique_ptr_message msg) {
		emit new_message_received(msg);
	}

	void add_to_messages_to_decrypt(pk_t pk_str, EncryptedMessageWrapper&& emw) {
		waiting_decrypt.emplace(pk_str, emw);
	}
	
	void sign_and_encrypt_key(std::stringstream& output, CryptoPP::SecByteBlock& key, pk_t sender, pk_t receiver);
	void generate_rsa_key_pair();

	void restart_server(bool restart = true) {
		receiver_->restart_server(restart);
	}

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void save(Archive& ar, const unsigned int version) const {
		ar & ip_map_;
		ar & soliciting_articles;
		ar & news_db;
		bool g;
		ar & g;
	}

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void load(Archive& ar, const unsigned int version) {
		ar & ip_map_;
		ar & soliciting_articles;
		ar & news_db;
		bool g;
		ar & g;
	}

	std::shared_ptr<StunClient> get_stun_client() {
		return stun_client;
	}

	CryptoPP::AutoSeededRandomPool& get_prng() {
		return prng_;
	}

	void pass_message_to_receiver(QByteArray& msg, QHostAddress ip, std::uint16_t port) {
		receiver_->process_received_np2ps_message(msg, ip, port);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()

	IpMap ip_map_;
	std::map<hash_t, std::vector<pk_t>> soliciting_articles;
	std::unordered_multimap<pk_t, unique_ptr_message> waiting_symmetrich_exchange;

public slots:
	void send_message(unique_ptr_message);
	void send_message_again_ip(pk_t message_originally_to);
	void decrypt_encrypted_messages(pk_t symmetric_key_sender);
	void user_member_results(seq_t msg_seq, bool is_member);
	void symmetric_exchanged(pk_t other_peer);

signals:
	void new_message_enrolled(unique_ptr_message);
	void new_message_received(unique_ptr_message);

private:
	const int port_ = PORT;

	news_database* news_db;
	msg_map waiting_level; //<seq number of message, message>
	encrypted_message_map waiting_decrypt; //<pk_t of other holder of symmetric key, message>
	std::unordered_multimap<pk_t, unique_ptr_message> waiting_ip; //<pk_t of receiver, message to send to receiver>
	CryptoPP::AutoSeededRandomPool prng_;
	bool sender_receiver_initialized;
	std::shared_ptr<PeerSender> sender_;
	std::shared_ptr<PeerReceiver> receiver_;
	
	QDataStream in_;


	//TODO: "pointerize" STUN client and server

	std::shared_ptr<StunClient> stun_client;
	std::shared_ptr<StunServer> stun_server;

	//boost::asio necessities
	boost::asio::io_context io_ctx;

};

#endif //PROGRAM_NETWORKING_H
