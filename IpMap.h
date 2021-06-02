#ifndef PROGRAM_IPMAP_H
#define PROGRAM_IPMAP_H

#include "IpWrapper.h"
#include <optional>

class IpMap {
public:
	bool add_to_map(pk_t, IpWrapper&& ip);
	void remove_from_map(pk_t);
	bool update_ip(pk_t, IpWrapper&& ip);
	bool get_ip(pk_t, IpWrapper&);
private:
	ip_map map_;
};

#endif //PROGRAM_IPMAP_H
