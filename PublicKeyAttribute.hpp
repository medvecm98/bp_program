#ifndef NP2PS_PUBLIC_KEY_ATTRIBUTE
#define NP2PS_PUBLIC_KEY_ATTRIBUTE

#include "StunMessages.hpp"

class PublicKeyAttribute : public StunMessageAttribute {
public:
    PublicKeyAttribute();

    virtual void initialize(const CryptoPP::RSA::PublicKey& public_key, StunMessageHeader* h);

    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t length, std::uint16_t type) override;

    virtual void write_stun_attribute(QDataStream&) override;

    void set_value(const CryptoPP::RSA::PublicKey& public_key);

    CryptoPP::RSA::PublicKey get_value();

    std::uint16_t get_value_size();

private:
    std::string value;
};

class PublicKeyAttributeFactory : public StunMessageAttributeFactory {
    stun_attr_ptr create() override {
        return std::make_shared<PublicKeyAttribute>();
    }
};

#endif