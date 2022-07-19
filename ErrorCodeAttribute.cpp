#include "ErrorCodeAttribute.hpp"

void ErrorCodeAttribute::set_error_code(std::uint32_t code) {
    error_code = code;
    stun_attr_length += 4;
}

void ErrorCodeAttribute::set_reason() {
    QString qs;
    switch(error_code) { //reason is set based on provided error code.
        case 300:
            qs = QString("The client should contact an alternate server for this request."); 
            break;
        case 400:
            qs = QString("The request was malformed.");
            break;
        case 401:
            qs = QString("The request did not contain the correct credentials to proceed.");
            break;
        case 420:
            qs = QString("The server received a STUN packet containing a comprehension-required attribute that it did not understand.");
            break;
        case 438:
            qs = QString("The NONCE used by client was no longer valid.");
            break;
        case 500:
            qs = QString("The server has suffered a temporary error.");
            break;
        default:
            throw unknown_comprehension_required_attribute_error("Invalid error code.");
    }
    reason = qs.toUtf8();
    stun_attr_length += reason.size();
}

std::uint16_t ErrorCodeAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    std::uint16_t read_length = StunMessageAttribute::read_stun_attribute(input, length, type);
    QByteArray in_reason(length - 4, 0);

    input >> error_code >> in_reason; //reads error code and reason

    read_length += length;

    return read_length;
}

void ErrorCodeAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    output << error_code << reason; //writes error code and reason
}