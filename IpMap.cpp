#include "IpMap.h"

bool IpMap::add_to_map(pk_t pk, IpWrapper&& ip) {
	return map_.insert({pk, ip}).second;
}

void IpMap::remove_from_map(pk_t pk) {
	map_.erase(pk);
}

/**
 * @brief Update given IP.
 * 
 * @param pk Which user IP to update.
 * @param ip IP to use.
 * @return True, if update took place.
 */
bool IpMap::update_ip(pk_t pk, const QHostAddress& ip) {
	if (map_.find(pk) == map_.end()) {
		return map_.insert({pk, IpWrapper(ip)}).second;
	}
	else {
		map_.at(pk).ipv4 = ip;
		return true;
	}
}

/**
 * @brief Update given IP.
 * 
 * @param pk Which user IP to update.
 * @param ip4 IPv4 to use.
 * @param ip6 IPv6 to use.
 * @return True, if update took place.
 */
bool IpMap::update_ip(pk_t pk, const QHostAddress& ip4, const QHostAddress& ip6) {
	if (map_.find(pk) == map_.end()) {
		return map_.insert({pk, IpWrapper(ip4, ip6)}).second;
	}
	else {
		map_.at(pk).ipv4 = ip4;
		map_.at(pk).ipv6 = ip6;
		return true;
	}
}

bool IpMap::update_rsa_public(pk_t pk, const std::string& eax) {
	if (map_.find(pk) == map_.end()) {
		return false;
	}
	else {
		map_.at(pk).add_rsa_key(eax);
		return true;
	}
}

bool IpMap::update_eax(pk_t pk, const std::string& rsa) {
	if (map_.find(pk) == map_.end()) {
		return false;
	}
	else {
		map_.at(pk).add_rsa_key(rsa);
		return true;
	}
}

QHostAddress IpMap::get_ip(pk_t pk) {
	auto it = map_.find(pk);
	if (it != map_.end()) {
		//element was found in map
		return it->second.ipv4;
	}

	return QHostAddress();
}

bool IpMap::have_ip(pk_t pk) {
	auto it = map_.find(pk);

	if (it != map_.end())
		return !(it->second.ipv4.isNull() || it->second.ipv6.isNull());

	return false;
}

ip_map::iterator IpMap::get_wrapper_for_pk(pk_t pk) {
	auto find_result = map_.find(pk);
	if (find_result != map_.end()) {
		return find_result;
	}
	return map_.end();
}
