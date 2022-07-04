#ifndef NP2PS_PUBLIC_IDENTIFIER_ATTRIBUTE
#define NP2PS_PUBLIC_IDENTIFIER_ATTRIBUTE

#include "StunMessages.hpp"
#include "GlobalUsing.h"

class PublicIdentifierAttribute : public StunMessageAttribute {
public:
    PublicIdentifierAttribute();

    virtual void initialize(pk_t public_identifier, StunMessageHeader* h);

    virtual std::uint16_t read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) override;

    virtual void write_stun_attribute(QDataStream& output) override;

    void set_public_identifier(pk_t pid);

    pk_t get_public_identifier();

    quint64 public_identifier;
};

class PublicIdentifierAttributeFactory : public StunMessageAttributeFactory {
    stun_attr_ptr create() override {
        return std::make_shared<PublicIdentifierAttribute>();
    }
};

#endif