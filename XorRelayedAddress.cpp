#include "XorRelayedAddress.hpp"

/**
 * @brief Initialize a new 
 * 
 * @param header 
 * @param family 
 * @param socket 
 * @return std::uint16_t 
 */
std::uint16_t XorRelayedAddressAttribute::initialize(StunMessageHeader* header, std::uint16_t family, QHostAddress a, std::uint16_t p) {
    std::uint16_t type = 0x0020; //STUN: XOR-MAPPED-ADDRESS
    std::uint16_t length = 0;

    std::uint32_t address;
    Q_IPV6ADDR address_6;

    std::uint16_t port = p;
    address_family = family;

    FAMILY_SELECT(
        family,
        address = (std::uint32_t) a.toIPv4Address();,
        address_6 = a.toIPv6Address();
    );

    FAMILY_SELECT(
        family,
        length += 4 + 4;,
        length += 4 * 4 + 4;
    );

    FAMILY_SELECT(
        family,
        set_address(address);,
        set_address(address_6);
    );
    
    set_port(port);

    StunMessageAttribute::initialize(length, header);

    return length;
}

/**
 * @brief For reading XorRelayedAddressAttribute from network stream.
 * 
 * To be used for reading from network only. For creation use factory and XorRelayedAddressAttribute::initialize().
 * Method will read and write fields exclusive for XorRelayedAddressAttribute only.
 * 
 * @param input Network stream to read from.
 * @return std::uint16_t Number of read bytes.
 */
std::uint16_t XorRelayedAddressAttribute::read_stun_attribute(QDataStream& input, std::uint16_t length, std::uint16_t type) {
    std::uint16_t read_length = StunMessageAttribute::read_stun_attribute(input, length, type);

    input >> address_family >> tcp_port;


    read_length += 4;

    FAMILY_SELECT(
        address_family,
        input >> address_ipv4; 
        read_length += 4;,
        input >> address_ipv6[3] >> address_ipv6[2] >> address_ipv6[1] >> address_ipv6[0];
        read_length += 4 + 4 + 4 + 4;
    );

    return read_length;
}

void XorRelayedAddressAttribute::set_address(Q_IPV6ADDR ipv6) {
    std::uint32_t four_bytes = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            address_ipv6[i] |= ipv6.c[j + (i * 4)];
            address_ipv6[i] = address_ipv6[i] << 8;
        }
    }
    address_ipv6[3] ^= MAGIC_COOKIE_VALUE;
    address_ipv6[2] ^= header->stun_transaction_id[2];
    address_ipv6[1] ^= header->stun_transaction_id[1];
    address_ipv6[0] ^= header->stun_transaction_id[0];
}

void XorRelayedAddressAttribute::set_address(std::uint32_t ipv4) {
    address_ipv4 = ipv4 ^ MAGIC_COOKIE_VALUE;
}

void XorRelayedAddressAttribute::set_port(std::uint16_t port) {
    tcp_port = port ^ (MAGIC_COOKIE_VALUE >> 16);
}

std::uint16_t XorRelayedAddressAttribute::get_port() {
    return (tcp_port ^ (MAGIC_COOKIE_VALUE >> 16));
}


QString XorRelayedAddressAttribute::get_address_qstring() {
    FAMILY_SELECT(
        address_family,
        return QString().setNum(get_address());,
        return QString(); //TODO: Happy eyeballs
    )
    throw invalid_address_family("Neither IPv4 nor IPv6 was used.");
}

void XorRelayedAddressAttribute::write_stun_attribute(QDataStream& output) {
    StunMessageAttribute::write_stun_attribute(output);
    output << address_family << tcp_port;


    FAMILY_SELECT(
        address_family,
        output << address_ipv4;,
        output << address_ipv6[3] << address_ipv6 [2] << address_ipv6[1] << address_ipv6[0];
    )
}

std::uint32_t XorRelayedAddressAttribute::get_address() {
    auto i4 = address_ipv4;
    return i4 ^ MAGIC_COOKIE_VALUE;
}

Q_IPV6ADDR XorRelayedAddressAttribute::get_address_6() {
    throw unsupported_feature("IPv6 for XorRelayedAddressAttribute is unsupported.");
}
