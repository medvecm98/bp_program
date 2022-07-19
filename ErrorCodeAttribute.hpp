#ifndef NP2PS_ERROR_CODE_ATTRIBUTE
#define NP2PS_ERROR_CODE_ATTRIBUTE

#include "StunMessages.hpp"

/**
 * @brief Class for STUN ErrorCode attribute.
 * 
 * Attribute inherits from StunMessageAttribute class.
 */
class ErrorCodeAttribute : public StunMessageAttribute {
public:
    /**
     * @brief Construct a new ErrorCode object.
     * 
     * Sets the attribute type in both numeric and enum form.
     */
    ErrorCodeAttribute() : StunMessageAttribute() {
        stun_attr_type = STUN_ATTR_ERROR_CODE;
        attribute_type = StunAttributeEnum::error_code;
    }

    /**
     * @brief Sets the error code into `error_code`.
     * 
     * @param code Error code to set.
     */
    void set_error_code(std::uint32_t code);

    /**
     * @brief Sets the reason for error.
     * 
     * Messages are not custom, but are always the same depending on error code.
     */
    void set_reason();

    /**
     * @brief Reads the STUN ErrorCode attribute from provided stream.
     * 
     * Length and type are required for base class implementation of this
     * function.
     * 
     * Reads the data of base class as well.
     * 
     * @param length Length of this attribute.
     * @param type Type of this attribute.
     * @return std::uint16_t Length read.
     */
    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t, std::uint16_t) override;

    /**
     * @brief Writes the STUN ErrorCode attribute into provided stream.
     * 
     * Serialization for networking purposes.
     * Writes the data of base class as well.
     * 
     * @param output Stream to write the STUN attribute into.
     */
    virtual void write_stun_attribute(QDataStream&) override;

private:
    std::uint32_t error_code; //error code of the error
    QByteArray reason; //why did this error happened?
};

/**
 * @brief Factory for ErrorCode attribute.
 * 
 */
class ErrorCodeAttributeFactory : public StunMessageAttributeFactory {
    /**
     * @brief Creates shared pointer to new ErrorCode attribute.
     * 
     * @return stun_attr_ptr New ErrorCode attribute.
     */
    stun_attr_ptr create() override {
        return std::make_shared<ErrorCodeAttribute>();
    }
};

#endif