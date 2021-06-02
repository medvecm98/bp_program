#include <cstdint>

#ifndef PROGRAM_IPWRAPPER_H
#define PROGRAM_IPWRAPPER_H

#include "GlobalUsing.h"
#include <unordered_map>

struct IpWrapper {
	explicit IpWrapper(const std::string& ip4) {
		ipv4 = ip4;
	}

	std::string ipv4;
	std::string ipv6;
};

using ip_map = std::unordered_map<pk_t, IpWrapper>;


#endif //PROGRAM_IPWRAPPER_H
