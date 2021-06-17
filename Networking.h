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

using boost::asio::ip::tcp;

class PeerSession;

using ProtoMessage = np2ps::Message;
using unique_ptr_message = std::unique_ptr< ProtoMessage>;
using msg_session_pair = std::pair< unique_ptr_message, PeerSession>;
using msg_session_queue = std::queue< msg_session_pair>;
using msg_queue = std::queue< unique_ptr_message>;

class Networking {
public:
	//template<class MSG>
	bool enroll_message_to_be_sent(ProtoMessage&& message);

	IpMap ip_map_;
	static void send_message(tcp::socket&, msg_queue&);
	static ProtoMessage receive_message(tcp::socket&);
private:
	const std::string port_ = "14128";

	msg_queue to_send_msg, received_msg;
	
	//boost::asio necessities
	boost::asio::io_context io_ctx;
};

/**
 * Handles one session with one specific Peer.
 */
class PeerSession {
public:
	PeerSession(tcp::socket socket, msg_queue& rmq, msg_queue& smq)
			: socket_(std::move(socket)), receive_mq_(rmq), send_mq_(smq)
	{}

	void start() {
		read_message_from_network();
	}
private:
	void read_message_from_network() {
		ProtoMessage msg = Networking::receive_message(socket_);
		receive_mq_.push(std::make_unique< ProtoMessage>(std::move(msg)));
	}
	void write_message_into_network() {
		Networking::send_message(socket_, send_mq_);
	}
	msg_queue& receive_mq_;
	msg_queue& send_mq_;
	tcp::socket socket_;
};

class PeerServer {
public:
	PeerServer(boost::asio::io_context& io_context, short port,
			   msg_queue& rmq, msg_queue& smq)
			: acceptor_(io_context, tcp::endpoint (tcp::v4(), port))
	{
		handle_message_accept(rmq, smq);
	}

	void handle_message_accept(msg_queue& rmq, msg_queue& smq) {
		acceptor_.async_accept(
				[this, &rmq, &smq](boost::system::error_code ec, tcp::socket socket) {
					if (!ec) {
						std::make_shared< PeerSession>(std::move(socket), rmq, smq)->start();
					}
					handle_message_accept(rmq, smq);
				});
	}
private:
	tcp::acceptor acceptor_;
};

#endif //PROGRAM_NETWORKING_H
