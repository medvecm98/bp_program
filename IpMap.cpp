#include "IpMap.h"

user_not_found_in_database create_ip_wrapper_error(pk_t user, const std::string& reason) {
	std::stringstream ss;
	ss << "IpWrapper for user " << user << " was not found when " << reason << ".";
	return user_not_found_in_database(ss.str().c_str());
}

IpMap::IpMap(pk_t public_id) {
	my_public_id = public_id;
}

IpMap::IpMap(const np2ps::IpMap& ip_map_serialized) : 
	my_public_id(ip_map_serialized.my_public_id())
{
	std::cout << "Deserializing IP MAP" << std::endl;
	for (auto& wrapper : ip_map_serialized.wrapper_map()) {
		std::cout << " deserializing user: " << wrapper.publicid() << std::endl;
		map_.emplace(wrapper.publicid(), wrapper);
	}

	if (ip_map_serialized.has_rsa_private_key()) {
		using namespace CryptoPP;
		std::string encoded_key = ip_map_serialized.rsa_private_key();
		std::string decoded_key;
		Base64Decoder decoder;
		decoder.Put( (byte*) encoded_key.data(), encoded_key.size() );
		decoder.MessageEnd();

		word64 size = decoder.MaxRetrievable();
		if (size && size <= SIZE_MAX) {
			decoded_key.resize(size);
			decoder.Get((byte*)&decoded_key[0], decoded_key.size());
		}

		ByteQueue bq;

		StringSource ss(decoded_key, true);
		ss.TransferTo(bq);
		RSA::PrivateKey public_id;
		public_id.Load(bq);
		private_rsa = {public_id};
	}
}

bool IpMap::add_to_map(pk_t public_id, IpWrapper&& ip) {
	return map_.emplace(public_id, ip).second;
}

bool IpMap::add_to_map(pk_t public_id, const IpWrapper& ip) {
	return map_.emplace(public_id, ip).second;
}

bool IpMap::add_to_map(pk_t id, QHostAddress& ip, std::uint16_t port) {
	IpWrapper ipw(ip, port);
	return map_.emplace(id, std::move(ipw)).second;
}

void IpMap::remove_from_map(pk_t public_id) {
	map_.erase(public_id);
}

void IpMap::enroll_new_np2ps_tcp_socket(pk_t id, QTcpSocket* socket, bool* updated_socket) {
	if (socket) { //check if socket isn't NULL, function IS called with socket set to null
		try {
			auto& w = get_wrapper_ref(id);
			auto* np2ps_socket = w.get_np2ps_socket(); 

			if (np2ps_socket && !np2ps_socket->isValid()) {
				w.clean_np2ps_socket();
				std::cout << "Removing socket that was previously enrolled for " 
						  << id << std::endl;
			}

			if (!np2ps_socket) { //check if given NP2PS socket isn't already enrolled
				socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
				w.set_np2ps_socket(socket);
				std::cout << "NP2PS socket NEWLY enrolled for " << id << std::endl;
				if (updated_socket) {
					*updated_socket = true;
				}
			}
			else {
				std::cout << "NP2PS socket already enrolled for " << id << std::endl;
			}
		}
		catch (const user_not_found_in_database& e) {
			std::cout << "User " << id << " readded to database\n";
			IpWrapper ipw(socket->peerAddress(), socket->peerPort());
			socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
			ipw.set_np2ps_socket(socket);
			
			add_to_map(id, std::move(ipw));
		}
	}
}

bool IpMap::update_ip(pk_t public_id, const QHostAddress& ip) {
	if (map_.find(public_id) == map_.end()) {
		return map_.emplace(public_id, IpWrapper(ip)).second;
	}
	else {
		map_.at(public_id).ipv4 = ip;
		return true;
	}
}

bool IpMap::update_ip(pk_t public_id, const QHostAddress& ip, std::uint16_t port /*= PORT*/, port_t stun_port) {
	if (map_.find(public_id) == map_.end()) {
		return map_.emplace(public_id, IpWrapper(ip, port, stun_port)).second;
	}
	else {
		map_.at(public_id).ipv4 = ip;
		map_.at(public_id).port = port;
		map_.at(public_id).stun_port = stun_port;
		return true;
	}
}


bool IpMap::update_ip(pk_t public_id, const QHostAddress& ip4, const QHostAddress& ip6, std::uint16_t port /*= PORT*/) {
	if (map_.find(public_id) == map_.end()) {
		return map_.insert({public_id, IpWrapper(ip4, ip6, port)}).second;
	}
	else {
		map_.at(public_id).ipv4 = ip4;
		map_.at(public_id).ipv6 = ip6;
		map_.at(public_id).port = port;
		return true;
	}
}

bool IpMap::update_rsa_public(pk_t public_id, const std::string& rsa) {
	if (map_.find(public_id) == map_.end()) {
		throw create_ip_wrapper_error(public_id, "updating for RSA public");
	}
	else {
		map_.at(public_id).add_rsa_key(rsa);
		return true;
	}
}

