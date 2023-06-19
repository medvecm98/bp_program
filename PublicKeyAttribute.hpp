#ifndef NP2PS_PUBLIC_KEY_ATTRIBUTE
#define NP2PS_PUBLIC_KEY_ATTRIBUTE

#include "StunMessages.hpp"

/**
 * @brief Class for STUN PublicKey attribute.
 * 
 * Attribute inherits from StunMessageAttribute class.
 */
class PublicKeyAttribute : public StunMessageAttribute {
public:
    /**
     * @brief Construct a new PublicKey object.
     * 
     * Sets the attribute type in both numeric and enum form.
     */
    PublicKeyAttribute();

    /**
     * @brief Initializes the attribute.
     * 
     * Function copies the time from argument into public field.
     * 
     * @param public_key Public key to save.
     * @param stun_header Pointer to header of StunMessageHeader.
     */
    virtual void initialize(const CryptoPP::RSA::PublicKey& public_key, StunMessageHeader* stun_header);

    /**
     * @brief Reads the STUN PublicKey attribute from provided stream.
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
     * @brief Writes the STUN PublicKey attribute into provided stream.
     * 
     * Serialization for networking purposes.
     * Writes the data of base class as well.
     * 
     * @param output Stream to write the STUN attribute into.
     */
    virtual void write_stun_attribute(QDataStream&) override;

    void set_value(const CryptoPP::RSA::PublicKey& public_key);

    CryptoPP::RSA::PublicKey get_value();

    /**
     * @brief Gets the size of value wrapper, std::string.
     * 
     * @return std::uint16_t size
     */
    std::uint16_t get_value_size();

private:
    std::string value;
};

/**
 * @brief Factory for PublicKey attribute.
 * 
 */
class PublicKeyAttributeFactory : public StunMessageAttributeFactory {
    /**
     * @brief Creates shared pointer to new PublicKey attribute.
     * 
     * @return stun_attr_ptr New PublicKey attribute.
     */
    stun_attr_ptr create() override {
        return std::make_shared<PublicKeyAttribute>();
    }
};

#endif
