#include "LifetimeAttribute.hpp"

LifetimeAttribute::LifetimeAttribute() : StunMessageAttribute::StunMessageAttribute()  {
    stun_attr_type = STUN_ATTR_LIFETIME; //numeric form
    attribute_type = StunAttributeEnum::lifetime; //enum form
}

std::uint16_t LifetimeAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);

    StunMessageAttribute::socket_wait_for_read<std::uint32_t>((QTcpSocket*)input.device());
    input >> time; //reads time

    return 4;
}

void LifetimeAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    output << time; //writes time
}

void LifetimeAttribute::initialize(std::uint32_t t, StunMessageHeader* stun_header) {
    StunMessageAttribute::initialize(4, stun_header);

    time = t; //sets time
}
