#include "PublicIdentifierAttribute.hpp"

PublicIdentifierAttribute::PublicIdentifierAttribute()  : StunMessageAttribute::StunMessageAttribute() {
    stun_attr_type = STUN_ATTR_PUBLIC_IDENTIFIER;
    attribute_type = StunAttributeEnum::public_identifier;
}

void PublicIdentifierAttribute::initialize(pk_t public_identifier, StunMessageHeader* stun_header) {
    StunMessageAttribute::initialize(8, stun_header);
    this->public_identifier = (quint64)public_identifier;
}

std::uint16_t PublicIdentifierAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);
    StunMessageAttribute::socket_wait_for_read<quint64>((QTcpSocket*)input.device());
    input >> public_identifier;
    return 8;
}

void PublicIdentifierAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    output << public_identifier;
}


void PublicIdentifierAttribute::set_public_identifier(pk_t pid) {
    public_identifier = (quint64) pid;
}

pk_t PublicIdentifierAttribute::get_public_identifier() {
    return (pk_t) public_identifier;
}
