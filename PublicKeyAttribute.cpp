#include "PublicKeyAttribute.hpp"

PublicKeyAttribute::PublicKeyAttribute() : StunMessageAttribute() {
    stun_attr_type = STUN_ATTR_PUBLIC_KEY;
    attribute_type = StunAttributeEnum::public_key;
}

void PublicKeyAttribute::initialize(const CryptoPP::RSA::PublicKey& public_key, StunMessageHeader* h) {
    CryptoPP::ByteQueue bq;
	public_key.Save(bq);
	CryptoPP::StringSink ss(value);
	bq.CopyTo(ss);
    StunMessageAttribute::initialize(value.size(), h);
}

std::uint16_t PublicKeyAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    StunMessageAttribute::read_stun_attribute(input, length, type);
    
    QString val(length);
    input >> val;
    value = val.toStdString();

    unpad(input, length);

    return length;
}

void PublicKeyAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);

    QString val = QString::fromStdString(value);
    output << val;
    pad(output, val.size());
}

void PublicKeyAttribute::set_value(const CryptoPP::RSA::PublicKey& public_key) {
    CryptoPP::ByteQueue bq;
	public_key.Save(bq);
	CryptoPP::StringSink ss(value);
	bq.CopyTo(ss);
    ss.MessageEnd();
}

CryptoPP::RSA::PublicKey PublicKeyAttribute::get_value() {
    CryptoPP::RSA::PublicKey pk;
    CryptoPP::ByteQueue bq;
    CryptoPP::StringSource ss(value, true);
    ss.CopyTo(bq);
    pk.Load(bq);
    bq.MessageEnd();
    return pk;
}

std::uint16_t PublicKeyAttribute::get_value_size() {
    return value.size();
}