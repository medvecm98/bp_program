#include <cstdint>

#ifndef PROGRAM_IPWRAPPER_H
#define PROGRAM_IPWRAPPER_H

#include "GlobalUsing.h"
#include <unordered_map>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <cryptopp/filters.h>

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

	explicit IpWrapper(const np2ps::IpWrapper& serialized_wrapper) : 
		ipv4(QHostAddress(serialized_wrapper.ipv4())),
		port(serialized_wrapper.port()),
		relay_flag(serialized_wrapper.relay_flag())
	{
		if (serialized_wrapper.has_rsa_public_key()) { //deserialize RSA public
			using namespace CryptoPP;
			std::string encoded_key = serialized_wrapper.rsa_public_key();
			std::string decoded_key;
			Base64Decoder decoder;
			decoder.Put( (byte*) encoded_key.data(), encoded_key.size());
			decoder.MessageEnd();

			word64 size = decoder.MaxRetrievable();
			if (size && size <= SIZE_MAX) {
				decoded_key.resize(size);
				decoder.Get((byte*)&decoded_key[0], decoded_key.size());
			}

			ByteQueue bq;

			StringSource ss(decoded_key, true);
			ss.TransferTo(bq);
			RSA::PublicKey pk;
			pk.Load(bq);
			key_pair.first = {pk};
		}

		if (serialized_wrapper.has_eax_key()) { //deserialize EAX
			using namespace CryptoPP;
			std::string encoded = serialized_wrapper.eax_key();
			std::string decoded;
			Base64Decoder decoder(new StringSink(decoded));
			decoder.Put(reinterpret_cast<const byte*>(&encoded[0]), encoded.size());
			decoder.MessageEnd();

			key_pair.second = {SecByteBlock(reinterpret_cast<const byte*>(&decoded[0]), decoded.size())};
		}
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

	QHostAddress get_stun_address() {
		return stun_address;
	}

	std::uint16_t get_stun_port() {
		return stun_port;
	}

	void set_relay_flag() {
		std::cout << "SETTING RELAY FLAG" << std::endl;
		relay_flag = true;
	}

	bool get_relay_flag() {
		return relay_flag;
	}

	void reset_relay_flag() {
		relay_flag = false;
	}

	bool has_rsa() {
		return key_pair.first.has_value();
	}

	bool has_eax() {
		return key_pair.second.has_value();
	}

	bool has_ipv4() {
		return !ipv4.isNull();
	}

	bool has_stun_ipv4() {
		return !stun_address.isNull();
	}

	CryptoPP::RSA::PublicKey& get_rsa() {
		if (has_rsa()) {
			return key_pair.first.value();
		}
		throw no_rsa_key_found("RSA key was not found.");
	}

	CryptoPP::SecByteBlock& get_eax() {
		if (has_eax()) {
			return key_pair.second.value();
		}
		throw no_eax_key_found("EAX key was not found.");
	}

	void serialize_wrapper(np2ps::IpWrapper* wrapper) {
		wrapper->set_ipv4(ipv4.toIPv4Address());
		wrapper->set_port(port);

		if (key_pair.second.has_value()) {
			std::string shared_key_b64;
			CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(shared_key_b64));
			CryptoPP::SecByteBlock& shared_key = key_pair.second.value();

			encoder.Put(shared_key, shared_key.size());
			encoder.MessageEnd();
			wrapper->set_eax_key(shared_key_b64);
		}

		if (key_pair.first.has_value()) {
			std::string public_key_b64;
			CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(public_key_b64));
			CryptoPP::RSA::PublicKey& public_key = key_pair.first.value();

			CryptoPP::ByteQueue queue;
			public_key.Save(queue);
			queue.CopyTo(encoder);
			encoder.MessageEnd();
			wrapper->set_rsa_public_key(public_key_b64);
		}

		wrapper->set_relay_flag(relay_flag);
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
	TimePoint time_since_last_disconnect; 

	rsa_eax_pair key_pair;	
	
	bool relay_flag = false;
};

using ip_map = std::unordered_map<pk_t, IpWrapper>;


#endif //PROGRAM_IPWRAPPER_H
