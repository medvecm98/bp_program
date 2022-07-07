#include "RelayedPublicIdentifierAttribute.hpp"

RelayedPublicIdentifierAttribute::RelayedPublicIdentifierAttribute()  : StunMessageAttribute::StunMessageAttribute() {
    stun_attr_type = STUN_ATTR_RELAYED_PUBLIC_IDENTIFIER;
    attribute_type = StunAttributeEnum::relayed_publid_identifier;
}

void RelayedPublicIdentifierAttribute::initialize(pk_t public_identifier, StunMessageHeader* h) {
    StunMessageAttribute::initialize(8, h);
    this->public_identifier = (quint64)public_identifier;
}

std::uint16_t RelayedPublicIdentifierAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);
    input >> public_identifier;
    return 8;
}

void RelayedPublicIdentifierAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    output << public_identifier;
}


void RelayedPublicIdentifierAttribute::set_public_identifier(pk_t pid) {
    public_identifier = (quint64) pid;
}

pk_t RelayedPublicIdentifierAttribute::get_public_identifier() {
    return (pk_t) public_identifier;
}