#ifndef NP2PS_DATA_ATTRIBUTE
#define NP2PS_DATA_ATTRIBUTE

#include "StunMessages.hpp"
#include "Message.h"

/**
 * @brief Class for STUN Data attribute.
 * 
 * This is a custom, non-standard attribute.
 * 
 * Attribute inherits from StunMessageAttribute class.
 */
class DataAttribute : public StunMessageAttribute {
public:
    /**
     * @brief Construct a new Data object.
     * 
     * Sets the attribute type in both numeric and enum form.
     */
    DataAttribute();

    /**
     * @brief Initializes the attribute.
     * 
     * Function copies the data from argument into public field.
     * 
     * @param msg Data to copy.
     * @param stun_header Pointer to header of StunMessageHeader.
     */
    virtual void initialize(QByteArray& msg, StunMessageHeader* stun_header);

    /**
     * @brief Reads the STUN Data attribute from provided stream.
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
     * @brief Writes the STUN Data attribute into provided stream.
     * 
     * Serialization for networking purposes.
     * Writes the data of base class as well.
     * 
     * @param output Stream to write the STUN attribute into.
     */
    virtual void write_stun_attribute(QDataStream& output) override;

    /**
     * @brief Get the np2ps messsage in std::string form.
     */
    std::string get_np2ps_messsage();

    QByteArray data; //message in raw bytes

};

/**
 * @brief Factory for Data attribute.
 * 
 */
class DataAttributeFactory : public StunMessageAttributeFactory {
    /**
     * @brief Creates shared pointer to new Data attribute.
     * 
     * @return stun_attr_ptr New Data attribute.
     */
    stun_attr_ptr create() override {
        return std::make_shared<DataAttribute>();
    }
};

#endif
