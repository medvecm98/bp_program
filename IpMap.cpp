//
// Created by michal on 26. 3. 2021.
//

#include "IpMap.h"

bool IpMap::add_to_map(pk_t pk, IpWrapper&& ip) {
	return _map.insert( {pk, ip}).second;
}

void IpMap::remove_from_map(pk_t pk) {
	_map.erase(pk);
}

/**
 *
 * @param pk
 * @param ip
 * @return True, if update took place.
 */
bool IpMap::update_ip(pk_t pk, IpWrapper&& ip) {
	if (_map.find(pk) == _map.end()) {
		_map.insert( {pk, ip});
	}
	return false;
}