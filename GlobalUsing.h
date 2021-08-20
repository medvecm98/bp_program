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

using pk_t = std::size_t;
using seq_t = std::size_t;

using symmetric_cipher = CryptoPP::EAX< CryptoPP::AES>;
using asymmetric_cipher = CryptoPP::RSA;
using signer_verifier = CryptoPP::RSASS< CryptoPP::PSSR, CryptoPP::SHA256>;

using my_string = std::string;
using hash_t = std::size_t;
using level_t = std::uint32_t;
using user_container = std::unordered_set<pk_t>;
using user_container_citer = user_container::const_iterator;
using user_level_map = std::unordered_map<pk_t, level_t>;
using user_level_map_citer = user_level_map::const_iterator;

using optional_my_string = std::optional<my_string>;

using user_variant = std::variant<user_container_citer, user_level_map_citer>;

using public_private_key_pair = std::pair<CryptoPP::RSA::PublicKey, CryptoPP::RSA::PrivateKey>;

#endif //PROGRAM_GLOBALUSING_H
