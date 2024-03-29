#include "RequestedTransportAttribute.hpp"

RequestedTransportAttribute::RequestedTransportAttribute() : StunMessageAttribute::StunMessageAttribute() {
    attribute_type = StunAttributeEnum::requested_transport;
    stun_attr_type = STUN_ATTR_REQUESTED_TRANSPORT;
}

void RequestedTransportAttribute::set_protocol(std::uint32_t protocol) {
    protocol_shifted = protocol << 24;
}

std::uint32_t RequestedTransportAttribute::get_protocol() {
    return protocol_shifted >> 24;
}

std::uint16_t RequestedTransportAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);

    StunMessageAttribute::socket_wait_for_read<std::uint32_t>((QTcpSocket*)input.device());
    input >> protocol_shifted;

    return 4;
}

void RequestedTransportAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    
    output << protocol_shifted;
}


/**
 * @brief 
 * 
 * @param p Protocol in normal, unshifted, form.
 * @param stun_header 
 */
void RequestedTransportAttribute::initialize(std::uint32_t p, StunMessageHeader* stun_header) {
    StunMessageAttribute::initialize(4, stun_header);
    set_protocol(p);
}
