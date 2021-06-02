#ifndef PROGRAM_NETWORKING_H
#define PROGRAM_NETWORKING_H

#include "IpMap.h"
#include <boost/asio.hpp>
#include "Message.h"
#include <queue>
#include <memory>

using boost::asio::ip::tcp;

class Networking {
public:
	//template<class MSG>
	bool enroll_message_to_be_sent(Message&& message);

	IpMap ip_map_;
private:
	void send_message();
	void receive_message();
	const std::string port_ = "50000";

	std::queue<std::unique_ptr<Message>> to_send_msg;
	std::queue<std::unique_ptr<Message>> received_msg;
	
	//boost::asio necessities
	boost::asio::io_context io_ctx;
	tcp::socket socket_;
	tcp::resolver resolver_;
	tcp::acceptor acceptor_;
};


#endif //PROGRAM_NETWORKING_H
