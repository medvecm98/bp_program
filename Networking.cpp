#include "Networking.h"

void send_message_using_socket(QTcpSocket* tcp_socket, const std::string& msg) {
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);
	out << QString(msg.c_str());
	tcp_socket->write(block);
	tcp_socket->disconnectFromHost();
}

void send_message_using_socket(QTcpSocket* tcp_socket, std::string&& msg) {
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);
	out << QString(msg.c_str());
	tcp_socket->write(block);
	tcp_socket->disconnectFromHost();
}

void send_message_using_socket(QTcpSocket* tcp_socket, unique_ptr_message msg) {
	send_message_using_socket(tcp_socket, msg->SerializeAsString());
}

bool Networking::enroll_message_to_be_sent(unique_ptr_message message) {
    //to_send_msg.push(std::move(message));
	emit new_message_enrolled(std::move(message));
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
	//output << std::setfill('0') << std::setw(16) << std::hex << send_msg_str.size(); //length of the message
	output << send_msg_str;
}

void Networking::send_message_again_ip(pk_t message_originally_to) {

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
 * @param msg Pointer to message to send.
 */
void Networking::send_message(unique_ptr_message msg) {
    auto ip_map_iter = ip_map_.get_wrapper_for_pk(msg->to());
	if (ip_map_iter != ip_map_.get_map_end() && msg->msg_type() == np2ps::PUBLIC_KEY) {
		IpWrapper& ipw = ip_map_iter->second;
		sender_->message_send(std::move(msg), ipw);
	}
	else if (ip_map_iter != ip_map_.get_map_end() && ip_map_iter->second.key_pair.first.has_value()) {
		IpWrapper& ipw = ip_map_iter->second;
		//auto msg = std::move(to_send_msg.front());
		//to_send_msg.pop();
		sender_->message_send(std::move(msg), ipw);
	}
	else {
		//request IP and public key from authority
		auto news_end = news_db.cend();
		for (auto news_iter = news_db.cbegin(); news_iter != news_end; news_iter++) {
			std::shared_ptr<std::string> request_string;
			request_string->append("r");
			request_string->append(std::to_string(msg->to()));
			enroll_message_to_be_sent(
				MFW::ReqCredentialsFactory(
					MFW::CredentialsFactory(
						ip_map_.my_public_id,
						*(news_iter->second.get_first_authority())
					),
					true, true, true, false,
					{request_string}, {}, {}, {}
				)
			);
		}
		waiting_ip.insert({msg->to(), msg});
	}
}

/**
 * @brief Checks if IP is in IP map and if not, it will be added to it.
 * 
 * @param tcp_socket Socket for remote endpoint address (sender IP).
 * @param pk_id Public identifier of user.
 * @param ip_map_ IP map of where to check.
 */
void check_ip(QTcpSocket* tcp_socket, pk_t pk_id, IpMap& ip_map_) {
	if (!ip_map_.have_ip4(pk_id)) {
		ip_map_.update_ip(pk_id, tcp_socket->peerAddress());
	}
}

/**
 * @brief Extracts class from received data.
 * 
 * @param message Message in string.
 * @return QString Message class.
 */
int read_class_and_length(QString& message) {
	auto rv = message.left(1).toInt();
	message = message.mid(1);
	return rv;
}

/**
 * @brief Reads meta message form received data.
 * 
 * @param message Received data.
 * @return QString Meta message.
 */
QString read_meta_message(const QString& message) {
	return message.right(message.size() - 1);
}

/**
 * @brief Gets init vector from message which is in string form.
 * 
 * Won't change the message.
 * 
 * @param s_msg Message to get the init vector from, in QString form.
 * @return Initialization vector CryptoPP::SecByteBlock.
 */
CryptoPP::SecByteBlock extract_init_vector(const QString& s_msg) {
	std::string iv_str = s_msg.left(16).toStdString();
	CryptoPP::SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(&iv_str[0]), iv_str.size());
	return std::move(iv);
}

/**
 * @brief Extract public identifier (of the sender) from meta-message.
 * 
 * @param s_msg Message to extract from.
 * @return Public identifier of sender.
 */
pk_t extract_public_identifier(const QString& s_msg) {
	return (pk_t)s_msg.mid(16, 16).toULongLong();
}

/**
 * @brief Gets actual encrypted message from meta-message.
 * 
 * @param s_msg 
 * @return std::string 
 */
std::string extract_encrypted_message(const QString& s_msg) {
	return s_msg.mid(32).toStdString();
}

/**
 * @brief Receive message from the network and put it in the queue.
 * 
 * @param tcp_socket Socket to use for receiving.
 * @return Sequence number of received message, only if message was a request. Used to recycle `PeerSession` sessions. No value otherwise.
 */
std::optional<seq_t> Networking::receive_message(QTcpSocket* tcp_socket) {
	return {1};
}

void Networking::init_sender_receiver() {
	sender_ = std::make_shared<PeerSender>(shared_from_this());
	receiver_ = std::make_shared<PeerReceiver>(shared_from_this());
}

PeerReceiver::PeerReceiver(networking_ptr net) {
	tcp_server_ = new QTcpServer();
	networking_ = net;
	if (!tcp_server_->listen(networking_->ip_map_.my_ip.ipv4, 14128)) {
		QTextStream(stdout)
			<< "Failed to start the server "
			<< tcp_server_->errorString()
			<< '\n';
		tcp_server_->close();
		return;
	}
	
	QObject::connect(tcp_server_, &QTcpServer::newConnection, this, &PeerReceiver::prepare_for_message_receive);
	
	
}

