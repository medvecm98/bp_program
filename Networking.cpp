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
	// tcp_socket->disconnectFromHost();
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
	stun_header_ptr new_stun_message = std::make_shared<StunMessageHeader>();
	networking_->get_stun_client()->create_request_send(new_stun_message, msg, to);
	networking_->get_stun_client()->send_stun_message(
		new_stun_message, networking_->ip_map_.get_wrapper_for_pk(to)->second.preferred_stun_server
	);
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
	std::size_t to_write = block.size();
	std::size_t written = 0;
	while (written < to_write) {
		written += tcp_socket->write(block);
		if (written < to_write) {
			tcp_socket->waitForBytesWritten();
		}
	}
	std::cout << "Bytes written: " << written << std::endl;
}

bool Networking::enroll_message_to_be_sent(shared_ptr_message message) {
	emit new_message_enrolled(std::move(message));
    return true;
}

void Networking::generate_rsa_key_pair() {
	auto pair = CryptoUtils::instance().generate_rsa_pair();

	ip_map_.private_rsa = {pair.second};
	ip_map_.my_ip().key_pair.first = {pair.first};
}

shared_ptr_message Networking::sign_and_encrypt_key(CryptoPP::ByteQueue& key_queue, pk_t sender, pk_t receiver) {
	if (!ip_map_.private_rsa.has_value() || !ip_map_.my_ip().key_pair.first.has_value()) {
		generate_rsa_key_pair(); //generated RSA keys, if they weren't before
	}
	auto& private_rsa = ip_map_.private_rsa;
	auto public_rsa = ip_map_.get_wrapper_ref(receiver).get_rsa_optional();

	std::string signature_str = CryptoUtils::instance().sign_key(
		key_queue,
		private_rsa
	);

	std::string encrypted_key = CryptoUtils::instance().encrypt_key(
		key_queue,
		public_rsa
	);

	/* creates the message to send */

	shared_ptr_message send_msg = std::make_shared<np2ps::Message>();
	send_msg->set_from(sender);
	send_msg->set_to(receiver);
	send_msg->set_msg_ctx(np2ps::ONE_WAY);
	send_msg->set_msg_type(np2ps::SYMMETRIC_KEY);

	send_msg->mutable_symmetric_key()->set_key(encrypted_key);
	send_msg->mutable_symmetric_key()->set_signature(signature_str);

	return send_msg;
}

void Networking::identify_peer_save_message(shared_ptr_message msg) {
	std::cout << "requesting IP and public key for pid: " << msg->to() << std::endl;
	stun_client->identify(msg->to()); //use STUN for IP and RSA public
	waiting_ip.emplace(msg->to(), msg); //store message for later, when IP and RSA will arrive
	std::cout << "User " << msg->to() << " was not found in IPMap" << std::endl;
}

void print_message(shared_ptr_message msg, std::string recv_send) {
	std::cout << "Printing message (" << recv_send << "):" << std::endl;

	if (recv_send == "receiving") {
		std::cout << " Sender:" << std::endl;
		std::cout << "  " << msg->from() << std::endl;
	}
	else if (recv_send == "sending") {
		std::cout << " Receiver: " << std::endl;
		std::cout << "  " << msg->to() << std::endl;
	}

	std::cout << " Context:" << std::endl;

	switch(msg->msg_ctx()) {
		case np2ps::REQUEST:
			std::cout << "  request" << std::endl;
			break;
		case np2ps::RESPONSE:
			std::cout << "  response" << std::endl;
			break;
		case np2ps::ONE_WAY:
			std::cout << "  one way" << std::endl;
			break;
		case np2ps::ERROR:
			std::cout << "  error" << std::endl;
			break;
		default:
			std::cout << "  <<unknown context>>" << std::endl;
			break;
	}

	std::cout << " Type:" << std::endl;

	switch(msg->msg_type()) {
		case np2ps::ARTICLE_ALL:
			std::cout << "  article all " << msg->article_all().header().heading() << " " << msg->article_all().header().main_hash() << std::endl;
			break;
		case np2ps::ARTICLE_LIST:
			std::cout << "  article list" << std::endl;
			break;
		case np2ps::ARTICLE_HEADER:
			std::cout << "  article header" << std::endl;
			break;
		case np2ps::SYMMETRIC_KEY:
			std::cout << "  symmetric key" << std::endl;
			break;
		case np2ps::CREDENTIALS:
			std::cout << "  creds" << std::endl;
			break;
		case np2ps::NEWSPAPER_LIST:
			std::cout << "  news list" << std::endl;
			break;
		case np2ps::ARTICLE_DATA_UPDATE:
			std::cout << "  article data update" << std::endl;
			break;
		case np2ps::USER_INFO:
			std::cout << "  user info" << std::flush;
			switch(msg->user_info().method()) {
				case np2ps::ADVERT_UI:
					std::cout << " advert" << std::endl;
					break;
				case np2ps::REQUEST_UI:
					std::cout << " request" << std::endl;
					break;
				case np2ps::RESPONSE_UI:
					std::cout << " response" << std::endl;
					break;
				default:
					break;
			}
			break;
		case np2ps::NEW_JOURNALIST:
			std::cout << "  new journalist" << std::endl;
			break;
		case np2ps::JOURNALIST:
			std::cout << "  journalist" << std::endl;
			break;
		case np2ps::GOSSIP:
			std::cout << "  gossip" << std::endl;
			break;
		default:
			std::cout << "  <<unknown type>>" << std::endl;
			break;
	}

}

