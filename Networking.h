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

using Message = np2ps::Message;

class Networking {
public:
	//template<class MSG>
	bool enroll_message_to_be_sent(Message&& message);

	IpMap ip_map_;
private:
	void send_message(tcp::socket& tcp_socket);
	Message receive_message(tcp::socket& tcp_socket);
	const std::string port_ = "14128";

	std::queue< std::unique_ptr< Message>> to_send_msg;
	std::queue< std::unique_ptr< Message>> received_msg;
	
	//boost::asio necessities
	boost::asio::io_context io_ctx;
	tcp::socket socket_;
	tcp::resolver resolver_;
	tcp::acceptor acceptor_;
};


#endif //PROGRAM_NETWORKING_H
