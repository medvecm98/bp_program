#include "Networking.h"

/**
 * @brief Send the message using provided TCP socket.
 * 
 * No more modification is made to the message. It is sent as-is.
 * 
 * @param tcp_socket Socket to use.
 * @param msg Message in `std::string` form.
 */
void send_message_using_socket(QTcpSocket* tcp_socket, const std::string& msg) {
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_0);
	QString qs = QString::fromStdString(msg);
	out << qs;
	tcp_socket->write(block);
	tcp_socket->disconnectFromHost();
}

/**
 * @brief Sends message to the peer relayed via STUN.
 * 
 * @param networking_ Networking class pointer, for access to IpMap.
 * @param msg Message to sent.
 * @param to Whom to relay the message.
 * @param ipw IpWrapper of the receiver.
 */
void send_message_using_turn(networking_ptr networking_, QByteArray& msg, pk_t to, IpWrapper& ipw) {
	stun_header_ptr m = std::make_shared<StunMessageHeader>();
	networking_->get_stun_client()->create_request_send(m, msg, to);
	networking_->get_stun_client()->send_stun_message(m, networking_->ip_map_.get_wrapper_for_pk(to)->second.preferred_stun_server);
}

/**
 * @brief Send the message using provided TCP socket.
 * 
 * No more modification is made to the message. It is sent as-is.
 * 
 * @param tcp_socket Socket to use.
 * @param block Message in QByteArray form.
 */
void send_message_using_socket(QTcpSocket* tcp_socket, QByteArray& block) {
	tcp_socket->write(block);
	
}

