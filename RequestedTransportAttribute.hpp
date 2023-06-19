#ifndef NP2PS_REQUESTED_TRANSPORT_ATTRIBUTE
#define NP2PS_REQUESTED_TRANSPORT_ATTRIBUTE

#include "StunMessages.hpp"

#define IANA_TCP 0x06
#define IANA_UDP 0x11

/**
 * @brief Class for STUN RequestedTransport attribute.
 * 
 * Attribute inherits from StunMessageAttribute class.
 */
class RequestedTransportAttribute : public StunMessageAttribute {
public:
    /**
     * @brief Construct a new RequestedTransport object.
     * 
     * Sets the attribute type in both numeric and enum form.
     */
    RequestedTransportAttribute();

    void set_protocol(std::uint32_t protocol);
    std::uint32_t get_protocol();

    /**
     * @brief Initializes the attribute.
     * 
     * Function copies the time from argument into public field.
     * 
     * @param p Protocol.
     * @param stun_header Pointer to header of StunMessageHeader.
     */
    virtual void initialize(std::uint32_t p, StunMessageHeader* stun_header);

    /**
     * @brief Reads the STUN RequestedTransport attribute from provided stream.
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
    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t length, std::uint16_t type) override;

    /**
     * @brief Writes the STUN RequestedTransport attribute into provided stream.
     * 
     * Serialization for networking purposes.
     * Writes the data of base class as well.
     * 
     * @param output Stream to write the STUN attribute into.
     */
    virtual void write_stun_attribute(QDataStream&) override;

private:
    std::uint32_t protocol_shifted; //shifted due to RFFU [RFC5766]
};

/**
 * @brief Factory for RequestedTransport attribute.
 * 
 */
class RequestedTransportAttributeFactory : public StunMessageAttributeFactory {
    /**
     * @brief Creates shared pointer to new RequestedTransport attribute.
     * 
     * @return stun_attr_ptr New RequestedTransport attribute.
     */
    stun_attr_ptr create() override {
        return std::make_shared<RequestedTransportAttribute>();
    }
};

#endif