bool IpMap::update_rsa_public(pk_t public_id, const CryptoPP::RSA::PublicKey& rsa) {
	if (map_.find(public_id) == map_.end()) {
		return map_.emplace(public_id, IpWrapper(rsa)).second;
	}
	else {
		map_.at(public_id).add_rsa_key(rsa);
		return true;
	}
}

bool IpMap::update_eax(pk_t public_id, const std::string& eax) {
	if (map_.find(public_id) == map_.end()) {
		throw create_ip_wrapper_error(public_id, "updating EAX string");
	}
	else {
		map_.at(public_id).set_eax_hex_string(eax);
		return true;
	}
}

bool IpMap::update_eax(pk_t public_id, CryptoPP::ByteQueue& eax) {
	if (map_.find(public_id) == map_.end()) {
		throw create_ip_wrapper_error(public_id, "updating EAX");
	}
	else {
		map_.at(public_id).add_eax_key(std::move(eax));
		return true;
	}
}

QHostAddress IpMap::get_ip4(pk_t public_id) {
	auto it = map_.find(public_id);
	if (it != map_.end()) {
		//element was found in map
		return it->second.ipv4;
	}

	throw create_ip_wrapper_error(public_id, "getting ipv4");
}

QHostAddress IpMap::get_ip6(pk_t public_id) {
	auto it = map_.find(public_id);
	if (it != map_.end()) {
		//element was found in map
		return it->second.ipv6;
	}

	throw create_ip_wrapper_error(public_id, "getting ipv6");
}

std::uint16_t IpMap::get_port(pk_t public_id) {
	auto it = map_.find(public_id);
	if (it != map_.end()) {
		//element was found in map
		return it->second.port;
	}

	throw create_ip_wrapper_error(public_id, "getting port");
}

rsa_public_optional IpMap::get_rsa_public(pk_t public_id) {
	auto it = map_.find(public_id);
	if (it != map_.end()) {
		//element was found in map
		return it->second.key_pair.first;
	}

	throw create_ip_wrapper_error(public_id, "getting RSA public");
}

eax_optional IpMap::get_eax(pk_t public_id) {
	auto it = map_.find(public_id);
	if (it != map_.end()) {
		//element was found in map
		return it->second.key_pair.second;
	}

	throw create_ip_wrapper_error(public_id, "getting EAX");
}


bool IpMap::have_ip4(pk_t public_id) {
	auto it = map_.find(public_id);

	if (it != map_.end())
		return !it->second.ipv4.isNull();

	throw create_ip_wrapper_error(public_id, "checking for ipv4");
}

bool IpMap::have_ip6(pk_t public_id) {
	auto it = map_.find(public_id);

	if (it != map_.end())
		return !it->second.ipv6.isNull();

	throw create_ip_wrapper_error(public_id, "checking for ipv6");
}

bool IpMap::have_rsa_public(pk_t public_id) {
	auto it = map_.find(public_id);

	if (it != map_.end())
		return it->second.key_pair.first.has_value();

	throw create_ip_wrapper_error(public_id, "checking for RSA public");
}

bool IpMap::have_eax(pk_t public_id) {
	auto it = map_.find(public_id);

	if (it != map_.end())
		return it->second.key_pair.second.has_value();
	
	throw create_ip_wrapper_error(public_id, "checking for EAX");
}

ip_map::iterator IpMap::get_wrapper_for_pk(pk_t public_id) {
	auto find_result = map_.find(public_id);
	if (find_result != map_.end()) {
		return find_result;
	}
	throw create_ip_wrapper_error(public_id, "getting entire wrapper");
}

IpWrapper& IpMap::get_wrapper_ref(pk_t id) {
	auto find_result = map_.find(id);
	if (find_result != map_.end()) {
		return find_result->second;
	}
	throw create_ip_wrapper_error(id, "getting wrapper ref");
}

void IpMap::set_tcp_socket(pk_t public_id, QTcpSocket* tcp_socket_) {
	if (map_.find(public_id) == map_.end()) {
		throw create_ip_wrapper_error(public_id, "setting TCP socker");
	}
	else {
		map_.at(public_id).copy_tcp_socket(tcp_socket_);
	}	
}
		
