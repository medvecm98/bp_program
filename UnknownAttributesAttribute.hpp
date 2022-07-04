#ifndef NP2PS_UNKNOWN_ATTRIBUTES_ATTRIBUTE
#define NP2PS_UNKNOWN_ATTRIBUTES_ATTRIBUTE

#include "StunMessages.hpp"

class UnknownAttributesAttribute : public StunMessageAttribute {
public:
    UnknownAttributesAttribute();

    void copy_attribute_vector(const stun_attr_type_vec& v) {
        unknown_attrs = stun_attr_type_vec(v);
    }

    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t, std::uint16_t) override;

    virtual void write_stun_attribute(QDataStream&) override;
private:
    stun_attr_type_vec unknown_attrs;
};

class UnknownAttributesAttributeFactory : public StunMessageAttributeFactory {
    stun_attr_ptr create() override {
        return std::make_shared<UnknownAttributesAttribute>();
    }
};

#endif 