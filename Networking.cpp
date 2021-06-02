#include "Networking.h"

bool Networking::enroll_message_to_be_sent(Message&& message) {
    to_send_msg.push(std::make_unique< Message>( message));
    return true;
}

void Networking::send_message() {
    //serialize message
    my_vector serialized_message = to_send_msg.front()->Serialize();
    to_send_msg.pop();
    //send message
    boost::asio::write( socket_, boost::asio::buffer( serialized_message, serialized_message.size()));
}

void Networking::receive_message() {
	my_vector msg_length;
	my_vector serialized_message;
    boost::asio::read( socket_, boost::asio::buffer( msg_length, 8));
    auto msg_length_uint = (std::size_t *)msg_length.data();

    boost::asio::read( socket_, boost::asio::buffer(serialized_message, *msg_length_uint));
    serialized_message.
}