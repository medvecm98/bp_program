#include "IpMap.h"

bool IpMap::add_to_map(pk_t pk, IpWrapper&& ip) {
	return map_.emplace(pk, ip).second;
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

/**
 * @brief Update given IP.
 * 
 * @param pk Which user IP to update.
 * @param ip4 IPv4 to use.
 * @param ip6 IPv6 to use.
 * @return True, if update took place.
 */
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
		return false;
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

void IpMap::serialize_keys() {
	if (private_rsa.has_value()) {
		CryptoPP::ByteQueue bq;
		private_rsa.value().Save(bq);

		std::string dir = GU::get_program_home().append("/Keys");
		std::filesystem::create_directories(dir.c_str());
		std::fstream file;
		file.open(dir.append("/PrivateKey.prvk"), std::ios_base::out);
		CryptoPP::FileSink fs(file);

		bq.CopyTo(fs);
		fs.MessageEnd();
	}
}

void IpMap::deserialize_keys() {	
	std::string file_path = GU::get_program_home().append("/Keys").append("/PrivateKey.prvk");
	if (std::filesystem::is_regular_file(file_path)) {
		CryptoPP::FileSource fs(file_path.c_str(), true);
		CryptoPP::ByteQueue bq;

		fs.TransferTo(bq);
		bq.MessageEnd();

		private_rsa = {CryptoPP::RSA::PrivateKey()};
		private_rsa.value().Load(bq);
	}
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