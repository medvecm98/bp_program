#ifndef NP2PS_LIFETIME_ATTRIBUTE
#define NP2PS_LIFETIME_ATTRIBUTE

#include "StunMessages.hpp"

#define MAX_TIME 3600

class LifetimeAttribute : public StunMessageAttribute {
public:
    LifetimeAttribute();

    virtual void initialize(std::uint32_t t, StunMessageHeader* h);

    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t length, std::uint16_t type) override;

    virtual void write_stun_attribute(QDataStream&);

    std::uint32_t time = 0;
};

class LifetimeAttributeFactory : public StunMessageAttributeFactory {
    stun_attr_ptr create() override {
        return std::make_shared<LifetimeAttribute>();
    }
};

#endif