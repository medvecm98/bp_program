#ifndef NP2PS_DATA_ATTRIBUTE
#define NP2PS_DATA_ATTRIBUTE

#include "StunMessages.hpp"
#include "Message.h"

class DataAttribute : public StunMessageAttribute {
public:
    DataAttribute();

    virtual void initialize(QByteArray& msg, StunMessageHeader* h);

    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t length, std::uint16_t type) override;

    virtual void write_stun_attribute(QDataStream&) override;

    std::string get_np2ps_messsage();

    QByteArray data;

};

class DataAttributeFactory : public StunMessageAttributeFactory {
    stun_attr_ptr create() override {
        return std::make_shared<DataAttribute>();
    }
};

#endif
