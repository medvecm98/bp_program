#include "IpMap.h"

bool IpMap::add_to_map(pk_t pk, IpWrapper&& ip) {
	return map_.emplace(pk, ip).second;
}

void IpMap::remove_from_map(pk_t pk) {
	map_.erase(pk);
}

void IpMap::enroll_new_np2ps_tcp_socket(pk_t id, QTcpSocket* socket) {
	if (socket) { //check if socket isn't NULL, function IS called like that
		auto w = get_wrapper_for_pk(id);
		if (!w->second.np2ps_tcp_socket_) { //check if given NP2PS socket isn't already enrolled
			std::cout << "Enrolling np2ps socket " << id << std::endl;
			w->second.np2ps_tcp_socket_ = socket;
			w->second.np2ps_tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
		}
		else {
			std::cout << "Np2ps socket already enrolled " << id << std::endl;
		}
	}
}

bool IpMap::update_ip(pk_t pk, const QHostAddress& ip, std::uint16_t port /*= PORT*/) {
	if (map_.find(pk) == map_.end()) {
		return map_.emplace(pk, IpWrapper(ip, port)).second;
	}
	else {
		map_.at(pk).ipv4 = ip;
		map_.at(pk).port = port;
		return true;
	}
}


bool IpMap::update_ip(pk_t pk, const QHostAddress& ip4, const QHostAddress& ip6, std::uint16_t port /*= PORT*/) {
	if (map_.find(pk) == map_.end()) {
		return map_.insert({pk, IpWrapper(ip4, ip6, port)}).second;
	}
	else {
		map_.at(pk).ipv4 = ip4;
		map_.at(pk).ipv6 = ip6;
		map_.at(pk).port = port;
		return true;
	}
}

bool IpMap::update_rsa_public(pk_t pk, const std::string& rsa) {
	if (map_.find(pk) == map_.end()) {
		return false;
	}
	else {
		map_.at(pk).add_rsa_key(rsa);
		return true;
	}
}

bool IpMap::update_rsa_public(pk_t pk, const CryptoPP::RSA::PublicKey& rsa) {
	if (map_.find(pk) == map_.end()) {
		map_.emplace(pk, IpWrapper(rsa));
	}
	else {
		map_.at(pk).add_rsa_key(rsa);
		return true;
	}
}

bool IpMap::update_eax(pk_t pk, const std::string& eax) {
	if (map_.find(pk) == map_.end()) {
		return false;
	}
	else {
		map_.at(pk).add_eax_key(eax);
		return true;
	}
}

QHostAddress IpMap::get_ip4(pk_t pk) {
	auto it = map_.find(pk);
	if (it != map_.end()) {
		//element was found in map
		return it->second.ipv4;
	}

	return QHostAddress();
}

QHostAddress IpMap::get_ip6(pk_t pk) {
	auto it = map_.find(pk);
	if (it != map_.end()) {
		//element was found in map
		return it->second.ipv6;
	}

	return QHostAddress();
}

std::uint16_t IpMap::get_port(pk_t pk) {
	auto it = map_.find(pk);
	if (it != map_.end()) {
		//element was found in map
		return it->second.port;
	}

	throw std::logic_error("Such user was not found...");
}

std::shared_ptr<rsa_public_optional> IpMap::get_rsa_public(pk_t pk) {
	auto it = map_.find(pk);
	if (it != map_.end()) {
		//element was found in map
		return std::make_shared<rsa_public_optional>(it->second.key_pair.first);
	}

	return {};
}

std::shared_ptr<eax_optional> IpMap::get_eax(pk_t pk) {
	auto it = map_.find(pk);
	if (it != map_.end()) {
		//element was found in map
		return std::make_shared<eax_optional>(it->second.key_pair.second);
	}

	return {};
}


bool IpMap::have_ip4(pk_t pk) {
	auto it = map_.find(pk);

	if (it != map_.end())
		return !it->second.ipv4.isNull();

	return false;
}

bool IpMap::have_ip6(pk_t pk) {
	auto it = map_.find(pk);

	if (it != map_.end())
		return !it->second.ipv6.isNull();

	return false;
}

bool IpMap::have_rsa_public(pk_t pk) {
	auto it = map_.find(pk);

	if (it != map_.end())
		return it->second.key_pair.first.has_value();

	return false;
}

bool IpMap::have_eax(pk_t pk) {
	auto it = map_.find(pk);

	if (it != map_.end())
		return it->second.key_pair.second.has_value();

	return false;
}

ip_map::iterator IpMap::get_wrapper_for_pk(pk_t pk) {
	auto find_result = map_.find(pk);
	if (find_result != map_.end()) {
		return find_result;
	}
	return map_.end();
}

void IpMap::set_tcp_socket(pk_t pk, QTcpSocket* tcp_socket_) {
	if (map_.find(pk) == map_.end()) {
		return;
	}
	else {
		map_.at(pk).copy_tcp_socket(tcp_socket_);
	}	
}
		
QTcpSocket* IpMap::get_tcp_socket(pk_t pk) {
	if (map_.find(pk) == map_.end()) {
		return NULL;
	}
	else {
		return map_.at(pk).get_tcp_socket();
	}	
}

void IpMap::update_preferred_stun_server(pk_t who, pk_t server) {
	auto map_it = map_.find(who);
	if (map_it == map_.end()) {
		map_.emplace(who, IpWrapper(server));
	}
	else {
		map_it->second.preferred_stun_server = server;
	}
}

bool IpMap::have_port(pk_t pk) {
	auto it = map_.find(pk);

	if (it != map_.end())
		return it->second.port != 0;

	return false;
}

bool IpMap::update_stun_ip(pk_t pid, const QHostAddress& ip, std::uint16_t port) {
	auto map_it = map_.find(pid);
	if (map_it == map_.end()) {
		return false;
	}
	else {
		map_it->second.stun_address = ip;
		map_it->second.stun_port = port;
	}
	return true;
}

void IpMap::remove_disconnected_users(std::vector<pk_t>& public_ids_to_remove) {
	for (auto&& item : map_) {
		if (item.second.tcp_socket_) { //check if STUN socket is connected
			std::cout << "STUN Socket state: " << item.second.tcp_socket_->state() << std::endl;
			std::cout << "STUN Socket error: " << item.second.tcp_socket_->error() << std::endl;
			if (item.second.tcp_socket_->state() == QAbstractSocket::UnconnectedState ||
				item.second.tcp_socket_->error() == QAbstractSocket::RemoteHostClosedError) 
			{
				item.second.tcp_socket_ = NULL; //NULLify the STUN socket
			}
		}
		/*if (item.second.np2ps_tcp_socket_) { //check if NP2PS socket is connected
			std::cout << "NP2PS Socket state: " << item.second.np2ps_tcp_socket_->state() << std::endl;
			std::cout << "NP2PS Socket error: " << item.second.np2ps_tcp_socket_->error() << std::endl;
			if (item.second.np2ps_tcp_socket_->state() == QAbstractSocket::UnconnectedState ||
				item.second.np2ps_tcp_socket_->error() == QAbstractSocket::RemoteHostClosedError) 
			{
				item.second.np2ps_tcp_socket_ = NULL; //NULLify the NP2PS socket
			}
		}*/
		if (!item.second.tcp_socket_/* && !item.second.np2ps_tcp_socket_*/) { //if both sockets were NULLified
			public_ids_to_remove.push_back(item.first); //push ID of that peer into list of peers to remove
		}
	}
}