#ifndef PROGRAM_GLOBALUSING_H
#define PROGRAM_GLOBALUSING_H

#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <variant>
#include <filesystem>
#include <utility>
#include <iostream>
#include <functional>
#include <chrono>
#include <memory>

#include "MyExceptions.hpp"

#include <QHostAddress>
#include <QString>
#include <QDate>

#include "protobuf_source/articles.pb.h"
#include "protobuf_source/messages.pb.h"
#include "protobuf_source/newspaper_entry.pb.h"
#include "protobuf_source/peer.pb.h"
#include "protobuf_source/ip_map.pb.h"

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
#include <cryptopp/base64.h>

#include <cstdlib>

#define PORT 14128

/* LOGGER */

struct ELog {};

class Log {
public:
    static Log& instance() {
        static Log l;
        return l;
    }
    Log(const Log&) = delete;
    Log& operator= (const Log&) = delete;
	
    Log& log(int i) {
        buffer << i << delim;
        return *this;
    }
    Log& log(ELog dummy) {
        end_log();
        return *this;
    }

    template <typename T>
    Log& operator<< (T t) {
        return log(t);
    }

    std::string str() {
        return buffer.str();
    }
    void end_log() {
        std::cout << str() << end_delim << std::endl;
        buffer.str("");
    }
    void set_delim(const std::string& delimiter) {
        delim = delimiter;
    }
    void set_end_delim(const std::string& end_delimiter) {
        end_delim = end_delimiter;
    }
private:
    Log() {}
    ~Log() {}
    std::stringstream buffer;
    std::string delim = " ";
    std::string end_delim = "";
};

static ELog ELOG;
#define LOG Log::instance()

/* OTHER */

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

using my_timepoint = std::chrono::time_point<std::chrono::system_clock>;

using timestamp_t = std::uint64_t;
using timed_article_map = std::map<timestamp_t, pk_t>;
using timed_article_map_iter = timed_article_map::iterator;
using timed_article_map_citer = timed_article_map::const_iterator;
using timed_article_map_pair = std::pair<timed_article_map_iter, timed_article_map_iter>;
using timed_article_map_cpair = std::pair<timed_article_map_citer, timed_article_map_citer>;

using optional_my_string = std::optional<my_string>;

using my_clock = std::chrono::system_clock;

//types for map to store keys for given user
using rsa_public_optional = std::optional< CryptoPP::RSA::PublicKey>;
using rsa_private_optional = std::optional< CryptoPP::RSA::PrivateKey>;
using eax_optional = std::optional< CryptoPP::SecByteBlock>;
using rsa_eax_pair = std::pair< rsa_public_optional, eax_optional>;
using pk_t_keys_map = std::unordered_map< pk_t, rsa_eax_pair>;




#endif //PROGRAM_GLOBALUSING_H
