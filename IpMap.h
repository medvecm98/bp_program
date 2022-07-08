#ifndef PROGRAM_IPMAP_H
#define PROGRAM_IPMAP_H

#include "IpWrapper.h"
#include <cryptopp/files.h>
#include <filesystem>
#include <optional>

class IpMap {
	friend class boost::serialization::access;
public:
	bool add_to_map(pk_t, IpWrapper&& ip);
	void remove_from_map(pk_t);
	bool update_ip(pk_t, const QHostAddress& ip, std::uint16_t port = PORT);
	bool update_ip(pk_t, const QHostAddress& ip4, const QHostAddress& ip6, std::uint16_t port = PORT);
	bool update_rsa_public(pk_t, const std::string& rsa);
	bool update_rsa_public(pk_t, const CryptoPP::RSA::PublicKey& rsa);
	bool update_eax(pk_t, const std::string&);
	void set_tcp_socket(pk_t id, QTcpSocket* tcp_socket_);
	QTcpSocket* get_tcp_socket(pk_t id);
	QHostAddress get_ip4(pk_t);
	QHostAddress get_ip6(pk_t);
	std::uint16_t get_port(pk_t);
	std::shared_ptr<rsa_public_optional> get_rsa_public(pk_t);
	std::shared_ptr<eax_optional> get_eax(pk_t);
	bool have_ip4(pk_t);
	bool have_ip6(pk_t);
	bool have_rsa_public(pk_t);
	bool have_eax(pk_t);
	ip_map::iterator get_map_end() {
		return map_.end();
	}
	void enroll_new_np2ps_tcp_socket(pk_t id, QTcpSocket* socket);

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void save(Archive& ar, const unsigned int version) const {
		ar & my_ip;
		ar & my_public_id;
		if (private_rsa.has_value()) {
			std::string s;
			CryptoPP::StringSink ss(s);
			private_rsa.value().DEREncode(ss);
			ar & s;
		}
		else {
			ar & "";
		}
		ar & map_;
	}

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void load(Archive& ar, const unsigned int version) {
		ar & my_ip;
		ar & my_public_id;
		std::string private_rsa_str;
		ar & private_rsa_str;
		if (private_rsa_str != "") {
			private_rsa = {CryptoPP::RSA::PrivateKey()};
			CryptoPP::StringSource ss(private_rsa_str, true);
			private_rsa.value().BERDecode(ss);
		}
		else {
			private_rsa = {};
		}
		ar & map_;
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()
	

	ip_map::iterator get_wrapper_for_pk(pk_t);
	IpWrapper my_ip;
	pk_t my_public_id;
	rsa_private_optional private_rsa;
private:
	void serialize_keys();
	void deserialize_keys();
	ip_map map_;
};

#endif //PROGRAM_IPMAP_H
