#ifndef PROGRAM_IPMAP_H
#define PROGRAM_IPMAP_H

#include "IpWrapper.h"
#include <optional>

class IpMap {
public:
	bool add_to_map(pk_t, IpWrapper&& ip);
	void remove_from_map(pk_t);
	bool update_ip(pk_t, const std::string& ip);
	bool update_ip(pk_t, const std::string& ip4, const std::string& ip6);
	bool update_rsa_public(pk_t, const std::string&);
	bool update_eax(pk_t, const std::string&);
	std::string get_ip(pk_t);
	bool have_ip(pk_t);
	ip_map::iterator get_map_end() {
		return map_.end();
	}
	ip_map::iterator get_wrapper_for_pk(pk_t);
	IpWrapper my_ip;
	pk_t my_public_id;
	rsa_private_optional private_rsa;
private:
	ip_map map_;
};

#endif //PROGRAM_IPMAP_H
