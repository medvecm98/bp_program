#include "CryptoUtils.hpp"

/**
 * @brief Sign key in string form and encode signature in hex-manner.
 * 
 * @param rsa Private key to sign with.
 * @param to_sign Key in string format to sign.
 * @return std::string Signature.
 */
std::string CryptoUtils::sign_with_keys(
    rsa_private_optional rsa,
    const std::string& to_sign
) {
    using namespace CryptoPP;
    auto& private_rsa = rsa.value();
    AutoSeededRandomPool prng;

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

/**
 * @brief Verify signature of provided string.
 * 
 * @param rsa Public key to use in verification.
 * @param to_verify What to verify in std::string form.
 * @param signature_to_verify_with_encoded Signature to use in verification, in hex-encoded form.
 * @return true Verification successful.
 * @return false Verification failed.
 */
bool CryptoUtils::verify_signature_with_keys(
    rsa_public_optional rsa,
    const std::string& to_verify,
    const std::string& signature_to_verify_with_encoded
) {
    using namespace CryptoPP;
    auto& public_rsa = rsa.value();

    signer_verifier::Verifier verifier(public_rsa);

    std::string signature_to_verify_with_decoded;
    StringSource temp1(
        signature_to_verify_with_encoded,
        new HexDecoder(
            new StringSink(
                signature_to_verify_with_decoded
            )
        )
    );

    std::string decoded_string;
    try {
        StringSource temp2(
            to_verify + signature_to_verify_with_decoded,
            new HexDecoder(
                new SignatureVerificationFilter(
                    verifier,
                    NULL,
                    SignatureVerificationFilter::THROW_EXCEPTION
                )
            )
        );
    }
    catch (...) {
        return false;
    }

    return true;
}

bool CryptoUtils::verify_key(
    rsa_public_optional rsa,
    CryptoPP::ByteQueue& to_verify,
    const std::string& signature_to_verify_with_encoded
) {
    using namespace CryptoPP;
    std::string hex_key;
    HexEncoder encoder(
        new StringSink(hex_key)
    );
    to_verify.CopyTo(encoder);
    return verify_signature_with_keys(rsa, hex_key, signature_to_verify_with_encoded);
}

rsa_pair CryptoUtils::generate_rsa_pair() {
    std::cout << "NEW RSA GENERATION" << std::endl;
    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::RSA::PrivateKey private_key;
	private_key.GenerateRandomWithKeySize(prng, 3072);

	CryptoPP::RSA::PublicKey public_key(private_key);
    return {public_key, private_key};
}

CryptoPP::SecByteBlock CryptoUtils::byte_queue_to_sec_byte_block(
    CryptoPP::ByteQueue& queue
) {
    std::size_t contiguousSize = queue.CurrentSize();
    return CryptoPP::SecByteBlock(queue.Spy(contiguousSize), contiguousSize);
}

CryptoPP::ByteQueue CryptoUtils::sec_byte_block_to_byte_queue(
    CryptoPP::SecByteBlock& byte_block
) {
    CryptoPP::ByteQueue queue;
    queue.Put(byte_block.BytePtr(), byte_block.SizeInBytes());
    return queue;
}

void CryptoUtils::decrypt_symmetric_key(
    std::string& enc_key,
    CryptoPP::ByteQueue& dec_key,
    CryptoPP::RSA::PublicKey& rsa_public,
    CryptoPP::RSA::PrivateKey& rsa_private
) {
    using namespace CryptoPP;

    AutoSeededRandomPool rng;
	rsa_encryptor_decryptor::Decryptor rsa_decryptor(
        rsa_private
    );

	StringSource ss0(
		enc_key,
		true,
		new HexDecoder(
            new PK_DecryptorFilter(
                rng,
                rsa_decryptor,
                new Redirector(
                    dec_key
                )
            )
		)
	);

    std::cout << "Decrypted symmetric key: " << CryptoUtils::instance().bq_to_hex(dec_key) << std::endl;
}

bool CryptoUtils::verify_decrypt_symmetric_key(
    std::string& enc_key,
    std::string& signature,
    CryptoPP::ByteQueue& dec_key,
    rsa_public_optional& rsa_public,
    rsa_private_optional& rsa_private
) {
    decrypt_symmetric_key(
        enc_key,
        dec_key,
        rsa_public.value(),
        rsa_private.value()
    );

    return verify_signature_with_keys(
        rsa_public,
        bq_to_hex(dec_key),
        signature
    );
}

std::string CryptoUtils::sign_key(
    CryptoPP::ByteQueue& to_sign,
    rsa_private_optional& rsa_private
) {
    using namespace CryptoPP;
    std::string key;
    HexEncoder encoder(
        new StringSink(key)
    );
    to_sign.CopyTo(encoder);
    auto signature = sign_with_keys(rsa_private, key);
    return signature;
}

std::string CryptoUtils::encrypt_key(
    CryptoPP::ByteQueue& to_encrypt,  
    rsa_public_optional& rsa_public
) {
    using namespace CryptoPP;

    AutoSeededRandomPool rng;
    std::cout << "Encrypting key: " << CryptoUtils::instance().bq_to_hex(to_encrypt) << std::endl;

    //encrypt with recipient's public key
    rsa_encryptor_decryptor::Encryptor rsa_encryptor(rsa_public.value());
    
    std::string encrypted_key;
    CryptoPP::PK_EncryptorFilter filter(
        rng,
        rsa_encryptor,
        new CryptoPP::HexEncoder (
            new CryptoPP::StringSink(
                encrypted_key
            )
        )
    );
    to_encrypt.CopyTo(filter);
    filter.MessageEnd();

    return encrypted_key;
}

std::string CryptoUtils::rsa_to_hex(
    const CryptoPP::RSA::PublicKey& key
) {
    using namespace CryptoPP;

    ByteQueue queue;
    key.DEREncodePublicKey(queue);

    HexEncoder encoder;
    queue.CopyTo(encoder);
    encoder.MessageEnd();

    std::string out;
    StringSink ss(out);
    encoder.CopyTo(ss);

    return out;
}

CryptoPP::RSA::PublicKey CryptoUtils::hex_to_rsa(
    const std::string& hex_rsa_public
) {
    using namespace CryptoPP;

    ByteQueue queue;
    HexDecoder decoder(new Redirector(queue));
    decoder.Put((const byte*)hex_rsa_public.data(), hex_rsa_public.size());
    decoder.MessageEnd();

    RSA::PublicKey key;
    key.BERDecodePublicKey(queue, false, queue.MaxRetrievable());

    return key;
}

std::string CryptoUtils::bq_to_hex(
    CryptoPP::ByteQueue& queue
) {
    using namespace CryptoPP;

    std::string out;
    HexEncoder hex_encoder(new StringSink(out));
    queue.CopyTo(hex_encoder);

    return out;
}

CryptoPP::ByteQueue CryptoUtils::hex_to_bq(
    const std::string& input
) {
    using namespace CryptoPP;

    ByteQueue queue;
    StringSource temp(
        input,
        true,
        new HexDecoder (
            new Redirector(
                queue
            )
        )
    );
    // queue.MessageEnd();

    return queue;
}
