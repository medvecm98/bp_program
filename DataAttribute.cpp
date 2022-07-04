#include "DataAttribute.hpp"

DataAttribute::DataAttribute() : StunMessageAttribute::StunMessageAttribute()  {
    stun_attr_type = STUN_ATTR_DATA;
    attribute_type = StunAttributeEnum::data;
}

void DataAttribute::initialize(std::string& d, StunMessageHeader* h) {
    data = QString::fromStdString(d);

    StunMessageAttribute::initialize(data.size(), h);
}

std::uint16_t DataAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);
    input >> data;
    return length;
}

void DataAttribute::write_stun_attribute(QDataStream& output) {
    output << data;
}

std::string DataAttribute::get_np2ps_messsage() {
    return data.toStdString();
}