QTcpSocket* IpMap::get_tcp_socket(pk_t public_id) {
	if (map_.find(public_id) == map_.end()) {
		return NULL;
	}
	else {
		return map_.at(public_id).get_tcp_socket();
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

bool IpMap::have_port(pk_t public_id) {
	auto it = map_.find(public_id);

	if (it != map_.end())
		return it->second.port != 0;

	throw create_ip_wrapper_error(public_id, "checking for port");
}

bool IpMap::update_stun_ip(pk_t pid, const QHostAddress& ip, std::uint16_t port) {
	auto map_it = map_.find(pid);
	if (map_it == map_.end()) {
		throw create_ip_wrapper_error(pid, "checking for STUN IP");
	}
	else {
		map_it->second.stun_address = ip;
		map_it->second.stun_port = port;
	}
	return true;
}

void IpMap::remove_disconnected_users(std::vector<pk_t>& public_ids_to_remove, QTcpSocket* socket) {
	for (auto&& item : map_) {
		// if (item.second.tcp_socket_) { //check if STUN socket is connected
		// 	if (item.second.tcp_socket_->state() == QAbstractSocket::UnconnectedState ||
		// 		item.second.tcp_socket_->error() == QAbstractSocket::RemoteHostClosedError) 
		// 	{
		// 		item.second.tcp_socket_ = NULL; //NULLify the STUN socket
		// 	}
		// }
		// if (!item.second.tcp_socket_) { //if both sockets were NULLified
		// 	public_ids_to_remove.push_back(item.first); //push ID of that peer into list of peers to remove
		// }
		std::cout << "Checking peer " << item.first << std::endl;
		if (item.second.get_np2ps_socket() == socket || item.second.get_tcp_socket() == socket) {
			std::cout << "Removing peer " << item.first << std::endl;
			public_ids_to_remove.push_back(item.first);
		}
	}
}

bool IpMap::has_wrapper(pk_t id) {
	if (map_.find(id) == map_.end()) {
		return false;
	}

	return true;
}

std::set<pk_t> IpMap::my_mapped_users() {
	std::set<pk_t> rv;
	for (auto&& [pid, wrap] : map_) {
		rv.emplace(pid);
	}
	return rv;
}

void IpMap::add_to_ip_map(pk_t pid, const IpWrapper& wrapper) {
	map_.emplace(pid, wrapper);
}

void IpMap::add_or_update_to_ip_map(pk_t pid, const IpWrapper& wrapper) {
	map_.erase(pid);
	map_.emplace(pid, wrapper);
}


void IpMap::add_to_ip_map(pk_t pid, IpWrapper&& wrapper) {
	map_.emplace(pid, wrapper);
}

void IpMap::add_to_ip_map_relayed(pk_t pid, pk_t relay_by_) {
	if (relay_by_ == my_public_id) {
		return;
	}
	IpWrapper wrapper;
	wrapper.set_relay_state(true);
	try {
		IpWrapper& relayer_wrapper = get_wrapper_ref(relay_by_);
		if (relayer_wrapper.relay_state == RelayState::Direct) {
			wrapper.add_relay_server(relay_by_);
			map_.emplace(pid, std::move(wrapper));
		}
	}
	catch (user_not_found_in_database e) {}
}

void IpMap::check_or_add_to_ip_map_relayed(pk_t pid, pk_t relay_by_, RelayState state) {
	try {
		IpWrapper& wrapper = get_wrapper_ref(pid);
		if (pid != relay_by_) {
			wrapper.set_relay_state(state);
			wrapper.add_relay_server(relay_by_);
		}
	}
	catch(user_not_found_in_database e) {
		add_to_ip_map_relayed(pid, relay_by_);
	}
}

void IpMap::check_and_remove_relayed(pk_t pid, pk_t relay_by_) {
	try {
		IpWrapper& wrapper = get_wrapper_ref(pid);
		pk_t temp = 0;
		for (int i = 0; i < wrapper.relay_by.size(); i++) {
			temp = wrapper.relay_by.front();
			wrapper.relay_by.pop_front();
			if (temp != relay_by_) {
				wrapper.add_relay_server(temp);
			}
			else {
				break;
			}
		}
	}
	catch(user_not_found_in_database e) {}
}

std::list<std::pair<pk_t, IpWrapper>> IpMap::select_connected_randoms(int count) {
	std::list<std::pair<pk_t, IpWrapper>> rv;
	std::vector<std::pair<pk_t, IpWrapper>> connected;
	for (auto&& wrapper : map_) {
		if (wrapper.second.np2ps_socket_connected()) {
			connected.push_back({wrapper.first, IpWrapper(wrapper.second.ipv4, wrapper.second.port)});
		}
	}
	std::random_device dev;
    std::mt19937 rng(dev());
	if (count > connected.size() || count <= 0) {
		count = connected.size();
	}
    std::shuffle(connected.begin(), connected.end(), rng);
	for (int i = 0; i < count; i++) {
		rv.push_back(connected[i]);
	}
	return rv;
}

std::list<std::pair<pk_t, IpWrapper>> IpMap::select_connected(int count) {
	std::list<std::pair<pk_t, IpWrapper>> connected;
	for (auto&& wrapper : map_) {
		if (wrapper.second.np2ps_socket_connected()) {
			connected.push_back({wrapper.first, wrapper.second});
		}
	}
	return connected;
}

std::list<std::pair<pk_t, IpWrapper>> IpMap::select_direct(int count) {
	std::list<std::pair<pk_t, IpWrapper>> direct;
	for (auto&& wrapper : map_) {
		if (wrapper.second.get_relay_state() == RelayState::Direct) {
			direct.push_back({wrapper.first, wrapper.second});
		}
	}
	return direct;
}
