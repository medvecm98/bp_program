#ifndef PROGRAM_GLOBALUSING_H
#define PROGRAM_GLOBALUSING_H

#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <optional>
#include <variant>

#include "cryptopp/rsa.h"
#include "cryptopp/rijndael.h"
#include "cryptopp/eax.h"
#include "cryptopp/aes.h"
#include "cryptopp/pssr.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/shared_ptr_helper.hpp>
#include <boost/serialization/variant.hpp>

#include <boost/serialization/shared_ptr_helper.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/tracking.hpp>

#include <cstdlib>

using pk_t = std::size_t;
using level_t = std::uint32_t;

struct PeerInfo {
	PeerInfo() = default;
	explicit PeerInfo(pk_t pk) {
		peer_key = pk;
		peer_level = 0;
	}
	PeerInfo(pk_t pk, level_t level) {
		peer_key = pk;
		peer_level = level;
	}
	pk_t peer_key = 0;
	level_t peer_level = 0;
	/* timestamp of info */
	
	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & peer_key;
		ar & peer_level;
	}
};

using seq_t = std::size_t;

using symmetric_cipher = CryptoPP::EAX< CryptoPP::AES>;
using asymmetric_cipher = CryptoPP::RSA;
using signer_verifier = CryptoPP::RSASS< CryptoPP::PSSR, CryptoPP::SHA256>;

using my_string = std::string;
using hash_t = std::size_t;
using user_container = std::unordered_set<pk_t>;
using user_container_citer = user_container::const_iterator;
using user_level_map = std::unordered_map<pk_t, PeerInfo>;
using user_level_map_citer = user_level_map::const_iterator;

using optional_my_string = std::optional<my_string>;

struct GU {
    static std::string get_program_home() {
    #if defined(_WIN32) 
        return std::getenv("LOCALAPPDATA");
    #else
        if (const char* env_p = std::getenv("XDG_DATA_HOME"))
            return std::string(env_p).append("/news_p2p_sharing");
        else
            return std::string(std::getenv("HOME")).append(".local/share/news_p2p_sharing");
    #endif
    }
};




#endif //PROGRAM_GLOBALUSING_H
