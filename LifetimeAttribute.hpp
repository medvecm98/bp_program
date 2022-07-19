#ifndef NP2PS_LIFETIME_ATTRIBUTE
#define NP2PS_LIFETIME_ATTRIBUTE

#include "StunMessages.hpp"

#define MAX_TIME 3600

/**
 * @brief Class for STUN Lifetime attribute.
 * 
 * Attribute inherits from StunMessageAttribute class.
 */
class LifetimeAttribute : public StunMessageAttribute {
public:
    /**
     * @brief Construct a new Lifetime object.
     * 
     * Sets the attribute type in both numeric and enum form.
     */
    LifetimeAttribute();

    /**
     * @brief Initializes the attribute.
     * 
     * Function copies the time from argument into public field.
     * 
     * @param t Time to copy.
     * @param h Pointer to header of StunMessageHeader.
     */
    virtual void initialize(std::uint32_t t, StunMessageHeader* h);

    /**
     * @brief Reads the STUN Lifetime attribute from provided stream.
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
    virtual std::uint16_t read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) override;

    /**
     * @brief Writes the STUN Lifetime attribute into provided stream.
     * 
     * Serialization for networking purposes.
     * Writes the data of base class as well.
     * 
     * @param output Stream to write the STUN attribute into.
     */
    virtual void write_stun_attribute(QDataStream&);

    std::uint32_t time = 0; //tiem of allocation, for future purposes
};

/**
 * @brief Factory for Lifetime attribute.
 * 
 */
class LifetimeAttributeFactory : public StunMessageAttributeFactory {

    /**
     * @brief Creates shared pointer to new Lifetime attribute.
     * 
     * @return stun_attr_ptr New Lifetime attribute.
     */
    stun_attr_ptr create() override {
        return std::make_shared<LifetimeAttribute>();
    }
};

#endif