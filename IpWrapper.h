#include <cstdint>

#ifndef PROGRAM_IPWRAPPER_H
#define PROGRAM_IPWRAPPER_H

#include "CryptoUtils.hpp"
#include <unordered_map>
#include <queue>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QTcpSocket>
#include <cryptopp/filters.h>

enum class RelayState {
	Direct,
	Relayed,
	Unknown
};

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

	explicit IpWrapper(const CryptoPP::RSA::PublicKey& public_id) {
		ipv4 = QHostAddress();
		ipv6 = QHostAddress();
		this->port = 0;
		key_pair.first = {public_id};
	}

	explicit IpWrapper(const np2ps::IpWrapper& serialized_wrapper) : 
		ipv4(QHostAddress(serialized_wrapper.ipv4())),
		port(serialized_wrapper.port())
	{
		if (serialized_wrapper.has_rsa_public_key()) { //deserialize RSA public
			using namespace CryptoPP;
			std::string encoded_key = serialized_wrapper.rsa_public_key();
			set_rsa_hex_string(encoded_key);
		}

		if (serialized_wrapper.has_eax_key()) { //deserialize EAX
			using namespace CryptoPP;
			std::string encoded_key = serialized_wrapper.eax_key();
			set_eax_hex_string(encoded_key);
		}

		switch (serialized_wrapper.relay_state()) {
			case np2ps::RelayState::Direct:
				relay_state = RelayState::Direct;
				break;
			case np2ps::RelayState::Relayed:
				relay_state = RelayState::Relayed;
				break;
			case np2ps::RelayState::Unknown:
				relay_state = RelayState::Unknown;
				break;
			default:
				throw other_error("Unknown relay state deserialized.");
		}

		for (auto&& serialized_relay_by : serialized_wrapper.relay_by()) {
			relay_by.push(serialized_relay_by);
		}

		for (auto&& serialized_relay_to : serialized_wrapper.relay_to()) {
			relay_to.emplace(serialized_relay_to);
		}
	}

	void add_rsa_key(const std::string& pkey_str) {
		CryptoPP::RSA::PublicKey pub_key = CryptoUtils::instance().hex_to_rsa(pkey_str);
		key_pair.first = {std::move(pub_key)};
	}

	void add_rsa_key(const CryptoPP::RSA::PublicKey& pkey) {
		key_pair.first = rsa_public_optional(pkey);
	}

	void add_rsa_key(CryptoPP::RSA::PublicKey&& pkey) {
		key_pair.first = rsa_public_optional(pkey);
	}

	void add_eax_key(const std::string& ekey_str) {
		using namespace CryptoPP;
		key_pair.second = {ByteQueue()};
		ByteQueue& queue = key_pair.second.value();
		StringSource source(ekey_str, true);
		source.TransferTo(queue);
	}

	void add_eax_key(CryptoPP::ByteQueue&& ekey) {
		using namespace CryptoPP;
		key_pair.second = {ekey};
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

	void set_relay_state(bool relay) {
		std::cout << "SETTING RELAY FLAG " << std::flush;
		if (relay) {
			std::cout << "Relayed" << std::endl;
			relay_state = RelayState::Relayed;
		}
		else {
			std::cout << "Direct" << std::endl;
			relay_state = RelayState::Direct;
		}
	}

	void set_relay_state(RelayState relay_state_) {
		relay_state = relay_state_;
	}

	RelayState get_relay_state() {
		return relay_state;
	}

	bool get_relay_state_bool() {
		return relay_state == RelayState::Relayed;
	}

	void reset_relay_state() {
		relay_state = RelayState::Unknown;
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

	bool has_np2ps_socket() {
		return !np2ps_tcp_socket_;
	}

	CryptoPP::RSA::PublicKey& get_rsa() {
		if (has_rsa()) {
			return key_pair.first.value();
		}
		throw no_rsa_key_found("RSA key was not found.");
	}

	rsa_public_optional get_rsa_optional() {
		if (has_rsa()) {
			return key_pair.first;
		}
		throw no_rsa_key_found("RSA key was not found. (optional)");
	}

	std::string get_rsa_hex_string() {
		return CryptoUtils::instance().rsa_to_hex(get_rsa());
	}

	CryptoPP::RSA::PublicKey& set_rsa_hex_string(const std::string& input) {
		key_pair.first = {CryptoUtils::instance().hex_to_rsa(input)};
		return key_pair.first.value();
	}

	std::string get_eax_hex_string() {
		return CryptoUtils::instance().bq_to_hex(get_eax());
	}

	CryptoPP::ByteQueue& set_eax_hex_string(const std::string& input) {
		using namespace CryptoPP;

		key_pair.second = std::optional<ByteQueue>(
			CryptoUtils::instance().hex_to_bq(input)
		);
		return key_pair.second.value();
	}

	CryptoPP::ByteQueue& get_eax() {
		if (has_eax()) {
			return key_pair.second.value();
		}
		throw no_eax_key_found("EAX key was not found.");
	}

	eax_optional& get_eax_optional() {
		if (has_eax()) {
			return key_pair.second;
		}
		throw no_eax_key_found("EAX key was not found.");
	}

	QTcpSocket* get_np2ps_socket() {
		return np2ps_tcp_socket_;
	}

	void set_np2ps_socket(QTcpSocket* socket) {
		np2ps_tcp_socket_ = socket;
	}

	void clean_np2ps_socket() {
		np2ps_tcp_socket_ = NULL;
	}

	void clean_turn_socket() {
		tcp_socket_ = NULL;
	}

	bool np2ps_socket_connected() {
		if (np2ps_tcp_socket_ &&
			np2ps_tcp_socket_->isValid() &&
			np2ps_tcp_socket_->state() == QAbstractSocket::ConnectedState)
		{
			return true;
		}
		return false;
	}

	bool stun_socket_connected() {
		if (tcp_socket_ &&
			tcp_socket_->isValid() &&
			tcp_socket_->state() == QAbstractSocket::ConnectedState)
		{
			return true;
		}
		return false;
	}

	bool ip_address_is_private(const QHostAddress& address) {
		
		quint32 address_number = address.toIPv4Address();

		if (address_number >= QHostAddress("10.1.0.0").toIPv4Address() && 
			address_number <= QHostAddress("10.1.255.255").toIPv4Address())
		{
			return true;
		}

		return false;

		// if ((address_number >= QHostAddress("10.0.0.0").toIPv4Address() && 
		// 	 address_number <= QHostAddress("10.255.255.255").toIPv4Address()) ||
        // 	(address_number >= QHostAddress("172.16.0.0").toIPv4Address() &&
		// 	 address_number <= QHostAddress("172.31.255.255").toIPv4Address()) ||
        // 	(address_number >= QHostAddress("192.168.0.0").toIPv4Address() &&
		// 	 address_number <= QHostAddress("192.168.255.255").toIPv4Address()))
		// {
        // 	return true;
    	// }

		// return false;
	}

	void serialize_wrapper(np2ps::IpWrapper* wrapper, bool local_serialize = true) {
		if (local_serialize || !ip_address_is_private(ipv4)) {
			wrapper->set_ipv4(ipv4.toIPv4Address());
			wrapper->set_port(port);
		}

		if (key_pair.second.has_value() && local_serialize) {
			std::string shared_key_b64 = get_eax_hex_string();
			wrapper->set_eax_key(shared_key_b64);
		}

		if (key_pair.first.has_value()) {
			std::string public_key_b64 = get_rsa_hex_string();
			wrapper->set_rsa_public_key(public_key_b64);
		}

		switch(relay_state) {
			case RelayState::Direct:
				wrapper->set_relay_state(np2ps::RelayState::Direct);
				break;
			case RelayState::Relayed:
				wrapper->set_relay_state(np2ps::RelayState::Relayed);
				break;
			case RelayState::Unknown:
				wrapper->set_relay_state(np2ps::RelayState::Unknown);
				break;
			default:
				throw unsupported_feature("Serializing unknown relay state.");
		}

		if (local_serialize) {
			pk_t temp = 0;
			for (int i = 0; i < relay_by.size(); i++) {
				temp = relay_by.front();
				relay_by.pop();
				wrapper->add_relay_by(temp);
				relay_by.push(temp);
			}
			for (auto&& relay_to_peer : relay_to) {
				wrapper->add_relay_to(relay_to_peer);
			}
		}
	}

	pk_t get_relay_stun_server() {
		return relay_by.front();
	}

	pk_t next_relay_stun_server() {
		relay_by.push(relay_by.front());
		relay_by.pop();
		if (first_relay_stun_server != 0 && first_relay_stun_server == relay_by.front()) {
			first_relay_stun_server = 0;
			throw no_more_relay_stun_servers(
				"All known relay STUN servers for this peer were used."
			);
		}
		return relay_by.front();
	}

	bool has_relay_stun_servers() {
		return relay_by.size() != 0;
	}

	void begin_relay_stun_server_tracking() {
		first_relay_stun_server = get_relay_stun_server();
	}

	void end_relay_stun_server_tracking() {
		first_relay_stun_server = 0;
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
	
	RelayState relay_state = RelayState::Unknown;
	user_container relay_to;
	std::queue<pk_t> relay_by;
	pk_t first_relay_stun_server = 0;
};

using ip_map = std::unordered_map<pk_t, IpWrapper>;


#endif //PROGRAM_IPWRAPPER_H
