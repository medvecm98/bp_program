#ifndef PROGRAM_IPMAP_H
#define PROGRAM_IPMAP_H

#include "IpWrapper.h"
#include <cryptopp/files.h>
#include <filesystem>
#include <optional>
#include <random>

/**
 * @brief Very important class, that creates the interface to
 * all the networking related information.
 * 
 * Stores IPs, ports, public and symmetric keys for all the peers using the
 * IpWrapper class in the map.
 * 
 */
class IpMap {
public:
	IpMap() = delete;

	explicit IpMap(pk_t public_id);

	explicit IpMap(const np2ps::IpMap& ip_map_serialized);

	/**
	 * @brief Adds a new IpWrapper into the map.
	 * 
	 * Calls `.emplace` function on underlying map.
	 * 
	 * @param id Public identifier of peer who's IpWrapper is being inserted.
	 * @param ip IpWrapper to insert.
	 * @return true If insertion took place.
	 * @return false If insertion did not took place.
	 */
	bool add_to_map(pk_t id, IpWrapper&& ip);
	bool add_to_map(pk_t id, const IpWrapper& ip);
	bool add_to_map(pk_t id, QHostAddress& ip, std::uint16_t port);

	/**
	 * @brief Removes the IpWrapper and thus all the infromation for the peer.
	 * 
	 * @param id Public identifier of peer to remove all information about.
	 */
	void remove_from_map(pk_t id);

	/**
	 * @brief Updates the preferred STUN server of the peer.
	 * 
	 * @param who Whose STUN server to update.
	 * @param server STUN server public identifier.
	 */
	void update_preferred_stun_server(pk_t who, pk_t server);

	/**
	 * @brief Updates the IPv4 for given peer.
	 * 
	 * Insertion will take place even if peer wasn't enrolled in ip_map_ before.
	 * 
	 * @param id Public identifier of peer, whose IP will be updated
	 * @param ip New IP.
	 * @param port New port.
	 * @return true If update took place.
	 * @return false Otherwise.
	 */
	bool update_ip(pk_t id, const QHostAddress& ip, std::uint16_t port = PORT);

	/**
	 * @brief Updates the STUN IPv4 for given peer.
	 * 
	 * If peer wasn't enrolled before, no insertion will take place.
	 * 
	 * @param id Public identifier of peer, whose STUN IP will be updated
	 * @param ip New IP.
	 * @param port New port.
	 * @return true If update took place.
	 * @return false Otherwise.
	 */
	bool update_stun_ip(pk_t id, const QHostAddress& ip, std::uint16_t port = 3478);

	/**
 	 * @brief Update given IP.
 	 * 
 	 * @param pk Which user IP to update.
 	 * @param ip4 IPv4 to use.
 	 * @param ip6 IPv6 to use.
 	 * @return True, if update took place.
 	 */
	bool update_ip(pk_t pk, const QHostAddress& ip4, const QHostAddress& ip6, std::uint16_t port = PORT);

	/**
	 * @brief Updates the RSA public key for given peer.
	 * 
	 * If peer wasn't enrolled before, no insertion will take place.
	 * 
	 * @param id Public ID of the peer whose RSA public key will be updated.
	 * @param rsa RSA public key in DER encoded form.
	 * @return True, if update took place.
	 */
	bool update_rsa_public(pk_t id , const std::string& rsa);

	/**
	 * @brief Updates the RSA public key for given peer.
	 * 
	 * If peer wasn't enrolled before, no insertion will take place.
	 * 
	 * @param id Public ID of the peer whose RSA public key will be updated.
	 * @param rsa RSA public key.
	 * @return True, if update took place.
	 */
	bool update_rsa_public(pk_t, const CryptoPP::RSA::PublicKey& rsa);

	/**
	 * @brief Updates the EAX key for given peer.
	 * 
	 * No insertion will take place if peer wasn't enrolled before.
	 * 
	 * @param id Peer whose EAX symmetric key will be updated.
	 * @return True, if insertion took place.
	 */
	bool update_eax(pk_t id, const std::string&);

	bool update_eax(pk_t id, CryptoPP::ByteQueue& eax);

	/**
	 * @brief Sets the TCP socket for STUN communication.
	 * 
	 * Insertion won't take place, unless the peer was already enrolled.
	 * 
	 * @param id Public ID of Peer to who this socket is connected.
	 * @param tcp_socket_ TCP socket pointer to set.
	 */
	void set_tcp_socket(pk_t id, QTcpSocket* tcp_socket_);

	/**
	 * @brief Gets the pointer to TCP socket for communication with given peer.
	 * 
	 * @param id Peer whose TCP socket to get.
	 * @return QTcpSocket* Pointer to TCP socket.
	 */
	QTcpSocket* get_tcp_socket(pk_t id);
	
	/**
	 * @brief Get the IPv4 address of the peer.
	 * 
	 * Default contructed QHostAddress is returned, when peer wasn't found.
	 * 
	 * @param id Public identifier of peer, whose IPv4 we want.
	 * @return QHostAddress IPv4 of requested peer.
	 */
	QHostAddress get_ip4(pk_t id);

