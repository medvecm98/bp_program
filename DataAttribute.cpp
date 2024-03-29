#include "DataAttribute.hpp"

DataAttribute::DataAttribute() : StunMessageAttribute::StunMessageAttribute()  {
    stun_attr_type = STUN_ATTR_DATA; //numberic form
    attribute_type = StunAttributeEnum::data; //enum form
}

void DataAttribute::initialize(QByteArray& d, StunMessageHeader* stun_header) {
    data = d;

    StunMessageAttribute::initialize(data.size(), stun_header);
}

std::uint16_t DataAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);
    data.resize(length);
    QTcpSocket* socket = (QTcpSocket*) input.device();
    StunMessageAttribute::socket_read_to_msg_array(length, socket, data);
    data.remove(0, 4);
    unpad(input, length);
    return length;
}

void DataAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    output << data;
    pad(output, data.size());
}

std::string DataAttribute::get_np2ps_messsage() {
    return data.toStdString();
}
