#ifndef PROGRAM_NETWORKING_H
#define PROGRAM_NETWORKING_H

#include "IpMap.h"
#include <boost/asio.hpp>
#include <queue>
#include <memory>
#include "protobuf_source/messages.pb.h"
#include "GlobalUsing.h"
#include <sstream>
#include <iomanip>
#include "cryptopp/rsa.h"

using tcp = boost::asio::ip::tcp;

class PeerSession;

using proto_message = np2ps::Message;
using unique_ptr_message = std::unique_ptr< proto_message>;
using msg_session_pair = std::pair< unique_ptr_message, PeerSession>;
using msg_session_queue = std::queue< msg_session_pair>;
using msg_queue = std::queue< unique_ptr_message>;
using msg_map = std::unordered_map< std::size_t, unique_ptr_message>;
using PeerSession_ptr = std::shared_ptr<PeerSession>;
using session_map = std::unordered_map< pk_t, PeerSession_ptr>;


class Networking {
public:
	//template<class MSG>
	bool enroll_message_to_be_sent(unique_ptr_message message);

	static void send_message(tcp::socket&, msg_queue&);
	static std::optional<seq_t> receive_message(tcp::socket& tcp_socket, msg_queue& recv_mq);
	
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
	IpWrapper my_ip;
private:
	const std::string port_ = "14128";

	msg_queue to_send_msg, received_msg;
	msg_map waiting_messages;
	
	//boost::asio necessities
	boost::asio::io_context io_ctx;
};

/**
 * Handles one session with one specific Peer.
 */
class PeerSession : public std::enable_shared_from_this<PeerSession> {
public:
	PeerSession(tcp::socket socket, msg_queue& mq, session_map& sessions)
			: socket_(std::move(socket)), mq_(mq), sessions_(sessions)
	{}

	void start_read() {
		auto msg_seq = read_message_from_network();

		if (msg_seq.has_value())
			sessions_.insert({msg_seq.value(), shared_from_this()});
	}

	void start_write() {
		write_into_network();
	}

	tcp::socket& get_socket() {
		return socket_;
	}
private:
	std::optional<seq_t> read_message_from_network() {
		return Networking::receive_message(socket_, mq_);
	}

	void write_into_network() {
		Networking::send_message(socket_, mq_);
	}

	msg_queue& mq_;
	tcp::socket socket_;
	session_map& sessions_;
};

class PeerServer {
public:
	PeerServer(boost::asio::io_context& io_context, short port, msg_queue& rmq, session_map& sessions)
			: acceptor_(io_context, tcp::endpoint (tcp::v4(), port)), sessions_(sessions)
	{
		handle_message_accept(rmq);
	}

	void handle_message_accept(msg_queue& rmq) {
		acceptor_.async_accept(
				[this, &rmq](boost::system::error_code ec, tcp::socket socket) {
					if (!ec) {
						std::make_shared< PeerSession>(std::move(socket), rmq, sessions_)->start_read();
					}
					handle_message_accept(rmq);
				});
	}
private:
	tcp::acceptor acceptor_;
	session_map sessions_;
};

/**
 * Client side of the P2P peer.
 */
class PeerClient {
public:
	PeerClient(boost::asio::io_context& io_context, const std::string& ip, const std::string& port, msg_queue& smq, session_map& sessions) :
		socket_(io_context), resolver_(io_context), send_mq_(smq), sessions_(sessions)
	{
		handle_message_send(ip, port);
	}

	void handle_message_send(const std::string& ip, const std::string& port) {
		resolver_.async_resolve(ip, port, [this](const boost::system::error_code& ec,
				const tcp::resolver::results_type& results) {
			//TODO: pop ASAP, this won't work with multiple threads
			if (!ec) {
				if (sessions_.find(send_mq_.front()->seq()) == sessions_.end()) {
					//we initialize new session
					boost::asio::async_connect(
						socket_, 
						results, 
						[this](
							const boost::system::error_code &ec,
							const tcp::endpoint &endpoint
							) {
								
								std::make_shared<PeerSession>(std::move(socket_), send_mq_, sessions_)->start_write();
							}
					);
				}
				else {
					//there is active session waiting for response
					Networking::send_message(sessions_[send_mq_.front()->to()]->get_socket(), send_mq_);
				}
			}
		});
	}
private:
	tcp::socket socket_;
	tcp::resolver resolver_;
	msg_queue& send_mq_;
	session_map& sessions_;
};

#endif //PROGRAM_NETWORKING_H
