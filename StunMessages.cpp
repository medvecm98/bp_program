#include "StunMessages.hpp"

StunMessageHeader::StunMessageHeader(std::uint16_t length) {
    stun_length = length;
}

void StunMessageHeader::append_attribute(std::shared_ptr<StunMessageAttribute> attribute) {
    attributes.push_back(attribute);
    stun_length += attribute->stun_attr_length + 4;
}

void StunMessageHeader::insert_tid(std::uint32_t t0, std::uint32_t t1, std::uint32_t t2) {
    stun_transaction_id[0] = t0;
    stun_transaction_id[1] = t1;
    stun_transaction_id[2] = t2;
}

void StunMessageHeader::copy_tid(std::shared_ptr<StunMessageHeader> header2) {
    stun_transaction_id[0] = header2->stun_transaction_id[0];
    stun_transaction_id[1] = header2->stun_transaction_id[1];
    stun_transaction_id[2] = header2->stun_transaction_id[2];
}

void StunMessageHeader::write_stun_message(QDataStream& output) {
    write_header(output);
    for (auto&& attr : attributes) {
        attr->write_stun_attribute(output);
    }
}

void StunMessageHeader::write_header(QDataStream& output) {
    output << encode_type() << stun_length << MAGIC_COOKIE_VALUE;
    output << stun_transaction_id[2] << stun_transaction_id[1] << stun_transaction_id[0];
}

template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

void StunMessageHeader::print_message() {
    std::cout <<
    "STUN length : " << stun_length <<
    "\nSTUN class : " << as_integer(stun_class) <<
    "\nSTUN method : " << as_integer(stun_method) << std::endl;
}

StunMethodEnum get_method_enum_from_int(std::uint16_t m) {
    switch (m)
    {
    case STUN_METHOD_BINDING:
        return StunMethodEnum::binding;
    case STUN_METHOD_ALLOCATE:
        return StunMethodEnum::allocate;
    case STUN_METHOD_IDENTIFY:
        return StunMethodEnum::identify;
    case STUN_METHOD_SEND:
        return StunMethodEnum::send;
    default:
        throw std::invalid_argument("Such method is not supported.");
    }
}

std::uint16_t get_int_from_method_enum(StunMethodEnum e) {
    switch (e)
    {
    case StunMethodEnum::binding:
        return STUN_METHOD_BINDING;
    case StunMethodEnum::allocate:
        return STUN_METHOD_ALLOCATE;
    case StunMethodEnum::identify:
        return STUN_METHOD_IDENTIFY;
    case StunMethodEnum::send:
        return STUN_METHOD_SEND;

    default:
        throw std::invalid_argument("Invalid method enum option.");
    }
}

StunClassEnum get_class_enum_from_int(std::uint16_t c) {    
    switch (c)
    {
    case STUN_CLASS_REQUEST:
        return StunClassEnum::request;
    case STUN_CLASS_INDICATION:
        return StunClassEnum::indication;
    case STUN_CLASS_RESPONSE_SUCCESS:
        return StunClassEnum::response_success;
    case STUN_CLASS_RESPONSE_ERROR:
        return StunClassEnum::response_error;    
    default:
        throw std::invalid_argument("Such class is not supported.");
    }
}

std::uint16_t get_int_from_class_enum(StunClassEnum e) {
    switch (e)
    {
    case StunClassEnum::request:
        return STUN_CLASS_REQUEST;
    case StunClassEnum::indication:
        return STUN_CLASS_INDICATION;
    case StunClassEnum::response_success:
        return STUN_CLASS_RESPONSE_SUCCESS;
    case StunClassEnum::response_error:
        return STUN_CLASS_RESPONSE_ERROR;
    
    default:
        throw std::invalid_argument("Invalid class enum option.");
    }
}

/**
 * @brief Decompose STUN message type into message method and class.
 * 
 * Return value checks, that header starts with two zero bits. This is not
 * part of the "Message Type" by STUN definition [RFC8489], but was included
 * here, to ease the decoding.
 * 
 * @param type Type to decompose.
 * @return true Type starts with 00.
 * @return false Type doesn't start with 00. 
 */
