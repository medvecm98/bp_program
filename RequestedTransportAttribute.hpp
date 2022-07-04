#ifndef NP2PS_REQUESTED_TRANSPORT_ATTRIBUTE
#define NP2PS_REQUESTED_TRANSPORT_ATTRIBUTE

#include "StunMessages.hpp"

#define IANA_TCP 0x06
#define IANA_UDP 0x11

class RequestedTransportAttribute : public StunMessageAttribute {
public:
    RequestedTransportAttribute();

    void set_protocol(std::uint32_t protocol);
    std::uint32_t get_protocol();
    virtual void initialize(std::uint32_t p, StunMessageHeader* h);
    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t length, std::uint16_t type) override;
    virtual void write_stun_attribute(QDataStream&) override;

private:
    std::uint32_t protocol_shifted; //shifted due to RFFU [RFC5766]
};

class RequestedTransportAttributeFactory : public StunMessageAttributeFactory {
    stun_attr_ptr create() override {
        return std::make_shared<RequestedTransportAttribute>();
    }
};

#endif