void Networking::send_message(shared_ptr_message msg) {
	send_message_with_credentials(msg, false);
}

void Networking::send_message_with_credentials(shared_ptr_message msg, bool send_credentials) {
	try {
		IpWrapper& ipw = ip_map().get_wrapper_ref(msg->to());

		std::cout << "User " << msg->to() << " found in IP MAP database; Networking::send_message(shared_ptr_message msg)" << std::endl;

		if (!ipw.has_ipv4()) { //no IP
			identify_peer_save_message(msg);
			return;
		}

		if (msg->msg_type() == np2ps::PUBLIC_KEY) {  // I'm requesting public key
			std::cout << "	PUBLIC_KEY type of message" << std::endl;
			sender_->message_send(std::move(msg), ipw);
			return;
		}

		if (!ipw.has_rsa()) { // no public key
			shared_ptr_message credentials_msg = 
				MFW::ReqCredentialsFactory(
					MFW::CredentialsFactory(
						msg->from(), msg->to()
					),
					false, false, true, true, 0,
					{}, {}, ip_map_.my_ip().get_rsa(), {}, {}
				);
			messages_waiting_for_credentials.emplace(msg->to(), msg); //save message for when exchange is finished

			credentials_msg->mutable_credentials()->set_req_eax_key(true);
			credentials_msg->mutable_credentials()->set_req_rsa_public_key(true);

			sender_->message_send(std::move(credentials_msg), ipw);
			return;
		}

		if (!ipw.has_eax()) {
			// if (!send_credentials) {
			// 	shared_ptr_message key_msg = generate_symmetric_key_message(msg);
			// 	waiting_symmetric_exchange.emplace(msg->to(), msg);
			// 	sender_->message_send(key_msg, ipw);
			// 	return;
			// }
			// else {
			// 	// shared_ptr_message credentials_msg =
			// 	shared_ptr_message key_msg = generate_symmetric_key_message(msg);
			// 	waiting_symmetric_exchange.emplace(msg->to(), msg);
			// 	shared_ptr_message creds = MFW::CredentialsFactory(peer_public_id, msg->to());

			// 	creds->set_msg_ctx(np2ps::ONE_WAY);
			// 	creds->mutable_credentials()->mutable_rsa_public_key()->set_key(
			// 		ip_map().my_ip().get_rsa_hex_string()
			// 	);
			// 	creds->mutable_credentials()->mutable_eax_key()->set_key(
			// 		key_msg->symmetric_key().key()
			// 	);
			// 	creds->mutable_credentials()->mutable_eax_key()->set_signature(
			// 		key_msg->symmetric_key().signature()
			// 	);
			// 	creds->mutable_credentials()->set_method(1);

			// 	sender_->message_send(creds, ipw);
			// 	return;
			// }
			messages_waiting_for_credentials.emplace(msg->to(), msg);
			shared_ptr_message credentials_msg = 
				MFW::ReqCredentialsFactory(
					MFW::CredentialsFactory(
						msg->from(), msg->to()
					),
					false, false, false, true, 0,
					{}, {}, ip_map_.my_ip().get_rsa(), {}, {}
				);
			sender_->message_send(credentials_msg, ipw);
			return;
		}

		std::cout << "	Message ready to be normally sent." << std::endl;
		sender_->message_send(std::move(msg), ipw);
	} catch (user_not_found_in_database& e) {
		identify_peer_save_message(msg);
	}
}

