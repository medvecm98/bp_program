#include "PublicKeyAttribute.hpp"

PublicKeyAttribute::PublicKeyAttribute() : StunMessageAttribute() {
    stun_attr_type = STUN_ATTR_PUBLIC_KEY;
    attribute_type = StunAttributeEnum::public_key;
}

void PublicKeyAttribute::initialize(const CryptoPP::RSA::PublicKey& public_key, StunMessageHeader* stun_header) {
    value = CryptoUtils::instance().rsa_to_hex(public_key);
    StunMessageAttribute::initialize(value.size(), stun_header);
}

std::uint16_t PublicKeyAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);
    
    QByteArray val;
    val.resize(length);
    
    unpad(input, length);
    StunMessageAttribute::socket_read_to_msg_array(length, (QTcpSocket*)input.device(), val);
    // input >> val;
    value = val.mid(4).toStdString();


    std::cout << "Read PublicKeyAttribute length: " << value.length() << " value: " << value << std::endl;

    return length;
}

void PublicKeyAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);

    QByteArray val(value.data(), value.size());
    output << val;

    std::cout << "Write PublicKeyAttribute length: " << value.size() << " value: " << value << std::endl;
    // pad(output, val.size());
}

void PublicKeyAttribute::set_value(const CryptoPP::RSA::PublicKey& public_key) {
    CryptoPP::ByteQueue bq;
	public_key.Save(bq);
	CryptoPP::StringSink ss(value);
	bq.CopyTo(ss);
    ss.MessageEnd();
}

CryptoPP::RSA::PublicKey PublicKeyAttribute::get_value() {
    return CryptoUtils::instance().hex_to_rsa(value);
}

std::uint16_t PublicKeyAttribute::get_value_size() {
    return value.size();
}