void decrypt_message_using_symmetric_key(std::string e_msg, CryptoPP::SecByteBlock iv, IpWrapper& ipw, networking_ptr networking) {
	symmetric_cipher::Decryption dec;

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

	unique_ptr_message m = std::make_shared<proto_message>();
	m->ParseFromString(dec_msg);
	if (m->msg_ctx() == np2ps::REQUEST) {
		seq_t rv = m->seq();
		networking->add_to_received(std::move(m));
		return;
	}
	else {
		networking->add_to_received(std::move(m));
		return;
	}

}

void PeerReceiver::prepare_for_message_receive() {
	std::cout << "preparing for message receive" << std::endl;
	tcp_socket_ = tcp_server_->nextPendingConnection();
	tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
	in_.setDevice(tcp_socket_);
	in_.setVersion(QDataStream::Qt_5_0);
	QObject::connect(tcp_socket_, &QIODevice::readyRead, this, &PeerReceiver::message_receive);
	QObject::connect(tcp_socket_, &QAbstractSocket::disconnected, tcp_socket_, &QObject::deleteLater);
}

void PeerReceiver::message_receive() {
	std::cout << "Receiving message" << std::endl;
	in_.startTransaction();
	std::cout << "transaction started" << std::endl;

	QString msg;
	in_ >> msg;

	if (!in_.commitTransaction()) {
		//TODO: receiving failed
		std::cout << "Message receiving failed" << std::endl;
		tcp_socket_->disconnectFromHost();
		return;
	}

	std::cout << "Message read and received" << std::endl;
	int msg_class = read_class_and_length(msg);

	if (msg_class == NORMAL_MESSAGE) {
		auto iv = extract_init_vector(msg); //init. vector
		auto pk_str = extract_public_identifier(msg); //public identifier
		auto e_msg = extract_encrypted_message(msg); //encrypted message

		//we can check now, if the IP of sender is already in database and if not, we will add it
		check_ip(tcp_socket_, pk_str, networking_->ip_map_);
		
		//decrypt
		auto& [ipw_pk, ipw] = *(networking_->ip_map_.get_wrapper_for_pk(pk_str));
		if (ipw.key_pair.second.has_value()) {
			decrypt_message_using_symmetric_key(e_msg, iv, ipw, networking_);
		}
		else {
			//common symmetric key for given sender isn't stored locally yet
			send_message_using_socket(
				tcp_socket_,
				MFW::ReqCredentialsFactory(
					MFW::CredentialsFactory(
						networking_->ip_map_.my_public_id,
						pk_str
					),
					false, false, false, true,
					{}, {}, {}, {}
				)
			);
			//message will now wait until symmetric key is received
			networking_->add_to_messages_to_decrypt(pk_str, EncryptedMessageWrapper(e_msg, iv, pk_str, NORMAL_MESSAGE));
			tcp_socket_->disconnectFromHost();
			return;
		}
	}
	else if (msg_class == KEY_MESSAGE) {
		auto m = std::make_shared<proto_message>();
		m->ParseFromString(msg.mid(1).toStdString());
		check_ip(tcp_socket_, m->from(), networking_->ip_map_);
		networking_->add_to_received(std::move(m));
		tcp_socket_->disconnectFromHost();
		return; 
	}
}

PeerSender::PeerSender(networking_ptr net) {
	networking_ = net;
	tcp_socket_ = new QTcpSocket();
}

void PeerSender::message_send(unique_ptr_message msg, IpWrapper& ipw) {
	//serialize message
    std::string serialized_msg, encrypted_msg;
	msg->SerializeToString(&serialized_msg);

	tcp_socket_->abort();
	tcp_socket_->connectToHost(ipw.ipv4, 14128);

	//encrypt message
	auto& prng = prng_;
	symmetric_cipher::Encryption enc;
	CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
	prng.GenerateBlock(iv, iv.size());

	std::stringstream length_plus_msg;
	if (msg->msg_type() != np2ps::PUBLIC_KEY) {
		// check if symmetric key exists for given receiver:
		if (!ipw.key_pair.second.has_value()) {
			//...no, and so symmetric key needs to be yet generated
			CryptoPP::SecByteBlock aes_key(CryptoPP::AES::DEFAULT_KEYLENGTH);
			prng.GenerateBlock(aes_key, aes_key.size());
			ipw.add_eax_key(std::move(aes_key));

			std::stringstream key_exchange_msg;
			networking_->sign_and_encrypt_key(key_exchange_msg, aes_key, msg->from(), msg->to());
			send_message_using_socket(tcp_socket_, key_exchange_msg.str());
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


		length_plus_msg << (char)NORMAL_MESSAGE;
		length_plus_msg << std::setfill('0') << std::setw(16) << std::hex << msg->from(); //public identifier won't be encrypted
		length_plus_msg << iv_str << encrypted_msg; //initialization vector is written after size, but before message itself
	}
	else {
		length_plus_msg << (char)KEY_MESSAGE;
		length_plus_msg << serialized_msg;
	}

    //send message
	send_message_using_socket( tcp_socket_, length_plus_msg.str());
}

void Networking::decrypt_encrypted_messages(pk_t original_sender) {
	auto [emb, eme] = waiting_decrypt.equal_range(original_sender);
	auto& [ipw_pk, ipw] = *(ip_map_.get_wrapper_for_pk(original_sender));
	for (; emb != eme; emb++) {
		decrypt_message_using_symmetric_key(emb->second.encrypted_message_or_symmetric_key, emb->second.initialization_vector_or_signature, ipw, shared_from_this());
	}
}

void Networking::user_member_results(seq_t msg_seq, bool is_member) {
	auto waiting_level_i = waiting_level.find(msg_seq);
	if (waiting_level_i != waiting_level.end()) {
		if (is_member)
			enroll_message_to_be_sent(waiting_level_i->second);
		waiting_level.erase(waiting_level_i);
	}
}
