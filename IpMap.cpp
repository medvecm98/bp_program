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