#include "IpMap.h"

bool IpMap::add_to_map(pk_t pk, IpWrapper&& ip) {
	return map_.insert({pk, ip}).second;
}

void IpMap::remove_from_map(pk_t pk) {
	map_.erase(pk);
}

/**
 *	Update given IP.
 * @param pk
 * @param ip
 * @return True, if update took place.
 */
bool IpMap::update_ip(pk_t pk, IpWrapper&& ip) {
	if (map_.find(pk) == map_.end()) {
		map_.insert({pk, ip});
		return true;
	}
	return false;
}

bool IpMap::get_ip(pk_t pk, IpWrapper& ip) {
	auto it = map_.find(pk);
	if (it != map_.end()) {
		//element was found in map
		ip = it->second;
		return true;
	}

	return false;
}