#include <cstdint>

#ifndef PROGRAM_IPWRAPPER_H
#define PROGRAM_IPWRAPPER_H

#include "GlobalUsing.h"
#include <unordered_map>
#include <QtNetwork/QHostAddress>

//types for map to store keys for given user
using rsa_public_optional = std::optional< CryptoPP::RSA::PublicKey>;
using rsa_private_optional = std::optional< CryptoPP::RSA::PrivateKey>;
using eax_optional = std::optional< CryptoPP::SecByteBlock>;
using rsa_eax_pair = std::pair< rsa_public_optional, eax_optional>;
using pk_t_keys_map = std::unordered_map< pk_t, rsa_eax_pair>;

/**
 * @brief Wrapper for IPs and RSA and EAX keys.
 * 
 * Stores both IPv4 and IPv6, together with RSA and EAX keys. RSA and EAX are optional.
 */
struct IpWrapper {
	IpWrapper() = default;

	explicit IpWrapper(const QHostAddress& ip4) {
		ipv4 = ip4;
	}

	IpWrapper (const QHostAddress& ip4, const QHostAddress& ip6) {
		ipv4 = ip4;
		ipv6 = ip6;
	}

	explicit IpWrapper(const std::string& ip4) {
		ipv4 = QHostAddress(QString(ip4.c_str()));
	}

	explicit IpWrapper(const QString& ip4) {
		ipv4 = QHostAddress(ip4);
	}

	IpWrapper(const QString& ip4, const QString& ip6) {
		ipv4 = QHostAddress(ip4);
		ipv6 = QHostAddress(ip6);
	}

	void add_rsa_key(const std::string& pkey_str) {
		CryptoPP::RSA::PublicKey pub_key;
		CryptoPP::StringSource s(pkey_str, true);
		pub_key.BERDecode(s);
		key_pair.first = {std::move(pub_key)};
	}

	void add_rsa_key(CryptoPP::RSA::PublicKey&& pkey) {
		key_pair.first = rsa_public_optional(pkey);
	}

	void add_eax_key(const std::string& ekey_str) {
		key_pair.second = {CryptoPP::SecByteBlock(reinterpret_cast<const CryptoPP::byte*>(&ekey_str[0]), ekey_str.size())};
	}


	void add_eax_key(CryptoPP::SecByteBlock&& ekey) {
		key_pair.second = eax_optional(ekey);
	}

	QHostAddress ipv4;
	QHostAddress ipv6;
	rsa_eax_pair key_pair;
};

using ip_map = std::unordered_map<pk_t, IpWrapper>;


#endif //PROGRAM_IPWRAPPER_H
