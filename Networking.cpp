#include "Networking.h"

bool Networking::enroll_message_to_be_sent(ProtoMessage&& message) {
    to_send_msg.push(std::make_unique< ProtoMessage>( message));
    return true;
}

void Networking::send_message(tcp::socket& tcp_socket) {
    //serialize message
    std::string serialized_msg;
    to_send_msg.front()->SerializeToString(&serialized_msg);
    to_send_msg.pop();

    //append length of message to the message, without the length itself
    std::stringstream length_plus_msg;
    length_plus_msg << std::setfill('0') << std::setw(16) << std::hex << serialized_msg.size();
    length_plus_msg << serialized_msg;

    //send message
    boost::asio::write( tcp_socket, boost::asio::buffer( length_plus_msg.str(), length_plus_msg.str().size()));
}

ProtoMessage Networking::receive_message(tcp::socket& tcp_socket) {
	//receive
	std::string recv_msg_len;
	boost::asio::read( tcp_socket, boost::asio::buffer( recv_msg_len, 16));
	std::size_t msg_len = std::stoll(recv_msg_len, 0, 16);
	std::string msg;
	msg.reserve(msg_len);
	boost::asio::read( tcp_socket, boost::asio::buffer( msg, msg_len));

	//deserialize
	ProtoMessage m;
	m.ParseFromString(msg);
	return m;
}