bool Networking::enroll_message_to_be_sent(unique_ptr_message message) {
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


void Networking::sign_and_encrypt_key(QDataStream& output, CryptoPP::SecByteBlock& key, pk_t sender, pk_t receiver) {
	if (!ip_map_.private_rsa.has_value() || !ip_map_.my_ip.key_pair.first.has_value()) {
		generate_rsa_key_pair(); //generated RSA kes, if they weren't before
	}

	auto& private_rsa = ip_map_.private_rsa.value();
	auto& public_rsa = ip_map_.my_ip.key_pair.first.value();

	signer_verifier::Signer signer(private_rsa);

	std::size_t length = signer.MaxSignatureLength();
	CryptoPP::SecByteBlock signature(length);
	length = signer.SignMessage( //sign the message
		prng_,
		key.data(),
		key.size(),
		signature
	);
	signature.resize(length);

	auto& [ipw_pk, ipw] = *(ip_map_.get_wrapper_for_pk(receiver));
	rsa_encryptor_decryptor::Encryptor rsa_encryptor(ipw.key_pair.first.value()); //encrypt with recipient's public key
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

	/* creates the message to send */

	np2ps::Message send_msg;
	send_msg.set_from(sender);
	send_msg.set_to(receiver);
	send_msg.set_msg_ctx(np2ps::ONE_WAY);
	send_msg.set_msg_type(np2ps::SYMMETRIC_KEY);

	send_msg.mutable_symmetric_key()->set_key(encrypted_key);
	send_msg.mutable_symmetric_key()->set_signature(signature_str);

	std::string send_msg_str = send_msg.SerializeAsString();
	quint64 msg_size = send_msg_str.size();

	output << VERSION;
	output << KEY_MESSAGE;
	output << msg_size;
	//output << std::setfill('0') << std::setw(16) << std::hex << send_msg_str.size(); //length of the message
	QByteArray send_msg_str_arr = QByteArray::fromStdString(send_msg_str); 
	output << send_msg_str_arr;
}

void Networking::send_message(unique_ptr_message msg) {
	

    auto ip_map_iter = ip_map_.get_wrapper_for_pk(msg->to());
	if (ip_map_iter != ip_map_.get_map_end() && msg->msg_type() == np2ps::PUBLIC_KEY) { //user is in IpMap database, and NP2PS message is of type PUBLIC_KEY
		IpWrapper& ipw = ip_map_iter->second;
		sender_->message_send(std::move(msg), ipw);
	}
	else if (ip_map_iter != ip_map_.get_map_end() && ip_map_iter->second.key_pair.first.has_value()) {//user is in IpMap database, and we have his public key
		IpWrapper& ipw = ip_map_iter->second;
		sender_->message_send(std::move(msg), ipw);
	}
	else { //user was not found in IpMap and we need its IP and RSA public
		std::cout << "requesting IP and public key for pid: " << msg->to() << std::endl;
		stun_client->identify(msg->to()); //use STUN for IP and RSA public
		waiting_ip.emplace(msg->to(), msg); //store message for later, when IP and RSA will arrive
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
		std::cout << "Updating NP2PS IP: " << tcp_socket->peerAddress().toString().toStdString() << ", port: " << tcp_socket->peerPort() << std::endl;
		ip_map_.update_ip(pk_id, tcp_socket->peerAddress(), tcp_socket->peerPort());
	}
}

/**
 * @brief Extracts class from received data.
 * 
 * @param message Message in string.
 * @return QString Message class.
 */
char read_class_and_length(QByteArray& message) {
	char rv = message[0];
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
CryptoPP::SecByteBlock extract_init_vector(const QByteArray& s_msg) {
	std::string iv_str = s_msg.toStdString();
	CryptoPP::SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(&iv_str[0]), iv_str.size());
	return std::move(iv);
}

/**
 * @brief Extract public identifier (of the sender) from meta-message.
 * 
 * @param s_msg Message to extract from.
 * @return Public identifier of sender.
 */
pk_t extract_public_identifier(const QByteArray& s_msg) {
	return (pk_t)s_msg.left(16).toULongLong(nullptr, 16);
}

/**
 * @brief Gets actual encrypted message from meta-message.
 * 
 * @param s_msg 
 * @return std::string 
 */
std::string extract_encrypted_message(const QByteArray& s_msg) {
	return s_msg.toStdString();
}

/**
 * @brief Initializes PeerSenver and Receiver
 * 
 * @param nd 
 */
void Networking::init_sender_receiver(news_database* nd) {
	if (!sender_receiver_initialized) {
		sender_ = std::make_shared<PeerSender>(shared_from_this());
		receiver_ = std::make_shared<PeerReceiver>(shared_from_this());
		news_db = nd;
		sender_receiver_initialized = true;
	}
}

PeerReceiver::PeerReceiver(networking_ptr net) : networking_(net) {}

void PeerReceiver::start_server(QHostAddress address) {
	if (!server_started) {
		tcp_server_ = new QTcpServer(this);
		if (!tcp_server_->listen(address, PORT)) {
			QTextStream(stdout)
					<< "Failed to start the server "
					<< tcp_server_->errorString()
					<< '\n';
				tcp_server_->close();
				return;
		}
		else {
			QTextStream(stdout) << "NP2PS server listening on: " << address.toString() << " and port: " << PORT << '\n';
		}
		QObject::connect(tcp_server_, &QTcpServer::newConnection, this, &PeerReceiver::prepare_for_message_receive);
		server_started = true;
	}
}

void decrypt_message_using_symmetric_key(std::string e_msg, CryptoPP::SecByteBlock iv, IpWrapper& ipw, networking_ptr networking, QTcpSocket* socket) {
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
		networking->add_to_received(std::move(m)); //message now can be read in GPB format
		return;
	}
	else {
		networking->add_to_received(std::move(m));
		return;
	}

}

void PeerReceiver::prepare_for_message_receive() {
	tcp_socket_ = tcp_server_->nextPendingConnection(); //get the socket for incoming connection
	tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
	in_.setDevice(tcp_socket_);
	in_.setVersion(QDataStream::Qt_5_0);
	QObject::connect(tcp_socket_, &QIODevice::readyRead, this, &PeerReceiver::message_receive_connected);
	QObject::connect(tcp_socket_, &QAbstractSocket::errorOccurred, this, &PeerReceiver::display_error);

}

void PeerReceiver::message_receive_connected() {
	tcp_socket_ = (QTcpSocket*)QObject::sender();
	in_.setDevice(tcp_socket_);
	in_.setVersion(QDataStream::Qt_5_0);
	message_receive();
}

