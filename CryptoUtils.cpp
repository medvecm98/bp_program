#include "CryptoUtils.hpp"

std::string CryptoUtils::sign_with_keys(
    rsa_private_optional rsa,
    const std::string& to_sign
) {
    using namespace CryptoPP;
    auto& private_rsa = rsa.value();

    signer_verifier::Signer signer(private_rsa);
    std::string signature;

    StringSource temp(
        to_sign,
        true,
        new SignerFilter(
            prng,
            signer,
            new HexEncoder(
                new StringSink(
                    signature
                )
            )
        )
    );

    return signature;
}

bool CryptoUtils::verify_signature_with_keys(
    rsa_public_optional rsa,
    const std::string& to_verify,
    const std::string& signature_to_verify_with
) {
    using namespace CryptoPP;
    auto& public_rsa = rsa.value();

    signer_verifier::Verifier verifier(public_rsa);

    std::string decoded_string;
    StringSource temp(
        signature_to_verify_with,
        true,
        new HexDecoder(
            new StringSink(
                decoded_string
            )
        )
    );

    bool result = false;
    StringSource temp2(
        decoded_string + to_verify,
        true,
        new SignatureVerificationFilter(
            verifier,
            new ArraySink(
                (byte*)&result,
                sizeof(result)
            )
        )
    );

    return result;
}
