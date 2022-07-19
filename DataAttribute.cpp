#include "DataAttribute.hpp"

DataAttribute::DataAttribute() : StunMessageAttribute::StunMessageAttribute()  {
    stun_attr_type = STUN_ATTR_DATA; //numberic form
    attribute_type = StunAttributeEnum::data; //enum form
}

void DataAttribute::initialize(QByteArray& d, StunMessageHeader* h) {
    data = d;

    StunMessageAttribute::initialize(data.size(), h);
}

std::uint16_t DataAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);
    data.resize(length);
    input >> data;
    return length;
}

void DataAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    output << data;
}

std::string DataAttribute::get_np2ps_messsage() {
    return data.toStdString();
}