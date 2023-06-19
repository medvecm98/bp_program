#ifndef XOR_RELAYED_ADDRESS
#define XOR_RELAYED_ADDRESS

#include "StunMessages.hpp"

/**
 * @brief Class for STUN XorRelayedAddress attribute.
 * 
 * Attribute inherits from StunMessageAttribute class.
 */
class XorRelayedAddressAttribute : public StunMessageAttribute {
public:
    /**
     * @brief Construct a new XorRelayedAddress object.
     * 
     * Sets the attribute type in both numeric and enum form.
     */
    XorRelayedAddressAttribute() : StunMessageAttribute() {
        stun_attr_type = STUN_ATTR_XOR_RELAYED_ADDRESS;
        attribute_type = StunAttributeEnum::xor_relayed_address;
    }

    /**
     * @brief Construct a new XorRelayedAddress object.
     * 
     */
    XorRelayedAddressAttribute(std::uint16_t type, StunMessageHeader* header, std::uint8_t family) : StunMessageAttribute(type, header) {
        address_family = family;
    }

    /**
     * @brief Initializes the attribute.
     * 
     * Function copies the time from argument into public field.
     * 
     * @param t Time to copy.
     * @param stun_header Pointer to header of StunMessageHeader.
     * @param address Address to set.
     * @param port Port to set.
     */
    std::uint16_t initialize(StunMessageHeader* stun_header, std::uint16_t family, QHostAddress address, std::uint16_t port);

    void set_address(std::uint32_t ipv4);

    void set_address(Q_IPV6ADDR ipv6);

    std::uint32_t get_address();

    Q_IPV6ADDR get_address_6();

    QString get_address_qstring();

    void set_port(std::uint16_t port);

    std::uint16_t get_port();

    /**
     * @brief Reads the STUN XorRelayedAddress attribute from provided stream.
     * 
     * Length and type are required for base class implementation of this
     * function.
     * 
     * Reads the data of base class as well.
     * 
     * @param input Stream to read attribute from.
     * @param length Length of this attribute.
     * @param type Type of this attribute.
     * @return std::uint16_t Length read.
     */
    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t, std::uint16_t) override;

    /**
     * @brief Writes the STUN XorRelayedAddress attribute into provided stream.
     * 
     * Serialization for networking purposes.
     * Writes the data of base class as well.
     * 
     * @param output Stream to write the STUN attribute into.
     */
    virtual void write_stun_attribute(QDataStream&) override;


private:
    std::uint16_t address_family;
    std::uint32_t address_ipv4;
    std::uint16_t tcp_port;
    std::array<std::uint32_t, 4> address_ipv6;
};

/**
 * @brief Factory for XorRelayedAddress attribute.
 * 
 */
class XorRelayedAddressAttributeFactory : public StunMessageAttributeFactory {
    /**
     * @brief Creates shared pointer to new XorRelayedAddress attribute.
     * 
     * @return stun_attr_ptr New XorRelayedAddress attribute.
     */
    stun_attr_ptr create() override {
        return std::make_shared<XorRelayedAddressAttribute>();
    }
};

#endif
