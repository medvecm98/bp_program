#ifndef PROGRAM_IPMAP_H
#define PROGRAM_IPMAP_H

#include "IpWrapper.h"
#include <optional>

class IpMap {
public:
	bool add_to_map(pk_t, IpWrapper&& ip);
	void remove_from_map(pk_t);
	bool update_ip(pk_t, const QHostAddress& ip);
	bool update_ip(pk_t, const QHostAddress& ip4, const QHostAddress& ip6);
	bool update_rsa_public(pk_t, const std::string&);
	bool update_eax(pk_t, const std::string&);
	QHostAddress get_ip4(pk_t);
	QHostAddress get_ip6(pk_t);
	std::shared_ptr<rsa_public_optional> get_rsa_public(pk_t);
	std::shared_ptr<eax_optional> get_eax(pk_t);
	bool have_ip4(pk_t);
	bool have_ip6(pk_t);
	bool have_rsa_public(pk_t);
	bool have_eax(pk_t);
	ip_map::iterator get_map_end() {
		return map_.end();
	}

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & my_ip;
		ar & my_public_id;
		ar & private_rsa;
	}

	ip_map::iterator get_wrapper_for_pk(pk_t);
	IpWrapper my_ip;
	pk_t my_public_id;
	rsa_private_optional private_rsa;
private:
	ip_map map_;
};

#endif //PROGRAM_IPMAP_H