bool StunMessageHeader::decode_type(std::uint16_t type) {
    std::uint16_t two_zeroes, m, c;
    two_zeroes = type & 0b1100'0000'0000'0000;

    if (two_zeroes != 0)
        throw invalid_stun_message_format_error("First two bits aren't non-zero.");

    //             0           1
    //             0123'4567'8901'2345
    m =   type & 0b0000'0000'0000'1111;
    m = ((type & 0b0000'0000'1110'0000) >> 1) | m;
    m = ((type & 0b0011'1110'0000'0000) >> 2) | m;

    //             0           1
    //             0123'4567'8901'2345
    c =  (type & 0b0000'0000'0001'0000) >> 4;
    c = ((type & 0b0000'0001'0000'0000) >> 7) | c;

    stun_method = get_method_enum_from_int(m);
    stun_class = get_class_enum_from_int(c);

    //std::cout << "SMH: decode_type, method: " << m << ", class: " << c << std::endl; 

    return true;
}

std::uint16_t StunMessageHeader::encode_type() {
    std::uint16_t m = 0, c = 0;
    std::uint16_t sm = get_int_from_method_enum(stun_method), sc = get_int_from_class_enum(stun_class);

    //           0           1                       0           1
    //           0123'4567'8901'2345                 0123'4567'8901'2345
    m =  (sm & 0b0000'1111'1000'0000) << 2;        //0011'1110'0000'0000
    m = ((sm & 0b0000'0000'0111'0000) << 1) | m;   //0011'1110'1110'0000
    m =  (sm & 0b0000'0000'0000'1111) | m;         //0011'1110'1110'1111

    c =  (sc & 0b0000'0000'0000'0010) << 7;        //0000'0001'0000'0000
    c = ((sc & 0b0000'0000'0000'0001) << 4) | c;   //0000'0001'0001'0000

    return m | c;
}

/**
 * @brief Reads STUN message header encoded in STUN message.
 * 
 * @param in_stream QDataStream to read from.
 * @return stun_header_ptr Decoded STUN message header.
 */
void StunMessageHeader::read_message_header(QDataStream& in_stream) {
    quint16 stun_header_type = 0;
    quint16 stun_header_length = 0;
    quint32 stun_header_magic_cookie = 0;

    in_stream >> stun_header_type >> stun_header_length >> stun_header_magic_cookie;

    std::cout << "Cookie in read_message_header: " << stun_header_magic_cookie << std::endl;
    std::cout << "Type in read_message_header: " << stun_header_type << std::endl;
    std::cout << "Length in read_message_header: " << stun_header_length << std::endl;

    this->stun_length = stun_header_length;

    if (stun_header_magic_cookie != MAGIC_COOKIE_VALUE)
        throw invalid_stun_message_format_error("Magic cookie value is invalid.");

    quint32 stun_header_tid_0 = 0, stun_header_tid_1 = 0, stun_header_tid_2 = 0;

    in_stream >> stun_header_tid_2 >> stun_header_tid_1 >> stun_header_tid_0; 

    decode_type((std::uint16_t)stun_header_type);
    insert_tid((std::uint32_t)stun_header_tid_0, 
        (std::uint32_t)stun_header_tid_1, (std::uint32_t)stun_header_tid_2);
}

/**
 * @brief Reads STUN attributes encoded in STUN message.
 * 
 * @param in_stream QDataStream to read from.
 * @param stun_attribute_factories Attribute factories used to create new attributes.
 */
void StunMessageHeader::read_attributes(QDataStream& in_stream, factory_map& stun_attribute_factories) {
    std::int32_t stun_message_remaining_length = stun_length;
    quint16 attr_type, attr_length;
    std::shared_ptr<StunMessageAttribute> stun_attribute;
    const std::uint16_t type_and_length_length = 4;

    while (stun_message_remaining_length > 0) { //load all attributes
        in_stream >> attr_type >> attr_length;

        std::cout << "SM: RA: type: " << attr_type << ", length: " << attr_length << std::endl;
        std::cout << "SM: RA: stun_length: " << stun_length << std::endl;

        stun_message_remaining_length -= type_and_length_length;

        stun_attribute = stun_attribute_factories[(std::uint16_t)attr_type]->create();
        std::uint16_t actual_attr_length_read = stun_attribute->read_stun_attribute(in_stream, attr_length, attr_type);

        if (actual_attr_length_read != attr_length) {
            std::string s = "Lengths expected and actual differ: " + std::to_string(actual_attr_length_read - attr_length);
            throw invalid_stun_message_format_error(s);
        }

        stun_message_remaining_length -= actual_attr_length_read;

        stun_attribute->stun_attr_length = attr_length;

        append_attribute(stun_attribute);
        std::cout << attributes.size() << std::endl;
    }

    if (stun_message_remaining_length < 0)
        throw invalid_stun_message_format_error("Negative length.");
}

void StunMessageHeader::generate_tid(CryptoPP::AutoSeededRandomPool& rng) {
    for (std::size_t i = 0; i < 3; i++) { //96-bits in tid
            stun_transaction_id[i] = 0;
            for (std::size_t j = 0; j < 4; j++) { //32-bits in one array entry
                stun_transaction_id[i] <<= 8;
                stun_transaction_id[i] |= rng.GenerateByte();
            }
        }
}

void StunMessageAttribute::initialize(std::uint16_t len, StunMessageHeader* h) {
    stun_attr_length = len;
    header = h;
}



std::uint16_t StunMessageAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    std::uint16_t read_length = 0;

    //input >> stun_attr_type >> stun_attr_length;
    stun_attr_type = type;
    stun_attr_length = length;

    std::cout << "StunMessageAttribute::read_stun_attribute: " << stun_attr_type << ' ' << stun_attr_length << std::endl;

    read_length = 2 + 2;

    return 0;
}


void StunMessageAttribute::write_stun_attribute(QDataStream& output) {
    output << stun_attr_type << stun_attr_length;
}

void StunMessageAttribute::pad(QDataStream& output, std::uint16_t length) {
    quint8 zero_byte = 0;
    while ((length % 4) != 0) {
        output << zero_byte;
        length++;
    }
}

void StunMessageAttribute::unpad(QDataStream& input, std::uint16_t length) {
    quint8 zero_byte = 0;
    while ((length % 4) != 0) {
        input >> zero_byte;
        length++;
    }
}