/**
 * @brief Checks if IP is in IP map and if not, it will be added to it.
 * 
 * @param tcp_socket Socket for remote endpoint address (sender IP).
 * @param pk_id Public identifier of user.
 * @param ip_map_ IP map of where to check.
 */
void check_ip(QTcpSocket* tcp_socket, pk_t pk_id, IpMap& ip_map_, bool ignore_existing = false) {
	try {
		if (ignore_existing || !ip_map_.have_ip4(pk_id)) {
			std::cout << "Updating NP2PS IP: " << tcp_socket->peerAddress().toString().toStdString() << ", port: " << tcp_socket->peerPort() << std::endl;
			ip_map_.update_ip(pk_id, tcp_socket->peerAddress(), tcp_socket->peerPort());
		}
	}
	catch(user_not_found_in_database& unf) {
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
	std::string iv_str = s_msg.mid(4).toStdString();
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
	std::cout << "  byte array: " << s_msg.size() << std::endl;
	std::string s_msg_string = s_msg.mid(4).toStdString();
	std::cout << "  string: " << s_msg_string.size() << std::endl;
	return s_msg_string;
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
		if (!tcp_server_->listen(QHostAddress::AnyIPv4, PORT)) {
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

shared_ptr_message decrypt_message_using_symmetric_key(
	std::string e_msg,
	CryptoPP::SecByteBlock iv,
	IpWrapper& ipw,
	networking_ptr networking,
	QTcpSocket* socket
) {
	symmetric_cipher::Decryption dec;

	//symmetric key present
	CryptoPP::SecByteBlock byte_block = CryptoUtils::instance().byte_queue_to_sec_byte_block(ipw.get_eax());

	std::cout << "Decrypting with symmetric key: " << CryptoUtils::instance().bq_to_hex(ipw.get_eax()) << std::endl;

	dec.SetKeyWithIV(byte_block, byte_block.size(), iv);
	std::string dec_msg; //decrypted message
	CryptoPP::StringSource s(
		e_msg,
		true,
		new StringDecoder(
			new CryptoPP::AuthenticatedDecryptionFilter (
				dec,
				new CryptoPP::StringSink(
					dec_msg
				)
			)
		)
	);

	//deserialize

	shared_ptr_message m = std::make_shared<proto_message>();
	m->ParseFromString(dec_msg);
	print_message(m, "receiving");

	return std::move(m); //message now can be read in GPB format

}

void PeerReceiver::prepare_for_message_receive() {
	QTcpSocket* tcp_socket_ = tcp_server_->nextPendingConnection(); //get the socket for incoming connection
	tcp_socket_->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
	QObject::connect(tcp_socket_, &QIODevice::readyRead, this, &PeerReceiver::message_receive_connected);
	QObject::connect(tcp_socket_, &QAbstractSocket::errorOccurred, this, &PeerReceiver::display_error);
	QObject::connect(tcp_socket_, &QAbstractSocket::disconnected, tcp_socket_, &QObject::deleteLater);
	QObject::connect(tcp_socket_, &QAbstractSocket::disconnected, networking_.get(), &Networking::peer_process_disconnected_users);
}

void PeerReceiver::message_receive_connected() {
	QTcpSocket* tcp_socket_ = (QTcpSocket*)QObject::sender();
	in_.setDevice(tcp_socket_);
	in_.setVersion(QDataStream::Qt_5_0);
	message_receive(tcp_socket_);
}

void PeerReceiver::message_receive(QTcpSocket* tcp_socket_) {
	in_.startTransaction();

	process_received_np2ps_message(in_, tcp_socket_);
}

QByteArray read_bytes_socket(QTcpSocket* socket, qint64 bytes) {
	QByteArray rv;
	while (socket->bytesAvailable() < bytes) {
		if (!socket->waitForReadyRead()) {}
	}
	rv = socket->read(bytes);
	return rv;
}

void write_encrypted_message(QDataStream& length_plus_msg, shared_ptr_message msg, 
						  const std::string& iv_str, const std::string& encrypted_msg) 
{
	QByteArray iv_byte_array = QByteArray::fromStdString(iv_str);
	QByteArray encrypted_msg_byte_array = QByteArray::fromStdString(encrypted_msg);
	length_plus_msg << VERSION;
	length_plus_msg << ENCRYPTED_MESSAGE;
	length_plus_msg << (quint64)msg->from(); //public identifier won't be encrypted
	length_plus_msg << (quint64)iv_byte_array.size() << iv_byte_array;
	length_plus_msg << (quint64)encrypted_msg_byte_array.size() << encrypted_msg_byte_array; //initialization vector is written after size, but before message itself
}

void write_plain_message(QDataStream& length_plus_msg, const std::string& serialized_msg) {
	QByteArray msg_array = QByteArray::fromStdString(serialized_msg);
	length_plus_msg << VERSION;
	length_plus_msg << PLAIN_MESSAGE;
	length_plus_msg << (quint64)msg_array.size() << msg_array;
}

template<typename T>
void socket_wait_for_read(QTcpSocket* np2ps_socket) {
	while(np2ps_socket->bytesAvailable() < sizeof(T)) {
		np2ps_socket->waitForReadyRead();
	}
}

void PeerReceiver::process_received_np2ps_message(QDataStream& msg, QTcpSocket* np2ps_socket) {
	do {
		socket_wait_for_read<quint16>(np2ps_socket);
		quint16 msg_version;
		msg >> msg_version;

		if (msg_version != VERSION) { //check version
			throw other_error("Version mismatch");
		}

		socket_wait_for_read<quint16>(np2ps_socket);
		quint16 msg_class;
		msg >> msg_class;

		if (msg_class == ENCRYPTED_MESSAGE) {
			socket_wait_for_read<quint64>(np2ps_socket);
			quint64 pid;
			msg >> pid; //public identifier

			socket_wait_for_read<quint64>(np2ps_socket);
			quint64 iv_size;
			msg >> iv_size;

			QByteArray iv_array;
			qint64 read_size = 0;
			while (read_size < iv_size) {
				iv_array += np2ps_socket->read(iv_size + 4);
				read_size = iv_array.size();
				if (read_size < iv_size) {
					np2ps_socket->waitForReadyRead();
				}
			}

			auto iv = extract_init_vector(iv_array); //init. vector

			socket_wait_for_read<quint64>(np2ps_socket);
			quint64 msg_size;
			msg >> msg_size;

			std::cout << "Received message payload size: " << msg_size << std::endl;
			read_size = 0;
			QByteArray msg_array;
			while (read_size < msg_size) {
				msg_array += np2ps_socket->read(msg_size - read_size + 4);
				read_size = msg_array.size();
				if (read_size < msg_size) {
					np2ps_socket->waitForReadyRead();
				}
			}
			
			// msg.commitTransaction();

			auto e_msg = extract_encrypted_message(msg_array); //encrypted message
			
			//decrypt
			auto& [ipw_pk, ipw] = *(networking_->ip_map_.get_wrapper_for_pk(pid));
			if (ipw.has_eax()) {

				auto message = decrypt_message_using_symmetric_key(e_msg, iv, ipw, networking_, np2ps_socket);

				bool updated_socket = false;
				networking_->ip_map_.enroll_new_np2ps_tcp_socket(pid, np2ps_socket, &updated_socket); //enroll NP2PS socket for peers, that are not enrolled yet
				//we can check now, if the IP of sender is already in database and if not, we will add it
				check_ip(np2ps_socket, pid, networking_->ip_map_, updated_socket);

				networking_->add_to_received(message);
			}
			else {
				bool updated_socket = false;
				networking_->ip_map_.enroll_new_np2ps_tcp_socket(pid, np2ps_socket, &updated_socket); //enroll NP2PS socket for peers, that are not enrolled yet
				//we can check now, if the IP of sender is already in database and if not, we will add it
				check_ip(np2ps_socket, pid, networking_->ip_map_, updated_socket);
				
				//common symmetric key for given sender isn't stored locally yet
				
				std::cout << "Symmetric key missing for " << pid << "!!!" << std::endl;

				auto cred_req = MFW::ReqCredentialsFactory(
					MFW::CredentialsFactory(
						networking_->ip_map_.my_public_id,
						pid
					),
					false, false, false, true, 0,
					{}, {}, {}, {}, {}
				);


				//message will now wait until symmetric key is received
				networking_->add_to_messages_to_decrypt(pid, EncryptedMessageWrapper(e_msg, iv, pid, ENCRYPTED_MESSAGE));

				std::string cred_req_msg = cred_req->SerializeAsString();

				QByteArray msg_key_sstream_block;
				QDataStream msg_key_sstream(&msg_key_sstream_block, QIODevice::ReadWrite);
				msg_key_sstream.setVersion(QDataStream::Qt_5_0);
				write_plain_message(msg_key_sstream, cred_req_msg);
				send_message_using_socket(np2ps_socket, msg_key_sstream_block);

				//tcp_socket_->disconnectFromHost();
			}

			
		}
		else if (msg_class == PLAIN_MESSAGE) {
			auto m = std::make_shared<proto_message>();

			socket_wait_for_read<quint64>(np2ps_socket);
			quint64 msg_size;
			msg >> msg_size;

			qint64 read_size = 0;
			QByteArray msg_array;
			while (read_size < msg_size) {
				msg_array += np2ps_socket->read(4 + msg_size);
				read_size = msg_array.size();
				if (read_size < msg_size) {
					np2ps_socket->waitForReadyRead();
				}
			}

			

			m->ParseFromString(msg_array.mid(4).toStdString());
			print_message(m, "receiving");

			if (m->msg_type() == np2ps::SYMMETRIC_KEY) {
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
			}
			else if (m->msg_type() == np2ps::CREDENTIALS) {
				networking_->add_to_ip_map(m->from(), np2ps_socket->peerAddress());
				networking_->ip_map_.enroll_new_np2ps_tcp_socket(m->from(), np2ps_socket); //enroll NP2PS socket for peers, that are not enrolled yet
				networking_->add_to_received(std::move(m));
			}
			else {
				throw other_error("Public key should be exchanged in credentials or using STUN.");
			}
		}
		std::cout << "\nRecived message consumed." << std::endl;
	} while (np2ps_socket->bytesAvailable() > 0);
	msg.commitTransaction();
	std::cout << "All subsequent messages consumed.\n" << std::endl;
}

PeerSender::PeerSender(networking_ptr net) {
	networking_ = net;
	tcp_socket_ = new QTcpSocket();
	QObject::connect(tcp_socket_, &QAbstractSocket::errorOccurred, this, &PeerSender::display_error);
}

void PeerSender::try_connect(shared_ptr_message msg, IpWrapper& connectee) {
	if (connectee.np2ps_tcp_socket_ && connectee.np2ps_tcp_socket_->isValid()) { //try, if connection isn't already established
		message_send(connectee.np2ps_tcp_socket_, msg, connectee, false);
	}
	else if (connectee.get_relay_flag()) { //if not, is relay flag set? If yes, relay the message
		message_send(NULL, msg, connectee, true);
	}
	else {
		if (connectee.np2ps_tcp_socket_ && !connectee.np2ps_tcp_socket_->isValid()) {
			std::cout << "Invalid NP2PS socket" << std::endl;
		}
		QTcpSocket* socket_ = new QTcpSocket(this);

		QObject::connect(socket_, &QAbstractSocket::connected, this, &PeerSender::host_connected);
		QObject::connect(socket_, &QAbstractSocket::errorOccurred, this, &PeerSender::handle_connection_error);
		QObject::connect(socket_, &QAbstractSocket::disconnected, socket_, &QObject::deleteLater);
		QObject::connect(socket_, &QAbstractSocket::readyRead, networking_->get_peer_receiver(), &PeerReceiver::message_receive_connected);

		message_waiting_for_connection = msg;
		connectee_waiting_for_connection = connectee;

		std::cout << "Connecting to host: " << connectee.ipv4.toString().toStdString() << " and port " << connectee.port << std::endl;  //try to connect to peer directly
		socket_->connectToHost(connectee.ipv4, connectee.port);
	}
}

void PeerSender::host_connected() {
	std::cout << "Host successfuly connected:" << std::endl; //connection to host was successful
	QTcpSocket* socket_ = (QTcpSocket*)QObject::sender();
	std::cout << '\t' << socket_->peerAddress().toString().toStdString() << ' ' << socket_->peerPort() << std::endl;

	auto mtemp = message_waiting_for_connection;
	auto itemp = connectee_waiting_for_connection;

	message_waiting_for_connection.reset();
	connectee_waiting_for_connection = IpWrapper();

	networking_->ip_map().enroll_new_np2ps_tcp_socket(mtemp->to(), socket_);
	message_send(socket_, mtemp, itemp, false); //send message directly
}

void PeerSender::handle_connection_error() {

	QTcpSocket* socket_ = (QTcpSocket*)QObject::sender();

	if (socket_->error() == QAbstractSocket::SocketError::ConnectionRefusedError || 
		socket_->error() == QAbstractSocket::SocketError::SocketTimeoutError) {
		std::cout << "Host connection failed" << std::endl; //connection failed, try relaying
		auto mtemp = message_waiting_for_connection;
		auto itemp = connectee_waiting_for_connection;

		message_waiting_for_connection.reset();
		connectee_waiting_for_connection = IpWrapper();

		if (itemp.port != 14128) {
			message_send(socket_, mtemp, itemp, true); //relay
		}
		else {
			std::cout << "Connection to NP2PS server failed for id " << mtemp->to() << std::endl;
		}
	}
}

void encrypt_message_symmetrically(
	eax_optional& key_queue,
	const CryptoPP::SecByteBlock& iv,
	const std::string& serialized_msg,
	std::string& encrypted_msg
) {
	using namespace CryptoPP;
	symmetric_cipher::Encryption enc;

	auto key = CryptoUtils::instance().byte_queue_to_sec_byte_block(key_queue.value());

	std::cout << "Encrypting with key: " << CryptoUtils::instance().bq_to_hex(key_queue.value()) << std::endl;

	//encrypt message
	enc.SetKeyWithIV(key, key.size(), iv);
	StringSource s(
		serialized_msg,
		true,
		new AuthenticatedEncryptionFilter(
			enc,
			new StringEncoder(
				new StringSink(
					encrypted_msg
				)
			)
		)
	);
}

std::string create_iv_string(CryptoPP::SecByteBlock& iv) {
	return std::string(reinterpret_cast<const char*>(&iv[0]), iv.size());
}



void PeerSender::message_send(QTcpSocket* socket, shared_ptr_message msg, IpWrapper ipw, bool relay = false) {
	//serialize message
    std::string serialized_msg;
	msg->SerializeToString(&serialized_msg);

	QByteArray length_plus_msg_block;
	QDataStream length_plus_msg(&length_plus_msg_block, QIODevice::ReadWrite);
	length_plus_msg.setVersion(QDataStream::Qt_5_0);
	if (msg->msg_type() == np2ps::PUBLIC_KEY ||
		msg->msg_type() == np2ps::SYMMETRIC_KEY ||
		msg->msg_type() == np2ps::CREDENTIALS) 
	{
		write_plain_message(length_plus_msg, serialized_msg);
	}
	else {
		std::string encrypted_msg;

		//encrypt message
		auto& prng = prng_;
		CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
		prng.GenerateBlock(iv, iv.size());

		encrypt_message_symmetrically(ipw.get_eax_optional(), iv, serialized_msg, encrypted_msg);

		//we will create the initialization vector (iv) string
		std::string iv_str = create_iv_string(iv);
		write_encrypted_message(length_plus_msg, msg, iv_str, encrypted_msg);
	}

	

    //send message
	if (!relay)
		send_message_using_socket( socket, length_plus_msg_block);
	else
		send_message_using_turn(networking_, length_plus_msg_block, msg->to(), ipw);
}

void PeerSender::message_send(shared_ptr_message msg, IpWrapper& ipw) {
	print_message(msg, "sending");
	try_connect(msg, ipw);
}

void Networking::decrypt_encrypted_messages(pk_t original_sender) {
	auto [emb, eme] = waiting_decrypt.equal_range(original_sender);
	auto& [ipw_pk, ipw] = *(ip_map_.get_wrapper_for_pk(original_sender));
	for (; emb != eme; emb++) {
		auto message = decrypt_message_using_symmetric_key(
			emb->second.encrypted_message_or_symmetric_key,
			emb->second.initialization_vector_or_signature,
			ipw,
			shared_from_this(),
			NULL
		);
		add_to_received(message);
	}
}

void Networking::symmetric_exchanged(pk_t other_peer) {
	auto [mapib, mapie] = waiting_symmetric_exchange.equal_range(other_peer);
	for (; mapib != mapie; mapib++) {
		auto msg_ptr = mapib->second;
		waiting_symmetric_exchange.erase(mapib->first);
		enroll_message_to_be_sent(msg_ptr); //enroll all messages for that given peer
		break;
	}
}

void Networking::set_peer_public_id(pk_t pid) {
	peer_public_id = pid;
	ip_map_.my_public_id = pid;
}

pk_t Networking::get_peer_public_id() {
	return peer_public_id;
}

void Networking::peer_process_disconnected_users() {
	QTcpSocket* socket = (QTcpSocket*) QObject::sender();
	std::vector<pk_t> to_remove;
	ip_map_.remove_disconnected_users(to_remove, socket);
	pk_t peer_id;

	if (!to_remove.empty()) {
		for (auto&& user : to_remove) { //remove disconnected users
			IpWrapper& disconnected_user = ip_map_.get_wrapper_ref(user);
			disconnected_readers_lazy_remove.users.emplace(user);
			disconnected_user.clean_np2ps_socket();
			disconnected_user.clean_turn_socket();
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

void Networking::start_servers_with_first_ip() {
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
			receiver_->start_server(QHostAddress(ipAddressesList.at(i)));
			stun_server->start_server(QHostAddress(ipAddressesList.at(i)));
			break;
		}
	}
}

bool Networking::add_to_ip_map(pk_t id, QHostAddress&& address) {
	IpWrapper wrapper(address);
	return ip_map().add_to_map(id, wrapper);
}

eax_optional Networking::get_or_create_eax(shared_ptr_message msg) {
	auto eax_opt = ip_map().get_eax(msg->from());

	if (eax_opt.has_value()) {
		return eax_opt;
	}

	save_symmetric_key(msg->from(), generate_symmetric_key());

	return ip_map().get_eax(msg->from());
}

IpWrapper& Networking::save_symmetric_key(pk_t save_to, CryptoPP::ByteQueue&& aes_key) {
	auto wrapper = ip_map().get_wrapper_for_pk(save_to);
	if (wrapper != ip_map_.get_map_end()) {
		wrapper->second.add_eax_key(std::move(aes_key));
	}
	else {
		throw user_not_found_in_database("When adding created EAX, user was not found.");
	}
	return wrapper->second;
}

CryptoPP::ByteQueue Networking::generate_symmetric_key() {
	CryptoPP::SecByteBlock aes_key(CryptoPP::AES::DEFAULT_KEYLENGTH);
	prng_.GenerateBlock(aes_key, aes_key.size());
	return CryptoUtils::instance().sec_byte_block_to_byte_queue(aes_key);
}

/**
 * Generates symmetric key. Returns already-sendable message.
*/
shared_ptr_message Networking::generate_symmetric_key_message(shared_ptr_message msg) {
	auto wrapper = save_symmetric_key(msg->to(), generate_symmetric_key());

	shared_ptr_message key_message = sign_and_encrypt_key(wrapper.get_eax(), msg->from(), msg->to());

	return key_message;
}

void Networking::add_stun_server(pk_t pid) {
	stun_client->add_stun_server(pid);
}

void Networking::add_stun_server(pk_t pid, IpWrapper& wrapper) {
	ip_map().add_or_update_to_ip_map(pid, wrapper);
	add_stun_server(pid);
}
