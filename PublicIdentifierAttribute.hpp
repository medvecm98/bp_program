#ifndef NP2PS_PUBLIC_IDENTIFIER_ATTRIBUTE
#define NP2PS_PUBLIC_IDENTIFIER_ATTRIBUTE

#include "StunMessages.hpp"
#include "GlobalUsing.h"

/**
 * @brief Class for STUN PublicIdentifier attribute.
 * 
 * This is custom, non-standard attribute.
 * 
 * Attribute inherits from StunMessageAttribute class.
 */
class PublicIdentifierAttribute : public StunMessageAttribute {
public:
    /**
     * @brief Construct a new PublicIdentifier object.
     * 
     * Sets the attribute type in both numeric and enum form.
     */
    PublicIdentifierAttribute();

    /**
     * @brief Initializes the attribute.
     * 
     * Function copies the time from argument into public field.
     * 
     * @param public_identifier Public identifier to copy.
     * @param h Pointer to header of StunMessageHeader.
     */
    virtual void initialize(pk_t public_identifier, StunMessageHeader* h);

    /**
     * @brief Reads the STUN PublicIdentifier attribute from provided stream.
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
     * @brief Writes the STUN PublicIdentifier attribute into provided stream.
     * 
     * Serialization for networking purposes.
     * Writes the data of base class as well.
     * 
     * @param output Stream to write the STUN attribute into.
     */
    virtual void write_stun_attribute(QDataStream& output) override;

    void set_public_identifier(pk_t pid);

    pk_t get_public_identifier();

    quint64 public_identifier;
};

/**
 * @brief Factory for PublicIdenitifier attribute.
 * 
 */
class PublicIdentifierAttributeFactory : public StunMessageAttributeFactory {
    /**
     * @brief Creates shared pointer to new PublicIdenitifier attribute.
     * 
     * @return stun_attr_ptr New PublicIdenitifier attribute.
     */
    stun_attr_ptr create() override {
        return std::make_shared<PublicIdentifierAttribute>();
    }
};

#endif