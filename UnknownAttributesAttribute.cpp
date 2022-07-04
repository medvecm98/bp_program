#include "UnknownAttributesAttribute.hpp"

UnknownAttributesAttribute::UnknownAttributesAttribute() {
    attribute_type = StunAttributeEnum::unknown_attributes;
    stun_attr_type = STUN_ATTR_UNKNOWN_ATTRIBUTES;
}

std::uint16_t UnknownAttributesAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);
    //TODO: Pad properly
    std::uint16_t read_length = 0;
    std::uint16_t unknown_attr;
    QByteArray in_reason(length - 4, 0);

    while (length > 0) {
        input >> unknown_attr;
        unknown_attrs.push_back(unknown_attr);

        length -= 4;
        read_length += 4;
    }

    return read_length;
}

void UnknownAttributesAttribute::write_stun_attribute(QDataStream& output) {
    //TODO: Pad properly
    for (auto&& attr : unknown_attrs) {
        output << attr;
    }
}