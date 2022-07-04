#ifndef NP2PS_ERROR_CODE_ATTRIBUTE
#define NP2PS_ERROR_CODE_ATTRIBUTE

#include "StunMessages.hpp"

class ErrorCodeAttribute : public StunMessageAttribute {
public:
    ErrorCodeAttribute() : StunMessageAttribute() {
        stun_attr_type = STUN_ATTR_ERROR_CODE;
        attribute_type = StunAttributeEnum::error_code;
    }

    void set_error_code(std::uint32_t code);
    void set_reason();
    void set_length();

    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t, std::uint16_t) override;

    virtual void write_stun_attribute(QDataStream&) override;

private:
    std::uint32_t error_code;
    QByteArray reason;
};

class ErrorCodeAttributeFactory : public StunMessageAttributeFactory {
    stun_attr_ptr create() override {
        return std::make_shared<ErrorCodeAttribute>();
    }
};

#endif