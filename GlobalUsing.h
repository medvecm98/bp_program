#ifndef PROGRAM_GLOBALUSING_H
#define PROGRAM_GLOBALUSING_H

#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <variant>
#include <filesystem>
#include <exception>
#include <utility>
#include <iostream>
#include <functional>
#include <chrono>
#include <memory>

#include <QHostAddress>
#include <QString>

#include "cryptopp/rsa.h"
#include "cryptopp/rijndael.h"
#include "cryptopp/eax.h"
#include "cryptopp/aes.h"
#include "cryptopp/pssr.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha3.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>

#include <cstdlib>

#define PORT 14128

using address_vec = std::vector<std::pair<QString, QHostAddress>>;
using address_vec_ptr = std::shared_ptr<address_vec>;

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

using pk_t = std::size_t;
using level_t = std::uint32_t;

/**
 * @brief Struct that contains all the information about peers other than me.
 * 
 */
struct PeerInfo {
	/**
	 * @brief Default contructor for a new Peer Info object.
	 */
	PeerInfo() = default;

	/**
	 * @brief Construct a new Peer Info object.
	 * 
	 * @param pk Public identifier od the peer.
	 */
	explicit PeerInfo(pk_t pk) {
		peer_key = pk;
		peer_level = 0;
	}

	/**
	 * @brief Construct a new Peer Info object providing the peer public ID.
	 * 
	 * @param pk Peer public identifier.
	 * @param level Level of the peer.
	 */
	PeerInfo(pk_t pk, level_t level) {
		peer_key = pk;
		peer_level = level;
	}
	pk_t peer_key = 0;
	level_t peer_level = 0; //usused
};

using seq_t = std::size_t;

using symmetric_cipher = CryptoPP::EAX< CryptoPP::AES>;
using asymmetric_cipher = CryptoPP::RSA;
using signer_verifier = CryptoPP::RSASS< CryptoPP::PSSR, CryptoPP::SHA256>;
using rsa_encryptor_decryptor = CryptoPP::RSAES< CryptoPP::OAEP< CryptoPP::SHA256>>;

using my_string = std::string;
using hash_t = std::size_t;
using user_container = std::unordered_set<pk_t>;
using user_container_citer = user_container::const_iterator;
using user_level_map = std::unordered_map<pk_t, PeerInfo>;
using user_level_map_citer = user_level_map::const_iterator;

using optional_my_string = std::optional<my_string>;

using my_clock = std::chrono::system_clock;

#endif //PROGRAM_GLOBALUSING_H
