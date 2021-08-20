#include "Networking.h"

bool Networking::enroll_message_to_be_sent(unique_ptr_message message) {
    to_send_msg.push(std::move(message));
    return true;
}

void Networking::generate_rsa_key_pair() {
	CryptoPP::RSA::PrivateKey private_key;
	private_key.GenerateRandomWithKeySize(prng_, 3072);

	CryptoPP::RSA::PublicKey public_key(private_key);

	ip_map_.private_rsa = {private_key};
	ip_map_.my_ip.key_pair.first = {public_key};
}

/**
 * @brief Signs then encrypts given EAX + AES key.
 * 
 * Output is written into `std::stringstream`. Public and private keys will be generated, if weren't before.
 * Protobuf is used as usual. Byte with value `20` is prepended, indicating a message containing symmetric key.
 * 
 * @param output Where to write the message.
 * @param key Key to sign and encrypt.
 * @param sender Sender of the message, creator of the symmetric key.
 * @param receiver Receiver of the message.
 */
void Networking::sign_and_encrypt_key(std::stringstream& output, CryptoPP::SecByteBlock& key, pk_t sender, pk_t receiver) {
	if (!ip_map_.private_rsa.has_value() || !ip_map_.my_ip.key_pair.first.has_value()) {
		generate_rsa_key_pair();
	}

	auto& private_rsa = ip_map_.private_rsa.value();
	auto& public_rsa = ip_map_.my_ip.key_pair.first.value();

	signer_verifier::Signer signer(private_rsa);

	std::size_t length = signer.MaxSignatureLength();
	CryptoPP::SecByteBlock signature(length);
	length = signer.SignMessage(
		prng_,
		key.data(),
		key.size(),
		signature
	);
	signature.resize(length);

	auto& [ipw_pk, ipw] = *(ip_map_.get_wrapper_for_pk(receiver));
	CryptoPP::RSAES_OAEP_SHA_Encryptor rsa_encryptor(ipw.key_pair.first.value()); //encrypt with recipient's public key
	std::string encrypted_key;

	CryptoPP::StringSource s(
		key.data(),
		key.size(),
		true,
		new CryptoPP::PK_EncryptorFilter(
			prng_,
			rsa_encryptor,
			new CryptoPP::StringSink(
				encrypted_key
			)
		)
	);

	std::string signature_str(reinterpret_cast<const char*>(&signature[0]), signature.size());

	np2ps::Message send_msg;
	send_msg.set_from(sender);
	send_msg.set_to(receiver);
	send_msg.set_msg_ctx(np2ps::ONE_WAY);
	send_msg.set_msg_type(np2ps::SYMMETRIC_KEY);

	send_msg.mutable_symmetric_key()->set_key(encrypted_key);
	send_msg.mutable_symmetric_key()->set_signature(signature_str);

	std::string send_msg_str = send_msg.SerializeAsString();
	output << (char)KEY_MESSAGE;
	output << std::setfill('0') << std::setw(16) << std::hex << send_msg_str.size(); //length of the message
	output << send_msg_str;
}

/**
 * @brief Sends message into the network using provided TCP socket.
 * 
 * Message is encrypted almost all the time with EAX and AES. The only exception is when EAX + AES key is being
 * exhanged, in which case the key is signed, and encrypted using RSA.
 * 
 * ```
 * First byte:
 * ...containing 10 -> Standard message
 * ...conatining 20 -> Key exchange message
 * ```
 * 
 * @param tcp_socket Socket to use when sending message.
 * @param msg Pointer to message to send.
 * @param ipw IpWrapper of the receiver of this message.
 */
void Networking::send_message(tcp::socket& tcp_socket, unique_ptr_message msg, IpWrapper& ipw) {
    //serialize message
    std::string serialized_msg, encrypted_msg;
	msg->SerializeToString(&serialized_msg);

	//encrypt message
	auto& prng = prng_;
	symmetric_cipher::Encryption enc;
	CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
	prng.GenerateBlock(iv, iv.size());

	// check if symmetric key exists for given receiver:
	if (!ipw.key_pair.second.has_value()) {
		//...no, and so symmetric key needs to be yet generated
		CryptoPP::SecByteBlock aes_key(CryptoPP::AES::DEFAULT_KEYLENGTH);
		prng.GenerateBlock(aes_key, aes_key.size());
		ipw.add_eax_key(std::move(aes_key));

		std::stringstream key_exchange_msg;
		sign_and_encrypt_key(key_exchange_msg, aes_key, msg->from(), msg->to());
		boost::asio::write( tcp_socket, boost::asio::buffer( key_exchange_msg.str(), key_exchange_msg.str().size()));
	}


	//encrypt message
	enc.SetKeyWithIV(ipw.key_pair.second.value(), ipw.key_pair.second.value().size(), iv);
	CryptoPP::StringSource s(
		serialized_msg,
		true,
		new CryptoPP::AuthenticatedEncryptionFilter(
			enc,
			new CryptoPP::StringSink(
				encrypted_msg
			)
		)
	);

	//we will create the initialization vector (iv) string
	std::string iv_str(reinterpret_cast<const char*>(&iv[0]), iv.size());

    //append length of message to the message, without the length itself, length of initialization vector needs to be accounted for as well
    std::stringstream length_plus_msg;
	length_plus_msg << (char)NORMAL_MESSAGE;
    length_plus_msg << std::setfill('0') << std::setw(16) << std::hex << (encrypted_msg.size() + CryptoPP::AES::BLOCKSIZE + sizeof(pk_t)); //length of the message
    length_plus_msg << std::setfill('0') << std::setw(16) << std::hex << msg->from(); //public identifier won't be encrypted
    length_plus_msg << iv_str << encrypted_msg; //initialization vector is written after size, but before message itself

    //send message
    boost::asio::write( tcp_socket, boost::asio::buffer( length_plus_msg.str(), length_plus_msg.str().size()));
}