void PeerReceiver::message_receive() {
	in_.startTransaction();

	process_received_np2ps_message(in_, tcp_socket_);
}

void PeerReceiver::process_received_np2ps_message(QDataStream& msg, QTcpSocket* np2ps_socket) {

	quint16 msg_version;
	msg >> msg_version;

	if (msg_version != VERSION) { //check version
		std::cout << "Version mismatch" << std::endl;
		return;
	}

	quint16 msg_class;
	msg >> msg_class;

	if (msg_class == NORMAL_MESSAGE) {
		quint64 pk_str;
		msg >> pk_str; //public identifier

		networking_->ip_map_.enroll_new_np2ps_tcp_socket(pk_str, np2ps_socket); //enroll NP2PS socket for peers, that are not enrolled yet

		quint64 iv_size;
		msg >> iv_size;

		QByteArray iv_array;
		iv_array.resize(iv_size);
		msg >> iv_array;

		auto iv = extract_init_vector(iv_array); //init. vector

		quint64 msg_size;
		msg >> msg_size;
		
		QByteArray msg_array;
		msg_array.resize(msg_size);
		msg >> msg_array;

		auto e_msg = extract_encrypted_message(msg_array); //encrypted message
		
		//we can check now, if the IP of sender is already in database and if not, we will add it
		check_ip(tcp_socket_, pk_str, networking_->ip_map_);

		//decrypt
		auto& [ipw_pk, ipw] = *(networking_->ip_map_.get_wrapper_for_pk(pk_str));
		if (ipw.key_pair.second.has_value()) {
			decrypt_message_using_symmetric_key(e_msg, iv, ipw, networking_, np2ps_socket);
		}
		else {
			//common symmetric key for given sender isn't stored locally yet
			

			auto cred_req = MFW::ReqCredentialsFactory(
				MFW::CredentialsFactory(
					networking_->ip_map_.my_public_id,
					pk_str
				),
				false, false, false, true,
				{}, {}, {}, {}
			);


			//message will now wait until symmetric key is received
			networking_->add_to_messages_to_decrypt(pk_str, EncryptedMessageWrapper(e_msg, iv, pk_str, NORMAL_MESSAGE));

			std::string cred_req_msg = cred_req->SerializeAsString();

			QByteArray msg_key_sstream_block;
			QDataStream msg_key_sstream(&msg_key_sstream_block, QIODevice::ReadWrite);
			msg_key_sstream.setVersion(QDataStream::Qt_5_0);
			msg_key_sstream << VERSION;
			msg_key_sstream << KEY_MESSAGE;
			msg_key_sstream << (quint64)cred_req_msg.size();
			msg_key_sstream << QByteArray::fromStdString(cred_req_msg);
			send_message_using_socket(tcp_socket_, msg_key_sstream_block);

			//tcp_socket_->disconnectFromHost();
			msg.commitTransaction();
			return;
		}
	}
	else if (msg_class == KEY_MESSAGE) {
		auto m = std::make_shared<proto_message>();

		quint64 msg_size;
		msg >> msg_size;

		QByteArray msg_array;
		msg_array.resize(msg_size);
		msg >> msg_array;

		m->ParseFromString(msg_array.toStdString());
		if (networking_->ip_map_.have_rsa_public(m->from())) { //check if received symmetric key message is from someone we know
			std::cout << "RSA public found for " << m->from() << std::endl;
			check_ip(np2ps_socket, m->from(), networking_->ip_map_);
			networking_->ip_map_.enroll_new_np2ps_tcp_socket(m->from(), np2ps_socket);
			networking_->add_to_received(std::move(m));
		}
		else {
			std::cout << "No public key and or ipv4 found for " << m->from() << "; needs to be identified" << std::endl;
			networking_->waiting_symmetric_key_messages.emplace(m->from(), m); //we dont know the sender and so we needs to identify him first
			networking_->get_stun_client()->identify(m->from());
		}

		msg.commitTransaction();
		return; 
	}
}