	/**
	 * @brief Get the IPv6 address of the peer.
	 * 
	 * Default contructed QHostAddress is returned, when peer wasn't found.
	 * 
	 * @param id Public identifier of peer, whose IPv6 we want.
	 * @return QHostAddress IPv6 of requested peer.
	 */
	QHostAddress get_ip6(pk_t);

	/**
	 * @brief Gets the port of the peer.
	 * 
	 * Exception of std::logic_error is thrown, when peer was not found.
	 * 
	 * @return std::uint16_t Port of the requested peer.
	 */
	std::uint16_t get_port(pk_t);

	/**
	 * @brief Gets the RSA public key. 
	 * 
	 * std::optional with no value is returned when peer was not found.
	 * 
	 * @return Optional value with RSA public key.
	 */
	rsa_public_optional get_rsa_public(pk_t);

	/**
	 * @brief Gets the EAX+AES symmetric key. 
	 * 
	 * std::optional with no value is returned when peer was not found.
	 * 
	 * @return Optional value with EAX+AES symmetric key.
	 */
	eax_optional get_eax(pk_t);

	/**
	 * @brief Checks, if peer has IPv4 assigned in the map.
	 * 
	 * @return True, if IPv4 was found, otherwise false.
	 */
	bool have_ip4(pk_t);

	/**
	 * @brief Checks, if peer has IPv6 assigned in the map.
	 * 
	 * @return True, if IPv6 was found, otherwise false.
	 */
	bool have_ip6(pk_t);

	/**
	 * @brief Checks, if peer has port assigned in the map.
	 * 
	 * Exceptionally useful, because port that can't be found will always throw
	 * an exception.
	 * 
	 * @return True if port was found, otherwise false.
	 */
	bool have_port(pk_t);

	/**
	 * @brief Checks, if peer has RSA public key assigned in the map.
	 * 
	 * @return True, if RSA public key was found, otherwise false.
	 */
	bool have_rsa_public(pk_t);

	/**
	 * @brief Checks, if peer has EAX+AES symmetric key assigned in the map.
	 * 
	 * @return True, if EAX+AES symmetric key was found, otherwise false.
	 */
	bool have_eax(pk_t);

	/**
	 * @brief Gets the end iterator of the map.
	 */
	ip_map::iterator get_map_end() {
		return map_.end();
	}

	/**
	 * @brief Inserts TCP socket for NP2PS communication into the map.
	 * 
	 * Checks, if the socket pointer has a non NULL value.
	 * Sets the socket option to be always alive.
	 * Update won't take place, if socket is already assigned.
	 * 
	 * @param id ID of peer whose NP2PS socket will be updated.
	 * @param updated_socket True, if socket was updated to already existing peer.
	 * @param socket Socket to use.
	 */
	void enroll_new_np2ps_tcp_socket(pk_t id, QTcpSocket* socket, bool* updated_socket = NULL);

	/**
	 * @brief Gets the IpWrapper for given peer.
	 * 
	 * Returns end iterator if no such peer was enrolled.
	 * 
	 * @param id Peer public identifier.
	 * @return ip_map::iterator IpWrapper for requested peer.
	 */
	ip_map::iterator get_wrapper_for_pk(pk_t id);

	IpWrapper& get_wrapper_ref(pk_t id);

	bool has_wrapper(pk_t id);

	/**
	 * @brief Scans all enrolled sockets and remove those peers, who are 
	 * comlpetely disconnected.
	 * 
	 * @param public_ids_to_remove Push back peers, whose sockets were detected disconnected.
	 */
	void remove_disconnected_users(std::vector<pk_t>& public_ids_to_remove, QTcpSocket* socket);

	void serialize_ip_map(np2ps::IpMap* im) {
		std::cout << "Serializing IP MAP" << std::endl;
		im->set_my_public_id(my_public_id);

		if (private_rsa.has_value()) {
			std::string public_key_b64;
			CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(public_key_b64));
			CryptoPP::RSA::PublicKey& private_key = private_rsa.value();

			CryptoPP::ByteQueue queue;
			private_key.Save(queue);
			queue.CopyTo(encoder);
			encoder.MessageEnd();
			im->set_rsa_private_key(public_key_b64);
		}

		for (auto&& wrapper_map_pair : map_) {
			std::cout << "	serializing user: " << wrapper_map_pair.first << std::endl;
			auto& wrapper = wrapper_map_pair.second;
			auto serialized_wrapper = im->add_wrapper_map();
			wrapper.serialize_wrapper(serialized_wrapper);
			serialized_wrapper->set_publicid(wrapper_map_pair.first);
		}
	}

	IpWrapper& my_ip() {
		return map_.at(my_public_id);
	}

	std::set<pk_t> my_mapped_users();

	void add_to_ip_map(pk_t pid, const IpWrapper& wrapper);
	void add_to_ip_map(pk_t pid, IpWrapper&& wrapper);

	std::list<std::pair<pk_t, IpWrapper>> select_connected_randoms(int count);

	// IpWrapper my_ip; //IpWrapper containing information related to my networking
	pk_t my_public_id; //my public identifier, set by Peer
	rsa_private_optional private_rsa; //my private RSA key
private:
	ip_map map_; //The map
};

#endif //PROGRAM_IPMAP_H
