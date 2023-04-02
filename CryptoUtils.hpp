#ifndef NPPS_CRYPTO_UTILS
#define NPPS_CRYPTO_UTILS

#include "GlobalUsing.h"

/**
 * Utilities for working with crypto++.
*/

struct CryptoUtils {
    CryptoUtils(const CryptoUtils&) = delete;

    CryptoUtils& operator= (const CryptoUtils&) = delete;

    static CryptoUtils& instance() {
        static CryptoUtils cu;
        return cu;
    }

    std::string sign_with_keys(
        rsa_private_optional rsa,
        const std::string& to_sign
    );

    bool verify_signature_with_keys(
        rsa_public_optional rsa,
        const std::string& to_verify,
        const std::string& signature_to_verify_with
    );

    rsa_pair generate_rsa_pair();

    CryptoPP::SecByteBlock byte_queue_to_sec_byte_block(
        CryptoPP::ByteQueue& queue
    );

    CryptoPP::ByteQueue sec_byte_block_to_byte_queue(
        CryptoPP::SecByteBlock& byte_block
    );

    void decrypt_symmetric_key(
        std::string& enc_key,
        CryptoPP::ByteQueue& dec_key,
        CryptoPP::RSA::PrivateKey& rsa_private
    );

    bool verify_decrypt_symmetric_key(
        std::string& enc_key,
        std::string& signature,
        CryptoPP::ByteQueue& dec_key,
        rsa_public_optional& rsa_public,
        rsa_private_optional& rsa_private
    );

    std::string sign_key(
        CryptoPP::ByteQueue& to_sign,
        rsa_private_optional& rsa_private
    );

    std::string encrypt_key(
        CryptoPP::ByteQueue& to_encrypt,
        rsa_public_optional& rsa_public
    );

    std::string rsa_to_hex(
        const CryptoPP::RSA::PublicKey& rsa_public
    );

    CryptoPP::RSA::PublicKey hex_to_rsa(
        const std::string& hex_rsa_public
    );

    std::string bq_to_hex(
        CryptoPP::ByteQueue& queue
    );

    CryptoPP::ByteQueue hex_to_bq(
        const std::string& input
    );

    bool verify_key(
        rsa_public_optional rsa,
        CryptoPP::ByteQueue& to_verify,
        const std::string& signature_to_verify_with_encoded
    );

private:
    CryptoPP::AutoSeededRandomPool prng;

    CryptoUtils() {}
    ~CryptoUtils() {}
};

#endif //NPPS_CRYPTO_UTILS