PeerSender::PeerSender(networking_ptr net) {
	networking_ = net;
	tcp_socket_ = new QTcpSocket();
	QObject::connect(tcp_socket_, &QAbstractSocket::errorOccurred, this, &PeerSender::display_error);
}

void PeerSender::try_connect(unique_ptr_message msg, IpWrapper& ipw) {
	if (ipw.np2ps_tcp_socket_ && ipw.np2ps_tcp_socket_->isValid()) { //try, if connection isn't already established
		message_send(ipw.np2ps_tcp_socket_, msg, ipw, false);
	}
	else if (ipw.get_relay_flag()) { //if not, is relay flag set? If yes, relay the message
		message_send(NULL, msg, ipw, true);
	}
	else {
		QTcpSocket* socket_ = new QTcpSocket(this);

		QObject::connect(socket_, &QAbstractSocket::connected, this, &PeerSender::host_connected);
		QObject::connect(socket_, &QAbstractSocket::errorOccurred, this, &PeerSender::handle_connection_error);

		QObject::connect(socket_, &QAbstractSocket::readyRead, networking_->get_peer_receiver(), &PeerReceiver::message_receive_connected);

		message_waiting_for_connection = msg;
		ipw_waiting_for_connection = ipw;

		std::cout << "Connecting to host: " << ipw.ipv4.toString().toStdString() << " and port " << ipw.port << std::endl;  //try to connect to peer directly
		socket_->connectToHost(ipw.ipv4, ipw.port);
	}

}

void PeerSender::host_connected() {
	std::cout << "Host successfuly connected" << std::endl; //connection to host was successful
	QTcpSocket* socket_ = (QTcpSocket*)QObject::sender();
	std::cout << socket_->peerAddress().toString().toStdString() << ' ' << socket_->peerPort() << std::endl;

	auto mtemp = message_waiting_for_connection;
	auto itemp = ipw_waiting_for_connection;

	message_waiting_for_connection.reset();
	ipw_waiting_for_connection = IpWrapper();

	message_send(socket_, mtemp, itemp, false); //send message directly
}

void PeerSender::handle_connection_error() {

	QTcpSocket* socket_ = (QTcpSocket*)QObject::sender();

	if (socket_->error() == QAbstractSocket::SocketError::ConnectionRefusedError || 
		socket_->error() == QAbstractSocket::SocketError::SocketTimeoutError) {
		std::cout << "Host non-successfuly connected" << std::endl; //connection failed, try relaying
		auto mtemp = message_waiting_for_connection;
		auto itemp = ipw_waiting_for_connection;

		message_waiting_for_connection.reset();
		ipw_waiting_for_connection = IpWrapper();

		if (itemp.port != 14128) {
			message_send(socket_, mtemp, itemp, true); //relay
		}
		else {
			std::cout << "Connection to NP2PS server failed for id " << mtemp->to() << std::endl;
		}
	}
}

