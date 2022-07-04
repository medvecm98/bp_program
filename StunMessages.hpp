#ifndef NP2PS_STUN_MESSAGES
#define NP2PS_STUN_MESSAGES

#include <memory>
#include <exception>
#include "MyExceptions.hpp"
#include "GlobalUsing.h"

#include <QObject>
#include <QApplication>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QNetworkInterface>
#include <QtCore>
#include <QtEndian>

#include <cryptopp/osrng.h>

#define STUN_IPV4 0x0001
#define STUN_IPV6 0x0002

#define STUN_PORT 3478

#define FAMILY_SELECT(family, ipv4, ipv6) if (family == STUN_IPV4) {ipv4} else if (family == STUN_IPV6) {ipv6}

#define STUN_ATTR_ERROR_CODE 0x0009
#define STUN_ATTR_UNKNOWN_ATTRIBUTES 0x000A
#define STUN_ATTR_XOR_MAPPED_ADDRESS 0x0020
#define STUN_ATTR_XOR_RELAYED_ADDRESS 0x0016
#define STUN_ATTR_DATA 0x0013
#define STUN_ATTR_LIFETIME 0x000D
#define STUN_ATTR_PUBLIC_IDENTIFIER 0x001F
#define STUN_ATTR_REQUESTED_TRANSPORT 0x0019

#define STUN_METHOD_BINDING 0x001
#define STUN_METHOD_ALLOCATE 0X003
#define STUN_METHOD_SEND 0x006
#define STUN_METHOD_IDENTIFY 0x00D

#define STUN_CLASS_REQUEST 0b00
#define STUN_CLASS_INDICATION 0b01
#define STUN_CLASS_RESPONSE_SUCCESS 0b10
#define STUN_CLASS_RESPONSE_ERROR 0b11

constexpr std::uint32_t MAGIC_COOKIE_VALUE = 0x2112A442;

using stun_attr_type_vec = std::vector<quint16>;

class StunMessageHeader;
using stun_header_ptr = std::shared_ptr<StunMessageHeader>;

enum class StunClassEnum {
    request = 0,
    indication = 1,
    response_success = 2,
    response_error = 3
};

enum class StunMethodEnum {
    binding = 1,
    allocate = 2,
    identify = 3,
    send = 4
};

enum class StunAttributeEnum {
    xor_mapped_address = 0,
    error_code = 1,
    unknown_attributes = 2,
    requested_transport = 3,
    lifetime = 4,
    xor_relayed_address = 5,
    public_identifier = 6,
    data = 7
};

class StunMessageAttribute {
public:
    virtual ~StunMessageAttribute() = default;

    StunMessageAttribute() = default;

    StunMessageAttribute(std::uint16_t type, StunMessageHeader* h) {
        stun_attr_length = 0;
        stun_attr_type = type;
        header = h;
    }

    virtual void initialize(std::uint16_t len, StunMessageHeader* h);

    virtual std::uint16_t read_stun_attribute(QDataStream&, std::uint16_t length, std::uint16_t type);

    virtual void write_stun_attribute(QDataStream&);

    std::uint16_t stun_attr_type, stun_attr_length;
    StunAttributeEnum attribute_type;

protected:
    StunMessageHeader* header;
};

using stun_attr_ptr = std::shared_ptr<StunMessageAttribute>;

class StunMessageAttributeFactory {
public:
    virtual ~StunMessageAttributeFactory() = default;

    virtual stun_attr_ptr create() = 0;
};

using factory_map = std::map<std::uint16_t, std::shared_ptr<StunMessageAttributeFactory>>;

class StunMessageHeader {
public:
    std::uint16_t stun_length;
    StunClassEnum stun_class;
    StunMethodEnum stun_method;
    std::array<std::uint32_t, 3> stun_transaction_id;
    
    std::vector<std::shared_ptr<StunMessageAttribute>> attributes;

    StunMessageHeader() = default;
    explicit StunMessageHeader(std::uint16_t length);
    StunMessageHeader(StunClassEnum stun_class, StunMethodEnum stun_method);
    bool decode_type(std::uint16_t type);
    std::uint16_t encode_type();
    void append_attribute(std::shared_ptr<StunMessageAttribute> attribute);
    void insert_tid(std::uint32_t t0, std::uint32_t t1, std::uint32_t t2);
    void copy_tid(std::shared_ptr<StunMessageHeader> header2);
    void generate_tid(CryptoPP::AutoSeededRandomPool& rng);

    void read_message_header(QDataStream&);
    void read_attributes(QDataStream&, factory_map&);

    void write_stun_message(QDataStream&);
    void write_header(QDataStream&);

    void print_message();

    template <typename ClassTag>
    void process_method(stun_header_ptr message_orig, stun_header_ptr message_new, QTcpSocket* socket, stun_attr_type_vec& unknown_cr_attr);
};


#endif