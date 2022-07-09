#include <cstdint>

#ifndef PROGRAM_IPWRAPPER_H
#define PROGRAM_IPWRAPPER_H

#include "GlobalUsing.h"
#include <unordered_map>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>

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

	explicit IpWrapper(const QHostAddress& ip4, std::uint16_t port = PORT) {
		ipv4 = ip4;
		this->port = port;
	}

	IpWrapper (const QHostAddress& ip4, const QHostAddress& ip6, std::uint16_t port = PORT) {
		ipv4 = ip4;
		ipv6 = ip6;
		this->port = port;
	}

	explicit IpWrapper(const std::string& ip4, std::uint16_t port = PORT) {
		ipv4 = QHostAddress(QString(ip4.c_str()));
		this->port = port;
	}

	explicit IpWrapper(const QString& ip4, std::uint16_t port = PORT) {
		ipv4 = QHostAddress(ip4);
		this->port = port;
	}

	IpWrapper(const QString& ip4, const QString& ip6, std::uint16_t port = PORT) {
		ipv4 = QHostAddress(ip4);
		ipv6 = QHostAddress(ip6);
		this->port = port;
	}

	explicit IpWrapper(pk_t server) {
		ipv4 = QHostAddress();
		ipv6 = QHostAddress();
		this->port = 0;
		preferred_stun_server = server;
	}

	explicit IpWrapper(const CryptoPP::RSA::PublicKey& pk) {
		ipv4 = QHostAddress();
		ipv6 = QHostAddress();
		this->port = 0;
		key_pair.first = {pk};
	}

	void add_rsa_key(const std::string& pkey_str) {
		CryptoPP::RSA::PublicKey pub_key;
		CryptoPP::StringSource s(pkey_str, true);
		pub_key.BERDecode(s);
		key_pair.first = {std::move(pub_key)};
	}

	void add_rsa_key(const CryptoPP::RSA::PublicKey& pkey) {
		key_pair.first = rsa_public_optional(pkey);
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

	void copy_tcp_socket(QTcpSocket* socket) {
		tcp_socket_ = socket;
	}

	QTcpSocket* get_tcp_socket() {
		return tcp_socket_;
	}

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void save(Archive& ar, const unsigned int version) const {
		if (ipv4.isNull()) {
			ar & ipv4.toString().toStdString();
		}
		else {
			ar & "";
		}

		if (ipv6.isNull()) {
			ar & ipv6.toString().toStdString();
		}
		else {
			ar & "";
		}
		
		if (key_pair.first.has_value()) {
			std::string s;
			CryptoPP::StringSink ss(s);
			key_pair.first.value().DEREncode(ss);
			ar & s;
		}
		else {
			ar & "";
		}

		if (key_pair.second.has_value()) {
			//EAX
			std::string s(reinterpret_cast<const char*>(&key_pair.second.value()[0]), key_pair.second.value().size());
			ar & s;
		}
		else {
			ar & "";
		}
	}

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void load(Archive& ar, const unsigned int version) {
		std::string s;
		ar & s;
		if (!s.empty()) {
			ipv4 = QHostAddress(s.c_str());
		}
		ar & s;
		if (!s.empty()) {
			ipv6 = QHostAddress(s.c_str());
		}
		ar & s;
		if (!s.empty()) {
			key_pair.first = {CryptoPP::RSA::PublicKey()};
			CryptoPP::StringSource ss(s, true);
			key_pair.first.value().BERDecode(ss);
		}
		else {
			key_pair.first = {};
		}
		ar & s;
		if (!s.empty()) {
			key_pair.second = {CryptoPP::SecByteBlock(reinterpret_cast<const CryptoPP::byte*>(&s[0]), s.size())};
		}
		else {
			key_pair.second = {};
		}
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()

	QHostAddress get_stun_address() {
		return stun_address;
	}

	std::uint16_t get_stun_port() {
		return stun_port;
	}

	//for normal traversal
	QHostAddress ipv4;
	QHostAddress ipv6;
	std::uint16_t port;

	//for STUN traversal
	QHostAddress stun_address;
	std::uint16_t stun_port;

	//for TURN traversal
	QTcpSocket* tcp_socket_ = NULL; //for STUN servers.
	QTcpSocket* np2ps_tcp_socket_ = NULL;
	pk_t preferred_stun_server = 0;

	rsa_eax_pair key_pair;
	

};

using ip_map = std::unordered_map<pk_t, IpWrapper>;


#endif //PROGRAM_IPWRAPPER_H
