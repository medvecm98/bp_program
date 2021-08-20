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

using tcp = boost::asio::ip::tcp;

class PeerSession;

using MFW = MessageFactoryWrapper;
using msg_session_pair = std::pair< unique_ptr_message, PeerSession>;
using msg_session_queue = std::queue< msg_session_pair>;
using msg_queue = std::queue< unique_ptr_message>;
using msg_queue_ptr = std::shared_ptr< msg_queue>;
using msg_map = std::unordered_map< std::size_t, unique_ptr_message>;
using PeerSession_ptr = std::shared_ptr<PeerSession>;
using session_map = std::unordered_map< pk_t, PeerSession_ptr>;

#define PORT "14128"
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

class PeerSession;
class PeerServer;
class PeerClient;

class Networking {
public:
	Networking() = default;

	explicit Networking(news_database::const_iterator ci) :
		news_database_begin(ci) {}

	//template<class MSG>
	bool enroll_message_to_be_sent(unique_ptr_message message);

	void send_message(tcp::socket&, unique_ptr_message, IpWrapper&);
	std::optional<seq_t> receive_message(tcp::socket& tcp_socket);
	
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
	
	IpMap ip_map_;

	friend PeerSession;
	friend PeerClient;
	friend PeerServer;

private:
	const std::string port_ = PORT;

	const news_database::const_iterator news_database_begin;
	const news_database::const_iterator news_database_end;
	session_map sessions_;
	msg_queue to_send_msg, received_msg;
	msg_map waiting_messages;
	encrypted_message_map messages_to_decrypt;
	CryptoPP::AutoSeededRandomPool prng_;
	//Peer& peer_;
	
	//boost::asio necessities
	boost::asio::io_context io_ctx;

	void generate_rsa_key_pair();
	void sign_and_encrypt_key(std::stringstream& output, CryptoPP::SecByteBlock& key, pk_t sender, pk_t receiver);
};

/**
 * Handles one session with one specific Peer.
 */
class PeerSession : public std::enable_shared_from_this<PeerSession> {
public:
	PeerSession(tcp::socket socket, Networking& networking)
			: socket_(std::move(socket)), networking_(networking)
	{}

	void start_read() {
		auto msg_seq = read_message_from_network();

		if (msg_seq.has_value())
			networking_.sessions_.insert({msg_seq.value(), shared_from_this()});
	}

	void start_write(unique_ptr_message msg, IpWrapper& ipw) {
		write_into_network(std::move(msg), ipw);
	}

	tcp::socket& get_socket() {
		return socket_;
	}
private:
	std::optional<seq_t> read_message_from_network() {
		return networking_.receive_message(socket_);
	}

	void write_into_network(unique_ptr_message msg, IpWrapper& ipw) {
		networking_.send_message(socket_, std::move(msg), ipw);
	}

	Networking& networking_;
	tcp::socket socket_;
};

class PeerServer {
public:
	PeerServer(boost::asio::io_context& io_context, Networking& networking)
			: acceptor_(io_context, tcp::endpoint (tcp::v4(), 14128)), networking_(networking)
	{
		handle_message_accept();
	}

	void handle_message_accept() {
		acceptor_.async_accept(
				[this](boost::system::error_code ec, tcp::socket socket) {
					if (!ec) {
						std::make_shared< PeerSession>(std::move(socket), networking_)->start_read();
					}
					handle_message_accept();
				});
	}
private:
	tcp::acceptor acceptor_;
	Networking& networking_;
};

/**
 * Client side of the P2P peer.
 */
class PeerClient {
public:
	/**
	 * @brief Construct a new PeerClient object.
	 * 
	 * Receive message queue is only for sessions, that would live long enoguh to write into it.
	 * 
	 * @param io_context Io_context to use with socket.
	 * @param smq Send Message Queue. Queue containing messages to be sent.
	 * @param rmq Receive Message Queue. Queue containing received messages, waiting to be processed.
	 * @param sessions List of all sessions, waiting for answer.
	 * @param peer_ip_map IpMap for sender Peer. Used for getting IpWrapper for requested receiving Peer.
	 */
	PeerClient(boost::asio::io_context& io_context, Networking& networking) :
		socket_(io_context), resolver_(io_context), networking_(networking)
	{
		auto msg = std::move(networking_.to_send_msg.front());
		networking_.to_send_msg.pop();
		handle_message_send(std::move(msg));
	}

	void handle_message_send(unique_ptr_message message) {
		auto ip_map_iter = networking_.ip_map_.get_wrapper_for_pk(message->to());
		if ((ip_map_iter != networking_.ip_map_.get_map_end()) && ip_map_iter->second.key_pair.first.has_value()) {
			IpWrapper& ipw = ip_map_iter->second;
			resolver_.async_resolve(ipw.ipv4, PORT, [this, &message, &ipw](const boost::system::error_code& ec,
					const tcp::resolver::results_type& results) {
				if (!ec) {
					if (networking_.sessions_.find(message->seq()) == networking_.sessions_.end()) {
						//we initialize new session
						boost::asio::async_connect(
							socket_, 
							results, 
							[this, &message, &ipw](
								const boost::system::error_code &ec,
								const tcp::endpoint &endpoint
								) {
									std::make_shared<PeerSession>(std::move(socket_), networking_)->start_write(std::move(message), ipw);
								}
						);
					}
					else {
						//there is active session waiting for response
						auto msg_to = message->to();
						networking_.sessions_[msg_to]->start_write(std::move(message), ipw);
						networking_.sessions_.erase(msg_to);
					}
				}
			});
		}
		else {
			/* 
			 * IP and/or public key needs to be requested. Since we don't know which nespaper may hold this information
			 * we need to ask them all. 
			 */
			news_database::const_iterator news_iter = networking_.news_database_begin;

			for (; news_iter != networking_.news_database_end; news_iter++) {
				std::shared_ptr<std::string> request_string;
				request_string->append("r");
				request_string->append(std::to_string(message->to()));
				networking_.enroll_message_to_be_sent(
					MFW::ReqCredentialsFactory(
						MFW::CredentialsFactory(
							networking_.ip_map_.my_public_id,
							*(news_iter->second.get_first_authority())
						),
						true, true, true, false,
						{request_string}, {}, {}, {}
					)
				);
			}
		}
	}
private:
	tcp::socket socket_;
	tcp::resolver resolver_;
	Networking& networking_;
};

#endif //PROGRAM_NETWORKING_H
