#ifndef XOR_RELAYED_ADDRESS
#define XOR_RELAYED_ADDRESS

#include "StunMessages.hpp"

class XorRelayedAddressAttribute : public StunMessageAttribute {
public:
    XorRelayedAddressAttribute() : StunMessageAttribute() {
        stun_attr_type = STUN_ATTR_XOR_RELAYED_ADDRESS;
        attribute_type = StunAttributeEnum::xor_relayed_address;
    }

    XorRelayedAddressAttribute(std::uint16_t type, StunMessageHeader* header, std::uint8_t family) : StunMessageAttribute(type, header) {
        address_family = family;
    }

    std::uint16_t initialize(StunMessageHeader* header, std::uint16_t family, QHostAddress address, std::uint16_t port);

    void set_address(std::uint32_t ipv4);

    void set_address(Q_IPV6ADDR ipv6);

    std::uint32_t get_address();

    Q_IPV6ADDR get_address_6();

    QString get_address_qstring();

    void set_port(std::uint16_t port);

    std::uint16_t get_port();

    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t, std::uint16_t) override;

    virtual void write_stun_attribute(QDataStream&) override;


private:
    std::uint16_t address_family;
    std::uint32_t address_ipv4;
    std::uint16_t tcp_port;
    std::array<std::uint32_t, 4> address_ipv6;
};

class XorRelayedAddressAttributeFactory : public StunMessageAttributeFactory {
    stun_attr_ptr create() override {
        return std::make_shared<XorRelayedAddressAttribute>();
    }
};

#endif