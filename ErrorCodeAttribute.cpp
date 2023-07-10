#include "ErrorCodeAttribute.hpp"

void ErrorCodeAttribute::set_error_code(std::uint32_t code) {
    error_code = code;
    stun_attr_length += 4;
}

void ErrorCodeAttribute::set_reason() {
    QString qs;
    switch(error_code) { //reason is set based on provided error code.
        case STUN_ERR_USE_OTHER_SERVER:
            qs = QString("The client should contact an alternate server for this request."); 
            break;
        case STUN_ERR_MALFORMED:
            qs = QString("The request was malformed.");
            break;
        case STUN_ERR_INCORRECT_CREDS:
            qs = QString("The request did not contain the correct credentials to proceed.");
            break;
        case STUN_ERR_UNKNOWN_COMPREHENSION_REQ_ATTR:
            qs = QString("The server received a STUN packet containing a comprehension-required attribute that it did not understand.");
            break;
        case STUN_ERR_INVALID_NONCE:
            qs = QString("The NONCE used by client was no longer valid.");
            break;
        case STUN_ERR_TEMP_ERROR:
            qs = QString("The server has suffered a temporary error.");
            break;
        case STUN_ERR_PEER_OFFLINE:
            qs = QString("Peer offline.");
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

    QTcpSocket* socket = (QTcpSocket*) input.device();

    StunMessageAttribute::socket_wait_for_read<std::uint32_t>((QTcpSocket*)input.device());
    input >> error_code;
    StunMessageAttribute::socket_read_to_msg_array(length - 4, socket, in_reason);

    read_length += length;

    return read_length;
}

void ErrorCodeAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    output << error_code << reason; //writes error code and reason
}
