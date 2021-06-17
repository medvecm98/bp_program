#include "Networking.h"

bool Networking::enroll_message_to_be_sent(ProtoMessage&& message) {
    to_send_msg.push(std::make_unique< ProtoMessage>( message));
    return true;
}

void Networking::send_message(tcp::socket& tcp_socket, msg_queue& send_mq) {
    //serialize message
    std::string serialized_msg;
	send_mq.front()->SerializeToString(&serialized_msg);
	send_mq.pop();

    //append length of message to the message, without the length itself
    std::stringstream length_plus_msg;
    length_plus_msg << std::setfill('0') << std::setw(16) << std::hex << serialized_msg.size();
    length_plus_msg << serialized_msg;

    //send message
    boost::asio::write( tcp_socket, boost::asio::buffer( length_plus_msg.str(), length_plus_msg.str().size()));
}

ProtoMessage Networking::receive_message(tcp::socket& tcp_socket) {
	//receive message length
	char recv_msg_len[16 + 1];
	boost::asio::read( tcp_socket, boost::asio::buffer( recv_msg_len, 16));
	recv_msg_len[16] = '\0';
	std::size_t msg_len = std::stoll(std::string(recv_msg_len), nullptr, 16);

	//read method requires char array
	//receive message
	char msg[msg_len + 1];
	boost::asio::read( tcp_socket, boost::asio::buffer( msg, msg_len));
	msg[msg_len] = '\0';

	std::string s_msg(msg);

	//deserialize
	ProtoMessage m;
	m.ParseFromString(s_msg);
	return m;
}