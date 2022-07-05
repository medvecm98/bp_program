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
 * @param h 
 */
void RequestedTransportAttribute::initialize(std::uint32_t p, StunMessageHeader* h) {
    StunMessageAttribute::initialize(4, h);
    set_protocol(p);
}