#ifndef NP2PS_STUN_MESSAGES
#define NP2PS_STUN_MESSAGES

#include <memory>
#include <exception>
#include "GlobalUsing.h"
#include "CryptoUtils.hpp"

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
#define STUN_ATTR_PUBLIC_KEY 0x002B
#define STUN_ATTR_RELAYED_PUBLIC_IDENTIFIER 0x002C

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
    data = 7,
    public_key = 8,
    relayed_publid_identifier = 9
};

/**
 * @brief Class representing the STUN attribute.
 * 
 * Initializes, reads and writes the attribute.
 * 
 * Padding and unpadding options as well for those attributes, who's length
 * is not divisible by 4.
 * 
 */
class StunMessageAttribute {
public:
    virtual ~StunMessageAttribute() = default;

    StunMessageAttribute() = default;

    StunMessageAttribute(std::uint16_t type, StunMessageHeader* stun_header) {
        stun_attr_length = 0;
        stun_attr_type = type;
        header = stun_header;
    }

    /**
     * @brief Initializes attrbitue with length and pointer to its header.
     * 
     * Used when creating new attributes.
     * 
     */
    virtual void initialize(std::uint16_t len, StunMessageHeader* stun_header);

    /**
     * @brief Reads the STUN attribute from the network (through stream) using provided strean.
     * 
     * Length and type needs to be supplied, because are read earlier on by
     * another method.
     * 
     * @param input Stream to read from.
     * @param length Length of attrbiute.
     * @param type Type of attribute.
     * @return std::uint16_t Read length.
     */
    virtual std::uint16_t read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type);

    /**
     * @brief Write STUN attribute to the network (through stream).
     * 
     * @param output Stream to write into.
     */
    virtual void write_stun_attribute(QDataStream& output);

    /**
     * @brief Pad until the length is divisible by 4. That is STUN requirement.
     * 
     * @param output Stream to write padding into.
     * @param length Pre-pad length.
     */
    virtual void pad(QDataStream& output, std::uint16_t length);

    /**
     * @brief Unpad until the length matches the pre-pad length.
     * 
     * @param output Stream to read padding from.
     * @param length Pre-pad length.
     */
    virtual void unpad(QDataStream& input, std::uint16_t length);

    template<typename T>
    inline void socket_wait_for_read(QTcpSocket* device) {
        if (device) {
            while(device->bytesAvailable() < sizeof(T)) {
                device->waitForReadyRead();
            }
        }
    }

    void socket_read_to_msg_array(std::size_t length, QTcpSocket* socket, QByteArray& msg_array) {
        qint64 read_size = 0;
        msg_array.clear();
        while (read_size < length) {
            msg_array += socket->read(length - read_size + 4);
            read_size = msg_array.size();
            if (read_size < length) {
                socket->waitForReadyRead();
            }
        }
    }

    std::uint16_t stun_attr_type, stun_attr_length;
    StunAttributeEnum attribute_type;

protected:
    StunMessageHeader* header;
};

using stun_attr_ptr = std::shared_ptr<StunMessageAttribute>;

/**
 * @brief Base class for attribute factory.
 * 
 */
class StunMessageAttributeFactory {
public:
    virtual ~StunMessageAttributeFactory() = default;

    /**
     * @brief Creates new instance of given attribute on heap.
     * 
     * @return stun_attr_ptr Shared pointer to new attribute.
     */
    virtual stun_attr_ptr create() = 0;
};

using factory_map = std::map<std::uint16_t, std::shared_ptr<StunMessageAttributeFactory>>;

/**
 * @brief Class representing the header of the STUN message.
 * 
 */
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

    /**
     * @brief Decode the stun class and method from its type.
     * 
     * @param type Type to decode
     * @return true Always.
     */
    bool decode_type(std::uint16_t type);

    /**
     * @brief Encodes the message class and method into message type.
     * 
     * @return std::uint16_t Type of the message.
     */
    std::uint16_t encode_type();

    /**
     * @brief Append given attribute to the message.
     * 
     * Increases length of message accordingly.
     * 
     * @param attribute Attribute to insert.
     */
    void append_attribute(std::shared_ptr<StunMessageAttribute> attribute);

    /**
     * @brief Inserts TID, one 4B number after another.
     * 
     * Send in network order; big endian.
     * 
     * @param t0 First number.
     * @param t1 Second number.
     * @param t2 Third number.
     */
    void insert_tid(std::uint32_t t0, std::uint32_t t1, std::uint32_t t2);

    /**
     * @brief Copies TID from other message.
     * 
     * Used when creating responses.
     * 
     * @param header2 Header to copy TID from.
     */
    void copy_tid(std::shared_ptr<StunMessageHeader> header2);

    /**
     * @brief Generates new TID for the message.
     * 
     * Used when creating requests and indications.
     * 
     * @param rng Random number generator for random numbers.
     */
    void generate_tid(CryptoPP::AutoSeededRandomPool& rng);

    /**
     * @brief Read message header from the network.
     * 
     * Checks magic cookie.
     * 
     * @param input Stream to read from.
     */
    void read_message_header(QDataStream& in_stream);

    /**
     * @brief Reads attributes from the network.
     * 
     * Compares legth in message and length of read attributes if it matches.
     * 
     * @param input Stream to read from.
     * @param factories Factories to build the attributes.
     */
    void read_attributes(QDataStream& input, factory_map& factories);

    /**
     * @brief Writes entire message into network.
     * 
     * Writes entire message including the 
     * 
     * @param output Stream to write into.
     */
    void write_stun_message(QDataStream& output);

    /**
     * @brief Writes the message header into the network.
     * 
     * Only header is written.
     * 
     * @param output Stream to write into.
     */
    void write_header(QDataStream& output);

    void print_message();

    template <typename ClassTag>
    void process_method(stun_header_ptr message_orig, stun_header_ptr message_new, QTcpSocket* socket, stun_attr_type_vec& unknown_cr_attr);

    template<typename T>
    inline void socket_wait_for_read(QTcpSocket* socket) {
        if (socket) {
            while(socket->bytesAvailable() < sizeof(T)) {
                socket->waitForReadyRead();
            }
        }
    }

    
};


#endif