/**
 * @brief Checks if IP is in IP map and if not, it will be added to it.
 * 
 * @param tcp_socket Socket for remote endpoint address (sender IP).
 * @param pk_id Public identifier of user.
 * @param ip_map_ IP map of where to check.
 */
void check_ip(tcp::socket& tcp_socket, pk_t pk_id, IpMap& ip_map_) {
	if (!ip_map_.have_ip(pk_id)) {
		ip_map_.update_ip(pk_id, tcp_socket.remote_endpoint().address().to_string());
	}
}

/**
 * @brief Reads class and length of message from given socket.
 * 
 * @param tcp_socket Socket to read from.
 * @return Message class and length pair.
 */
std::pair<int, std::size_t> read_class_and_length(tcp::socket& tcp_socket) {
	char recv_msg_len[16 + 1 + 1]; //length of length plus message class identifier plus NULL
	boost::asio::read( tcp_socket, boost::asio::buffer( recv_msg_len, 16));
	recv_msg_len[17] = '\0';

	//parse and return
	return {(int)recv_msg_len[0], std::stoll(std::string(recv_msg_len, 1, 17), nullptr, 16)};
}

std::string read_meta_message(std::size_t msg_len, tcp::socket& tcp_socket) {
	char msg[msg_len + 1];
	boost::asio::read( tcp_socket, boost::asio::buffer( msg, msg_len));
	msg[msg_len] = '\0';

	return std::string(msg);
}

/**
 * @brief Gets init vector from message which is in string form.
 * 
 * Won't change the message.
 * 
 * @param s_msg Message to get the init vector from, in string form.
 * @return Initialization vector CryptoPP::SecByteBlock.
 */
CryptoPP::SecByteBlock extract_init_vector(const std::string& s_msg) {
	std::string iv_str = s_msg.substr(0, 16);
	CryptoPP::SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(&iv_str[0]), iv_str.size());
	return std::move(iv);
}

/**
 * @brief Extract public identifier (of the sender) from meta-message.
 * 
 * @param s_msg Message to extract from.
 * @return Public identifier of sender.
 */
pk_t extract_public_identifier(const std::string& s_msg) {
	return std::stoll(s_msg.substr(16, 16), nullptr, 16);
}

/**
 * @brief Gets actual encrypted message from meta-message.
 * 
 * @param s_msg 
 * @return std::string 
 */
std::string extract_encrypted_message(const std::string& s_msg) {
	return s_msg.substr(32);
}

/**
 * @brief Receive message from the network and put it in the queue.
 * 
 * @param tcp_socket Socket to use for receiving.
 * @return Sequence number of received message, only if message was a request. Used to recycle `PeerSession` sessions. No value otherwise.
 */
std::optional<seq_t> Networking::receive_message(tcp::socket& tcp_socket) {
	//receive message length and class
	auto [msg_class, msg_len] = read_class_and_length(tcp_socket);

	//read method requires char array
	//receive message
	auto s_msg = read_meta_message(msg_len, tcp_socket);

	if (msg_class == NORMAL_MESSAGE) {
		auto iv = extract_init_vector(s_msg); //init. vector
		auto pk_str = extract_public_identifier(s_msg); //public identifier
		auto e_msg = extract_encrypted_message(s_msg); //encrypted message

		//we can check now, if the IP of sender is already in database and if not, we will add it
		check_ip(tcp_socket, pk_str, ip_map_);
		
		//decrypt
		symmetric_cipher::Decryption dec;
		auto& [ipw_pk, ipw] = *(ip_map_.get_wrapper_for_pk(pk_str));
		if (ipw.key_pair.second.has_value()) {
			//symmetric key present
			dec.SetKeyWithIV(ipw.key_pair.second.value(), ipw.key_pair.second.value().size(), iv);

			std::string dec_msg; //decrypted message

			CryptoPP::StringSource s(
				e_msg,
				true,
				new CryptoPP::AuthenticatedDecryptionFilter (
					dec,
					new CryptoPP::StringSink(
						dec_msg
					)
				)
			);

			//deserialize
			unique_ptr_message m = std::make_unique<proto_message>();
			m->ParseFromString(dec_msg);
			if (m->msg_ctx() == np2ps::REQUEST) {
				seq_t rv = m->seq();
				received_msg.push(std::move(m));
				return {rv};
			}
			else {
				received_msg.push(std::move(m));
				return {};
			}
		}
		else {
			//common symmetric key for given sender isn't stored locally yet
			send_message(
				tcp_socket,
				MFW::ReqCredentialsFactory(
					MFW::CredentialsFactory(
						ip_map_.my_public_id,
						pk_str
					),
					false, false, false, true,
					{}, {}, {}, {}
				),
				ipw
			);
			messages_to_decrypt.insert({pk_str, EncryptedMessageWrapper(e_msg, iv, pk_str, NORMAL_MESSAGE)});
			return {};
		}
	}
	else if (msg_class == KEY_MESSAGE) {
		auto m = std::make_unique<proto_message>();
		m->ParseFromString(s_msg.substr(16));
		check_ip(tcp_socket, m->from(), ip_map_);
		received_msg.push(std::move(m));
		return {};
	}
}