void PeerSender::message_send(QTcpSocket* socket, unique_ptr_message msg, IpWrapper ipw, bool relay = false) {
	//serialize message
    std::string serialized_msg, encrypted_msg;
	msg->SerializeToString(&serialized_msg);


	//encrypt message
	auto& prng = prng_;
	symmetric_cipher::Encryption enc;
	CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
	prng.GenerateBlock(iv, iv.size());

	QByteArray length_plus_msg_block;
	QDataStream length_plus_msg(&length_plus_msg_block, QIODevice::ReadWrite);
	length_plus_msg.setVersion(QDataStream::Qt_5_0);
	if (msg->msg_type() != np2ps::PUBLIC_KEY) {
		// check if symmetric key exists for given receiver:
		if (!ipw.key_pair.second.has_value()) {
			//...no, and so symmetric key needs to be yet generated
			CryptoPP::SecByteBlock aes_key(CryptoPP::AES::DEFAULT_KEYLENGTH);
			prng.GenerateBlock(aes_key, aes_key.size()); //generation
			auto wrapper = networking_->ip_map_.get_wrapper_for_pk(msg->to());
			if (wrapper != networking_->ip_map_.get_map_end()) {
				wrapper->second.add_eax_key(std::move(aes_key));
			}
			//ipw.add_eax_key(std::move(aes_key)); //adding to ip map

			QByteArray key_exchange_msg_block;
			QDataStream key_exchange_msg(&key_exchange_msg_block, QIODevice::ReadWrite);
			key_exchange_msg.setVersion(QDataStream::Qt_5_0);

			networking_->sign_and_encrypt_key(key_exchange_msg, aes_key, msg->from(), msg->to());

			if (!relay)
				send_message_using_socket(socket, key_exchange_msg_block);
			else
				send_message_using_turn(networking_, key_exchange_msg_block, msg->to(), ipw);

			networking_->waiting_symmetrich_exchange.insert({msg->to(), std::move(msg)});
			return;
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

		length_plus_msg << VERSION;
		length_plus_msg << NORMAL_MESSAGE;
		length_plus_msg << (quint64)msg->from(); //public identifier won't be encrypted
		length_plus_msg << (quint64)iv_str.size() << QByteArray::fromStdString(iv_str);
		length_plus_msg << (quint64)encrypted_msg.size() << QByteArray::fromStdString(encrypted_msg); //initialization vector is written after size, but before message itself
	}
	else {
		length_plus_msg << VERSION;
		length_plus_msg << KEY_MESSAGE;
		length_plus_msg << (quint64)serialized_msg.size();
		length_plus_msg << QByteArray::fromStdString(serialized_msg);
	}

    //send message
	if (!relay)
		send_message_using_socket( socket, length_plus_msg_block);
	else
		send_message_using_turn(networking_, length_plus_msg_block, msg->to(), ipw);
}

void PeerSender::message_send(unique_ptr_message msg, IpWrapper& ipw) {
	try_connect(msg, ipw);
}

void Networking::decrypt_encrypted_messages(pk_t original_sender) {
	auto [emb, eme] = waiting_decrypt.equal_range(original_sender);
	auto& [ipw_pk, ipw] = *(ip_map_.get_wrapper_for_pk(original_sender));
	for (; emb != eme; emb++) {
		decrypt_message_using_symmetric_key(emb->second.encrypted_message_or_symmetric_key, emb->second.initialization_vector_or_signature, ipw, shared_from_this(), NULL);
	}
}

void Networking::symmetric_exchanged(pk_t other_peer) {
	auto [mapib, mapie] = waiting_symmetrich_exchange.equal_range(other_peer);
	for (; mapib != mapie; mapib++) {
		auto msg_ptr = mapib->second;
		waiting_symmetrich_exchange.erase(mapib->first);
		enroll_message_to_be_sent(msg_ptr); //enroll all messages for that given peer
		break;
	}
}

void Networking::set_peer_public_id(pk_t pid) {
	peer_public_id = pid;
}

pk_t Networking::get_peer_public_id() {
	return peer_public_id;
}

void Networking::peer_process_disconnected_users() {
	std::vector<pk_t> to_remove;
	ip_map_.remove_disconnected_users(to_remove);
	pk_t peer_id;

	if (!to_remove.empty()) {
		for (auto&& user : to_remove) { //remove disconnected users
			for (auto it = readers_->begin(); it != readers_->end(); it++) {
				if (it->second->peer_key == user) {
					it = readers_->erase(it); 
				}
			}
			user_map->erase(user);
			ip_map_.remove_from_map(user);
			journalists_->erase(user);
		}
	}
	
}

void Networking::get_network_interfaces() {
	auto interfaces = QNetworkInterface::allInterfaces(); //get all interfaces, including localhost
	address_vec_ptr addresses_and_interfaces = std::make_shared<address_vec>();
	for (auto&& interface : interfaces) { 
		auto addresses = interface.allAddresses();
		for (auto&& address : addresses) {
			if (address.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol) { //get only IPv4 addresses
				addresses_and_interfaces->emplace_back(interface.humanReadableName(), address);
			}
		}
	}
	emit got_network_interfaces(addresses_and_interfaces);
}