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

private:
    CryptoPP::AutoSeededRandomPool prng;

    CryptoUtils() {}
    ~CryptoUtils() {}
};

#endif //NPPS_CRYPTO_UTILS
