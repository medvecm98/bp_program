#include "Peer.h"

/**
 * @brief Adds new article to its corresponding newspaper.
 * 
 * @param a Article to add.
 */
void Peer::enroll_new_article(Article article, bool header_only) {
	auto news_id = article.news_id();
	NewspaperEntry& news = get_news(news_id);
	if (article.get_ancestor() != 0) {
		news.remove_article(article.get_ancestor());
	}
	auto main_hash = article.main_hash();
	news_[news_id].add_article(main_hash, std::move(article));
	emit checked_display_article(news_id, main_hash);
	emit new_article_list(news_id);
}

void Peer::slot_dns_result(const QHostInfo& host) {
	std::string name = "";
	if (waiting_dns_newspapers.count(host.hostName()) > 0) {
		name = waiting_dns_newspapers.find(host.hostName())->second;
		waiting_dns_newspapers.erase(host.hostName());
	}
	if (host.error() != QHostInfo::NoError) {
		std::cout << "Newspaper URL: " << host.hostName().toStdString() << " failed to resolve."
		<< std::endl;
	}
	else {
		QHostAddress address = host.addresses()[0];
		identify_newspaper(address, PORT, STUN_PORT,name);
	}
}

void Peer::identify_newspaper(QString address, const std::string& newspaper_name) {
	QHostAddress address_normal(address);
	waiting_dns_newspapers.emplace(address, newspaper_name);
	QHostInfo::lookupHost(address, this, &Peer::slot_dns_result);
}

void Peer::identify_newspaper(QHostAddress address, port_t np2ps_port, port_t stun_port, const std::string &newspaper_name) {
	networking_->newspapers_awaiting_identification.emplace(
		address.toIPv4Address(),
		std::make_tuple(newspaper_name, np2ps_port, stun_port)
	);
	networking_->get_stun_client()->identify(address, stun_port);
}

void Peer::newspaper_identified(pk_t newspaper_pid, my_string newspaper_name, std::string newspaper_ip_domain, port_t np2ps_port, port_t stun_port) {
	add_new_newspaper(
		newspaper_pid,
		newspaper_name,
		QHostAddress(QString::fromStdString(newspaper_ip_domain)),
		np2ps_port,
		stun_port,
		true
	);
}

/**
 * Adds new newspaper to the collection.
 * @param newspaper_pid Public key of newspaper.
 * @param newspaper_ip_domain IP, or domain name, of the newspaper.
 */
void Peer::add_new_newspaper(pk_t newspaper_pid, const my_string& newspaper_name, const std::string &newspaper_ip_domain, bool allocate_now) {
	networking_->ip_map_.add_to_map(newspaper_pid, IpWrapper(QHostAddress(QString::fromStdString(newspaper_ip_domain))));

	auto news = NewspaperEntry(newspaper_pid, newspaper_pid, newspaper_name, &networking_->disconnected_readers_lazy_remove);
	news.await_confirmation = true;

	newspapers_awaiting_confirmation_.emplace(newspaper_pid, std::move(news));
	
	if (allocate_now) {
		networking_->get_stun_client()->allocate_request(newspaper_pid);
	}
}

/**
 * Adds new newspaper to the collection.
 * @param newspaper_pid Public key of newspaper.
 * @param newspaper_ip_domain IP of the newspaper.
 */
NewspaperEntry& Peer::add_new_newspaper(pk_t newspaper_pid, const my_string& newspaper_name, QHostAddress&& newspaper_ip_domain, port_t np2ps_port, port_t stun_port, bool allocate_now) {
	try{
		IpWrapper& wrapper = networking_->ip_map().get_wrapper_ref(newspaper_pid);
		wrapper.ipv4 = newspaper_ip_domain;
		wrapper.port = np2ps_port;
		wrapper.stun_port = stun_port;
	}
	catch(user_not_found_in_database e) {
		networking_->ip_map().add_to_map(newspaper_pid, IpWrapper(newspaper_ip_domain, np2ps_port, stun_port));
	}
	if (allocate_now) {
		networking_->get_stun_client()->allocate_request(newspaper_pid);
		NewspaperEntry& new_news = newspapers_awaiting_confirmation_.emplace(
			newspaper_pid,
			NewspaperEntry(
				newspaper_pid,
				newspaper_pid,
				newspaper_name,
				&networking_->disconnected_readers_lazy_remove
			)
		).first->second;
		return new_news;
	}
	else {
		return news_.emplace(newspaper_pid, NewspaperEntry(newspaper_pid, newspaper_pid, newspaper_name, &networking_->disconnected_readers_lazy_remove)).first->second;
	}
}

/**
 * Adds new newspaper to the collection.
 * @param newspaper_pid Public key of newspaper.
 * @param sender Sender of this NewspaperEntry.
 */
void Peer::add_new_newspaper(pk_t newspaper_pid, const my_string& newspaper_name, pk_t sender) {
	networking_->get_stun_client()->identify(newspaper_pid, sender);
	NewspaperEntry ne(newspaper_pid, newspaper_pid, newspaper_name, &networking_->disconnected_readers_lazy_remove);
	ne.add_reader(sender);
	newspapers_awaiting_confirmation_.emplace(newspaper_pid, std::move(ne));
}

NewspaperEntry& Peer::add_new_newspaper(NewspaperEntry&& newspaper_entry, QHostAddress&& address, port_t np2ps_port, port_t stun_port, bool allocate_now) {
	pk_t news_pid = newspaper_entry.get_id();
	networking_->ip_map().add_to_map(news_pid, IpWrapper(address, np2ps_port, stun_port));
	if (allocate_now) {
		networking_->get_stun_client()->allocate_request(news_pid);
		return newspapers_awaiting_confirmation_.emplace(
			news_pid,
			std::move(newspaper_entry)
		).first->second;
	}
	else {
		return news_.emplace(news_pid, std::move(newspaper_entry)).first->second;
	}
}

void Peer::newspaper_confirm(pk_t pid) {
	newspaper_confirm_public_key(pid, rsa_public_optional());
}

void Peer::newspaper_confirm_public_key(pk_t pid, rsa_public_optional public_key) {
	auto ne = newspapers_awaiting_confirmation_.find(pid);
	if (ne == newspapers_awaiting_confirmation_.end()) { //no newspaper were awaiting confirmation with this pid
		return;
	}

	try {
		NewspaperEntry& news = news_.at(pid);
		if (public_key.has_value()) {
			news.set_newspaper_public_key(public_key.value());
		}
		news.set_confirmation(true);
	}
	catch (std::out_of_range e) {
		NewspaperEntry& news = news_.emplace(pid, ne->second).first->second;
		if (public_key.has_value()) {
			news.set_newspaper_public_key(public_key.value());
		}
		news.set_confirmation(true);
	}
	
	newspapers_awaiting_confirmation_.erase(ne);
	emit got_newspaper_confirmation(pid);
}

/**
 * @brief Intialize a new newspaper.
 * 
 * Public ID is same as peer's.
 * 
 * @param name Name of the newspaper we want to create.
 */
void Peer::init_newspaper(my_string name) {
	newspaper_name_ = name;
	newspaper_id_ = public_identifier_;
	auto [news_db, temp] = news_.insert({newspaper_id_, NewspaperEntry(public_identifier_, newspaper_id_, newspaper_name_, &networking_->disconnected_readers_lazy_remove)}); //our news are in same db as all the others
	auto [public_key, private_key] = CryptoUtils::instance().generate_rsa_pair();
	news_db->second.set_newspaper_private_key(private_key);
	news_db->second.set_newspaper_public_key(public_key);
	news_db->second.emplace_journalist(get_public_id());
	journalist_of_.emplace(newspaper_id_);
}

/**
 * @brief Finds article in database. All newspaper are searched.
 * 
 * @param article_hash Hash of article we are looking for.
 * @return article_optional Article header of article we are looking for, no value otherwise.
 */
article_optional Peer::find_article_in_database(hash_t article_hash) {
	for(auto&& news : news_) {
		auto& [hash, news_entry] = news;
		auto search = news_entry.find_article_header(article_hash);
		if (search.has_value()) {
			return {search.value()};
		}
	}
	return {};
}

//TODO: try to implement this as extensible as possible, e. g. using POLICIES
/**
 * @brief List all articles made by this Peer.
 * 
 * Filters out unwanted categories. May or may not set more than one newspapers.
 * 
 * @param articles Where to put listed articles.
 * @param categories Category filter.
 * @param news_id ID of newspaper to use. If zero, all newspapers will be used.
 * @return Number of articles listed. 
 */
size_t Peer::list_all_articles_by_me(std::set<Article*> &articles, const std::set<category_t> &categories, pk_t news_id) {
	size_t article_counter = 0;
	std::function<news_database::iterator()> news_functor;
	AllTheNews al(news_.begin(), news_.end());
	TheSameNews ts;

	news_database::iterator news;
	if (news_id == 0) {
		news_functor = al;
	}
	else {
		news = news_.find(news_id);
		if (news == news_.end()) {
			return 0;
		}
		ts.entry = news;
		ts.end_iterator = news_.end();
		news_functor = ts;
	}

	for (auto news_iterator = news_functor(); news_iterator != news_.end(); news_iterator = news_functor()) {
		/* iterate through one or all news */

		auto i_end = news_iterator->second.get_iterator_database_end();
		for (auto i = news_iterator->second.get_iterator_database(); i != i_end; i++) {
			if (!categories.empty()) {
				for (auto &&category : categories) {
					if ((i->second.author_id() == public_identifier_) && (categories.empty() || i->second.is_in_category(category))) {
						articles.insert(&(i->second));
						article_counter++;
						break;
					}
				}
			}
			else {
				if (i->second.author_id() == public_identifier_) {
					articles.insert(&(i->second));
					article_counter++;
				}
			}
		}
	}

	return article_counter;
}

/**
 * @brief List all articles made by this Peer.
 * 
 * All categories are used. May or may not set more than one newspapers.
 * 
 * @param articles Where to put listed articles.
 * @param news_id ID of newspaper to use. If zero, all newspapers will be used.
 * @return Number of articles listed. 
 */
size_t Peer::list_all_articles_by_me(article_container &articles, pk_t news_id) {

	return list_all_articles_by_me(articles, std::set<category_t>(), news_id);
}

/**
 * @brief Find article written (or catalogized) by me.
 * 
 * All newspapers are searched. First match returns. There should be no other matches, however.
 * 
 * @param article_hash 
 * @return article_optional 
 */
article_optional Peer::find_article(hash_t article_hash) {
	for (auto&& news : news_) {
		auto this_news_search = news.second.find_article_header(article_hash);
		if (this_news_search.has_value())
			return this_news_search;
	}

	return article_optional();
}

void Peer::create_margin_request(pk_t to, hash_t article_hash) {
	std::vector<Margin> vm;

	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextRequest(
			MFW::UpdateMarginFactory(
				public_identifier_,
				to,
				article_hash,
				vm
			)
		)
	);
}

/**
 * @brief Universal method to handle the message from the top of the message queue.
 * 
 * @param article_hash Shared pointer to message which needs to be handled.
 */
void Peer::handle_message(shared_ptr_message message) {
	std::cout << "Peer: Handling message from " << message->from() << std::endl;
	if (message->msg_ctx() == np2ps::REQUEST) {
		std::cout << "Peer: Handling request from " << message->from() << std::endl;
		handle_requests( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::RESPONSE) {
		std::cout << "Peer: Handling response from " << message->from() << std::endl;
		handle_responses( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::ONE_WAY) {
		std::cout << "Peer: Handling one way from " << message->from() << std::endl;
		handle_one_way( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::ERROR) {
		std::cout << "Peer: Handling error from " << message->from() << std::endl;
		handle_error( std::move( message));
	}
	else {
		std::cout << "message unknown" << std::endl;
	}
}

/**
 * @brief Handles messages of context "request".
 * 
 * Further flow is determined by the type of the message. Almost every one of
 * these flow paths will generate another message that is later on sent via network.
 * 
 * @param message Shared pointer to message (request) which needs to be handled. 
 */
void Peer::handle_requests(shared_ptr_message message) {
	auto type = message->msg_type();

	if (type == np2ps::ARTICLE_ALL) {
		handle_article_all_request(message);
	}
	else if (type == np2ps::ARTICLE_HEADER) {
		handle_article_header_request(message);
	}
	else if (type == np2ps::ARTICLE_LIST) {
		handle_article_list_request(message);
	}
	else if (type == np2ps::ARTICLE_DATA_UPDATE) {
		handle_article_data_update_request(message);
	}
	else if (type == np2ps::UPDATE_MARGIN) {
		handle_update_margin_request(message);
	}
	else if (type == np2ps::CREDENTIALS) {
		handle_credentials_request(message);
	}
	else if (type == np2ps::NEWSPAPER_ENTRY) {
		handle_newspaper_entry_request(message);
	}
	else if (type == np2ps::NEWSPAPER_LIST) {
		handle_newspaper_list_request(message);
	}
	else if (type == np2ps::JOURNALIST) {
		handle_journalist_request(message);
	}
	else if (type == np2ps::GOSSIP) {
		handle_gossip_request(message);
	}
	else {
		throw unsupported_message_type_in_context("Peer received a message type that is unsupported in given context");
	}
}

/**
 * @brief Handles messages of context "response".
 * 
 * Further flow is determined by the type of the message. Usually, no message is
 * sent via network.
 * 
 * @param message Shared pointer to message (request) which needs to be handled. 
 */
void Peer::handle_responses(shared_ptr_message message) {
	auto type = message->msg_type();

	if (type == np2ps::ARTICLE_ALL) {
		handle_article_all_response(message);
	}
	else if (type == np2ps::ARTICLE_LIST) {
		handle_article_list_response(message);
	}
	else if (type == np2ps::CREDENTIALS) {
		handle_credentials_response(message);
	}
	else if (type == np2ps::PUBLIC_KEY) {
		handle_public_key_response(message);
	}
	else if (type == np2ps::SYMMETRIC_KEY) {
		handle_symmetric_key_response(message);
	}
	else if (type == np2ps::UPDATE_MARGIN) {
		handle_update_margin_request(message);
	}
	else if (type == np2ps::NEWSPAPER_LIST) {
		handle_newspaper_list_response(message);
	}
	else if (type == np2ps::JOURNALIST) {
		handle_journalist_response(message);
	}
	else if (type == np2ps::GOSSIP) {
		handle_gossip_response(message);
	}
	else {
		throw unsupported_message_type_in_context("Peer received a message type that is unsupported in given context");
	}
}

void Peer::allocate_on_stun_server(pk_t target) {
	networking_->get_stun_client()->allocate_request(target);
}

pk_t Peer::check_destination_valid(pk_t destination, pk_t newspaper) {
	if (networking_->ip_map_.get_wrapper_for_pk(destination) == networking_->ip_map_.get_map_end()) {
		return newspaper;
	}
	return destination;
}

/**
 * @brief Article all message generator.
 * 
 * Generator for `np2ps::ARTICLE_ALL`. Will send the message via network.
 * 
 * @param news_id ID of newspaper this Article belongs to.
 * @param article_hash Article hash of desired article.
 */
void Peer::generate_article_all_message(pk_t news_id, hash_t article_hash) {
	downloading_articles.insert(article_hash);
	emit check_selected_item();

	auto& news = get_news(news_id);
	auto& article = news.get_article(article_hash);

	if (article.readers_count() > 0) {
		std::cout << "Asking " << article.readers_count() << " readers." << std::endl;
		for (auto&& reader_id : article.readers()) {
			if (reader_id == article.author_id() ||
				reader_id == public_identifier_
			) {
				continue;
			}
			try {
				auto& wrapper = get_networking()->ip_map().get_wrapper_ref(reader_id);
				if (!wrapper.np2ps_socket_connected() &&
					wrapper.relay_state == RelayState::Direct)
				{
					continue;
				}
				if (wrapper.relay_state == RelayState::Direct) {
					article.add_article_download(reader_id, true);
				}
				else {
					article.add_article_download(reader_id, false);
				}
			}
			catch(user_not_found_in_database e) {
				// continue;
				article.add_article_download(reader_id, false);
			}
		}
	}
	article.add_article_download(article.author_id(), false);

	article_all_send(article);
}

void Peer::article_all_send(Article& article) {
	int readers_asked_at_once = 2 < article.readers_count() ? 2 : article.readers_count();
	while (readers_asked_at_once-- > 0) {
		if (article.get_one_article_download() == 0) {
			break;
		}
		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextRequest(
				MFW::ArticleDownloadFactory(
					public_identifier_,
					article.remove_one_article_download(),
					&article
				)
			)
		);
	}
}

void Peer::article_all_send_more(Article& article) {
	int readers_asked_at_once = 2 < article.readers_count() ? 2 : article.readers_count();
	article.inc_failed_peers();
	if (article.failed_peers() < readers_asked_at_once) {
		return;
	}
	article.reset_failed_peers();
	while (readers_asked_at_once-- > 0) {
		if (article.get_one_article_download() == 0) {
			break;
		}
		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextRequest(
				MFW::ArticleDownloadFactory(
					public_identifier_,
					article.remove_one_article_download(),
					&article
				)
			)
		);
	}
}

void Peer::article_all_send_more_message(shared_ptr_message message)
{
	hash_t article_hash = message->article_all().article_hash();
	pk_t news_id = message->article_all().header().news_id();

	Article& article = get_news(news_id).get_article(article_hash);
	article_all_send_more(article);
}


/**
 * @brief Article header message generator.
 * 
 * Generator for `np2ps::ARTICLE_HEADER`.
 * 
 * @param destination Who is the receiver of the message.
 * @param article_hash Article hash of desired article header.
 */
void Peer::generate_article_header_message(pk_t destination, hash_t article_hash) {
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextRequest(
			MFW::ArticleHeaderFactory(
				public_identifier_,
				destination,
				article_hash
			)
		)
	);
}

/**
	 * @brief Generator for article list message.
	 * 
	 * @param newspaper_id ID of newspaper which article list we want.
	 */
void Peer::generate_article_list_message(pk_t newspaper_id, bool ignore_timestamp) {
	getting_article_list.insert(newspaper_id);
	emit check_selected_item();

	try {
		auto& news = get_news(newspaper_id);

		std::size_t list_size = config.list_size_default;

		if (news.get_article_count() == 0) {
			list_size = config.list_size_first;
		}

		networking_->enroll_message_to_be_sent(
			MFW::ReqArticleListFactory(
				MFW::ArticleListFactory(
					public_identifier_,
					newspaper_id
				),
				newspaper_id,
				list_size,
				news.last_updated(),
				std::vector<my_string>()
			)
		);

		for (auto&& user : news.get_journalists()) { //send the same request to journalists
			if (user == newspaper_id) {
				continue;
			}
			networking_->enroll_message_to_be_sent(
				MFW::ReqArticleListFactory(
					MFW::ArticleListFactory(
						public_identifier_,
						user
					),
					newspaper_id,
					list_size,
					news.last_updated(),
					std::vector<my_string>()
				)
			);	
		}

		news.randomize_readers();
		int random_count = 10;
		for (auto&& reader : news.get_readers()) {
			if (reader == public_identifier_) continue;
			if (random_count-- <= 0) break;
			if (reader == newspaper_id) continue;
			networking_->enroll_message_to_be_sent(
				MFW::ReqArticleListFactory(
					MFW::ArticleListFactory(
						public_identifier_,
						reader
					),
					newspaper_id,
					list_size,
					news.last_updated(),
					std::vector<my_string>()
				)
			);	
		}
	}
	catch (unknown_newspaper_error e) {

	}
}

void Peer::generate_article_list_message_all_news() {
	std::cout << "Auto updating news" << std::endl;
	for (auto&& [news_id, news] : news_) {
		if (news_id == get_public_id()) {
			continue;
		}
		std::cout << "  id: " << news_id << std::endl;
		generate_article_list_message(news_id, true);
	}
}

/**
 * @brief Handler for messages of context "one way".
 * 
 * Even though response to such message makes no sense, responses are made,
 * because it requires different handling that `requests`.
 * 
 * @param msg Shared pointer to `one way` message.
 */
void Peer::handle_one_way(shared_ptr_message msg) {
	auto type = msg->msg_type();
	if (type == np2ps::ARTICLE_SOLICITATION) {
		handle_article_solicitation_one_way(msg);
	}
	else if (type == np2ps::SYMMETRIC_KEY) {
		handle_symmetric_key_one_way(msg);
	}
	else if (type == np2ps::PUBLIC_KEY) {
		handle_public_key_one_way(msg);
	}
	else if (type == np2ps::ARTICLE_DATA_UPDATE) {
		handle_article_data_update_one_way(msg);
	}
	else if (type == np2ps::CREDENTIALS) {
		handle_credentials_one_way(msg);
	}
	else if (type == np2ps::USER_INFO) {
		handle_user_info_message_one_way(msg);
	}
	else if (type == np2ps::NEW_JOURNALIST) {
		handle_new_journalist_one_way(msg);
	}
	else if (type == np2ps::GOSSIP) {
		handle_gossip_one_way(msg);
	}
	else if (type == np2ps::ARTICLE_ALL) {
		handle_article_all_one_way(msg);
	}
	else if (type == np2ps::PING) {
		handle_ping_one_way(msg);
	}
	else {
		throw unsupported_message_type_in_context("Peer received a message type that is unsupported in given context");
	}
}

/**
 * @brief Handler for messages of context "error".
 * 
 * @param msg Shared pointer to the message.
 */
void Peer::handle_error(shared_ptr_message msg) {
	auto type = msg->msg_type();
	if (type == np2ps::ARTICLE_SOLICITATION) { //possible owner did not have the article
		handle_article_solicitation_error(msg);
	}
	else if (type == np2ps::ARTICLE_HEADER) {
		handle_article_header_error(msg);
	}
	else if (type == np2ps::ARTICLE_ALL) {
		handle_article_all_error(msg);
	}
	else if (type == np2ps::JOURNALIST) {
		handle_journalist_error(msg);
	}
}

news_database& Peer::get_news() {
	return news_;
}

NewspaperEntry& Peer::get_news(pk_t newspaper_id) {
	if (news_.find(newspaper_id) != news_.end()) {
		return news_.at(newspaper_id);
	}

	throw unknown_newspaper_error("Requested newspaper are not in database.");
}

void Peer::handle_article_all_request(shared_ptr_message message) {
	auto article_opt = find_article(message->article_all().article_hash());

	if (article_opt.has_value()) { //at least article header is in database
		Article* article = article_opt.value();
		
		NewspaperEntry& news = get_news(message->article_all().header().news_id());
		news.add_reader(message->from());

		if (article_opt.value()->article_present()) { //whole article is present in the database
			std::string article_whole;

			article_whole = article->read_contents();
			article->add_reader(message->from());

			shared_ptr_message article_msg = MFW::RespArticleDownloadFactory(
				MFW::ArticleDownloadFactory(
					public_identifier_, 
					message->from(), 
					article_opt.value()
				),
				article_opt.value(), 
				std::move(article_whole)
			);

			//send message
			networking_->enroll_message_to_be_sent(article_msg);
		}
		else { //only article header is present in article database
			networking_->enroll_message_to_be_sent(
				MFW::ErrorArticleDownloadFactory(
					MFW::ArticleDownloadFactory(
						message->from(),
						public_identifier_,
						article_opt.value()
					),
					article_opt.value()
				)
			);
		}
	}
	else {
		//article not found in database
		networking_->enroll_message_to_be_sent(
			MFW::ErrorArticleDownloadFactory(
				MFW::ArticleDownloadFactory(
					message->from(),
					public_identifier_,
					NULL
				),
				message->article_all().header().news_id()
			)
		);
	}
}

void Peer::handle_article_header_request(shared_ptr_message message) {
	if (message->has_article_header() && message->article_header().has_article()) {
		hash_t recv_article_hash = message->article_header().article_hash();
		if (journalists_.find(message->article_header().article().author_id()) != journalists_.end()) { //check if sender is a journalist for this newspaper
			Article recv_article(message->article_header().article());
			enroll_new_article(recv_article, true);
			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextResponse(
					MFW::ArticleHeaderFactory(
						public_identifier_,
						message->from(),
						recv_article_hash
					)
				)
			);
		}
		else {
			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextError(
					MFW::ArticleHeaderFactory(
						public_identifier_,
						message->from(),
						recv_article_hash
					)
				)
			);
		}
	}
}

void Peer::handle_article_list_request(shared_ptr_message message) {
	auto req_news_id = message->article_list().newspaper_id();
	const int articles_per_batch = 15;

	try {
		auto& news_ref = get_news(req_news_id);
		news_ref.add_reader(message->from());

		ArticleListSort sort = ArticleListSort::Created;
		if (message->article_list().timestamp_sort_type() == np2ps::ArticleListTimestamp::AL_MODIFIED) {
			sort = ArticleListSort::Modified;
		}
		bool first_in_batch = true;
		article_container articles;
		article_container readers_only_articles;
		for (std::size_t i = 0; i < message->article_list().count(); i += articles_per_batch) {

			news_ref.get_newest_articles(
				articles,
				readers_only_articles,
				i,
				i + articles_per_batch < message->article_list().count() ? i + articles_per_batch : message->article_list().count(),
				message->article_list().timestamp(),
				sort
			);

			networking_->enroll_message_to_be_sent(
				MFW::RespArticleListFactory(
					MFW::ArticleListFactory(
						public_identifier_,
						message->from()
					),
					articles,
					news_ref.get_id(),
					readers_only_articles,
					first_in_batch
				)
			);
			first_in_batch = false;
			articles.clear();
		}
	}
	catch (unknown_newspaper_error& e) {
		networking_->enroll_message_to_be_sent(
			MFW::ErrorArticleListFactory(
				MFW::ArticleListFactory(
					public_identifier_,
					message->from()
				),
				req_news_id
			)
		);
	}
}

void Peer::handle_article_data_update_request(shared_ptr_message message) {
	// //reporter part
	// if (find_article(message->article_data_update().article_pk()).has_value()) {
	// 	if (message->article_data_update().article_action() == np2ps::DOWNLOAD) {
	// 		if (user_map.find(message->from()) == user_map.end()) {
	// 			auto uit = user_map.insert( {message->from(), PeerInfo(message->from())} );
	// 			readers_.insert( {message->from(), &(uit.first->second)} );
	// 		}
	// 		else {
	// 			auto uit = user_map.find(message->from());
	// 			readers_.insert( {message->from(), &(uit->second)} );
	// 		}
	// 	}
	// 	else if (message->article_data_update().article_action() == np2ps::REMOVAL) {
	// 		auto [bit, eit] = readers_.equal_range(message->article_data_update().article_pk());
	// 		for (auto it = bit; it != eit; it++) {
	// 			if (it->second->peer_key == message->from()) {
	// 				readers_.erase(it);
	// 				break;
	// 			}
	// 		}
	// 	}
	// }
}

void Peer::handle_update_margin_request(shared_ptr_message message) {
	auto article = find_article(message->update_margin().article_pk());

	auto [margin_begin, margin_end] = article.value()->get_range_iterators(public_identifier_);

	std::vector<Margin> vm;
	for (; margin_begin != margin_end; margin_begin++) {
		vm.push_back(margin_begin->second);
	}

	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextResponse(
			MFW::UpdateMarginFactory(
				public_identifier_,
				message->from(),
				message->update_margin().article_pk(),
				vm
			)
		)
	);
}

void Peer::handle_credentials_request(shared_ptr_message message) {
	QString resp_ip4, resp_ip6;
	CryptoPP::RSA::PublicKey resp_rsa_public;
	eax_optional resp_eax;
	IpWrapper& ipw = get_networking()->ip_map().get_wrapper_ref(message->from());

	if (message->credentials().has_rsa_public_key()) {
		ipw.set_rsa_hex_string(message->credentials().rsa_public_key().key());
	}
	if (message->credentials().has_eax_key()) {
		CryptoPP::ByteQueue dec_key;
		std::string enc_key = message->credentials().eax_key().key();
		std::string signature = message->credentials().eax_key().signature();
		auto public_rsa = ipw.get_rsa_optional();
		auto private_rsa = get_networking()->ip_map().private_rsa;
		bool verified = CryptoUtils::instance().verify_decrypt_symmetric_key(
			enc_key,
			signature,
			dec_key,
			public_rsa,
			private_rsa
		);
		ipw.add_eax_key(std::move(dec_key));
	}

	bool req_my_credentials = message->credentials().target() == 0 || message->credentials().target() == public_identifier_;
	pk_t target = message->credentials().target();

	if (req_my_credentials)
		target = public_identifier_;

	if (message->credentials().req_ipv4()) {
		resp_ip4 = networking_->ip_map_.get_ip4(target).toString();
	}
	if (message->credentials().req_ipv6()) {
		resp_ip6 = networking_->ip_map_.get_ip6(target).toString();
	}
	if (message->credentials().req_rsa_public_key()) {
		resp_rsa_public = networking_->ip_map_.get_wrapper_ref(target).get_rsa();
	}
	if (message->credentials().req_eax_key()) {
		if (!networking_->ip_map_.have_eax(message->from())) {
			networking_->save_symmetric_key(
				message->from(),
				networking_->generate_symmetric_key()
			);
		}
		resp_eax = networking_->ip_map_.get_eax(message->from());
	}

	CredentialsPayload payload;
	payload.set_private(get_networking()->ip_map().private_rsa);
	payload.set_public(ipw.get_rsa_optional());

	networking_->enroll_message_to_be_sent(
		MFW::RespCredentialsFactory(
			MFW::CredentialsFactory(
				public_identifier_,
				message->from()
			),
			resp_ip4, resp_ip6, resp_rsa_public, resp_eax, payload
		)
	);
}

bool verify_article_hash(Article& article, shared_ptr_message message) {
	std::cout << "Article " << article.main_hash() << " verification" << std::flush;
	if (article.verify(message->article_all().article_actual())) {
		std::cout << " succeeded." << std::endl;
		return true;
	}

	std::cout << " failed." << std::endl;
	return false;
}

bool verify_article_news_signature(Article& article, NewspaperEntry& news) {
	std::cout << "Article " << article.main_hash() << " verification for news" << news.get_id() << std::flush;
	if (article.verify_news_signature(news.get_newspaper_public_key())) {
		std::cout << " succeeded." << std::endl;
		return true;
	}

	std::cout << " failed." << std::endl;
	return false;
}

void Peer::handle_article_all_response(shared_ptr_message message) {
	if (message->has_article_all() && message->article_all().has_header() && message->article_all().has_article_actual()) {
		hash_t recv_article_id = message->article_all().article_hash();
		auto article_opt = find_article(recv_article_id);
		auto& news = get_news(message->article_all().header().news_id());

		if (article_opt.has_value()) {

			/* Article (header) found in database */
			/* Verify article */

			Article& article = *(article_opt.value());

			if (verify_article_hash(article, message)
				&& verify_article_news_signature(article, news)) 
			{

				/* Successful verification */

				std::cout << "Article verification succeded for " << recv_article_id << std::endl;
				
				article.add_readers(message->article_all().header().readers());

				if (article.is_header_only()) {

					/* Article in database contains only the header */
					
					Article article_from_message(message->article_all().header(), "");
					article.update_metadata(article_from_message);
					article.set_path(message->article_all().article_actual());

					generate_successful_download_message(article.author_id(), article.main_hash());
					generate_successful_download_message(article.news_id(), article.main_hash());

					for (auto&& reader : article.readers()) {
						if (reader == get_public_id()) continue;
						generate_successful_download_message(
							reader, article.main_hash()
						);
					}

					emit checked_display_article(message->article_all().header().news_id(), article.main_hash());
				}
				else if (article.modification_time() < message->article_all().header().modification_time()) {

					/* Received article is newer than one in database */

					get_news(message->article_all().header().news_id()).remove_article(message->article_all().header().main_hash());
					Article recv_article(message->article_all().header(), message->article_all().article_actual());
					enroll_new_article(recv_article, false);
				}
				article.article_downloaded();
			}
			else {

				/* Failed verification */

				std::cout << "Article verification failed for: " << recv_article_id << std::endl;

				if (networking_->soliciting_articles.find(recv_article_id) != networking_->soliciting_articles.end()) {

					/* try another peer when this one failed */

					auto destination = networking_->soliciting_articles[recv_article_id].back();
					networking_->soliciting_articles[recv_article_id].pop_back();
					generate_article_all_message(destination, recv_article_id);
				}
				else {

					/* Article is not soliciting, it can't be downloaded. */

					if (downloading_articles.find(recv_article_id) != downloading_articles.end()) {
						downloading_articles.erase(recv_article_id);
						emit check_selected_item();
					}
				}

				article_all_send_more(article);

				return;
			}
		}
		else {

			Article recv_article(message->article_all().header(), message->article_all().article_actual());
			/* Article not found in database. */
			if (recv_article.get_ancestor() != 0) {
				news.remove_article(recv_article.get_ancestor());
			}
			else {
				recv_article.lazy_remove_readers(networking_->disconnected_readers_lazy_remove.users);

				/* Inform every reader, that we downloaded article successfully */

				// generate_successful_download_message_all_readers(
				// 	recv_article.const_readers(),
				// 	message->from(),
				// 	recv_article_id
				// );

			}
			enroll_new_article(recv_article, false);
		}

		/* Article is now downloaded. */

		downloading_articles.erase(recv_article_id);
		emit check_selected_item();


		if (networking_->soliciting_articles.find(recv_article_id) != networking_->soliciting_articles.end()) { //remote the article from soliticing articles, since we got an answer
			networking_->soliciting_articles.erase(recv_article_id);
		}
	}
}

void Peer::generate_successful_download_message_all_readers(const user_container& readers, pk_t from, pk_t recv_article_id) {
	for (auto&& reader : readers) {
		if (reader == from || reader == get_public_id() || friends_.count(reader) > 0) {
			continue;
		}

		generate_successful_download_message(reader, recv_article_id);
	}
	for (auto&& fren : friends_) {
		generate_successful_download_message(fren, recv_article_id);
	}
}

void Peer::generate_successful_download_message(pk_t reader, pk_t recv_article_id) {
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextOneWay(
			MFW::ArticleDataChangeFactory(
				public_identifier_,
				reader,
				recv_article_id,
				true
			)
		)
	);
}

void check_add_article_to_news(shared_ptr_message message, NewspaperEntry& news, Article&& article) {
	auto add_to_news = [&](){
		article.add_reader(message->from());
		news.add_article(article.main_hash(), std::move(article));
	};

	try { //try finding received article from article list in article database
		auto& local_article = news.get_article(article.main_hash());
		local_article.update_metadata(article);
		if (local_article.modification_time() < article.modification_time()) { //if received article is newer, replace it
			news.remove_article(article.main_hash());
			add_to_news();
		}
		else {
			local_article.add_reader(message->from());
		}
	}
	catch (article_not_found_database& e) { //if it wasn't found, add it
		add_to_news();
	}
}

void Peer::handle_article_list_response(shared_ptr_message message) {
	auto list_size = message->article_list().response_size();
	auto readers_list_size = message->article_list().response_readers_size();
	if (list_size != 0 || readers_list_size != 0) {
		std::cout << "Received article list size: " << list_size << std::endl;
		std::cout << "Received article readers: " << readers_list_size << std::endl;
		pk_t list_news_id = message->article_list().newspaper_id();
		auto& news = get_news(list_news_id);

		/* add received articles to their news, or at least update the metadata */
		for (auto it = message->article_list().response().begin(); it != message->article_list().response().end(); it++) {
			check_add_article_to_news(message, news, Article(*it));
		}

		/* update reader metadata of articles */
		for (auto readers_it = message->article_list().response_readers().begin(); readers_it != message->article_list().response_readers().end(); readers_it++) {
			try {
				Article& article = news.get_article(readers_it->main_hash());
				article.update_readers_from_gpb(*readers_it, message->from(), list_news_id, article.author_id());
			}
			catch (article_not_found_database anfd) {
				std::cout << "Received readers for article without content: " << readers_it->main_hash() << std::endl;
			}
		}

		/* news contents was updated -> can be used when requesting article list later on */
		news.update();
		auto [bit,eit] = news.get_newest_articles(1, get_article_list_sort_config());
		// auto [bit,eit] = news.get_newest_articles(slot_get_config_peer_article_list_default_total(), get_article_list_sort_config());

		/* request newest N articles from list, that are not present yet */
		if (message->article_list().first_in_batch()) {
			for (; bit != eit; bit++) {
				if (news.get_article(bit->second).is_header_only()) {
					generate_article_all_message(list_news_id, bit->second);
				}
			}
		}
		emit new_article_list(list_news_id);
	}
	else {
		std::cout << "Article List response for " << message->from() << "; empty list" << std::endl;
	}
	getting_article_list.erase(message->from());

	
	
	emit check_selected_item();
}

void Peer::handle_credentials_response(shared_ptr_message message) {
	if (message->credentials().req_ipv4()) {
		if (message->credentials().req_ipv6()) {
			networking_->ip_map_.update_ip(
				(pk_t)message->from(),
				QHostAddress(QString(message->credentials().ipv4().c_str())),
				QHostAddress(QString(message->credentials().ipv6().c_str())));
		}
		else {
			networking_->ip_map_.update_ip(
				(pk_t)message->from(),
				QHostAddress(QString(message->credentials().ipv4().c_str()))
			);
		}
	}
	if (message->credentials().has_rsa_public_key()) {
		networking_->ip_map_.update_rsa_public((pk_t)message->from(), message->credentials().rsa_public_key().key());
	}
	if (message->credentials().has_eax_key()) {
		CryptoPP::ByteQueue dec_key;
		std::string enc_key = message->credentials().eax_key().key();
		std::string signature = message->credentials().eax_key().signature();
		auto public_rsa = get_networking()->ip_map().get_wrapper_ref(message->from()).get_rsa_optional();
		auto private_rsa = get_networking()->ip_map().private_rsa;
		CryptoUtils::instance().verify_decrypt_symmetric_key(
			enc_key,
			signature,
			dec_key,
			public_rsa,
			private_rsa
		);
		get_networking()->ip_map().update_eax((pk_t)message->from(), dec_key);
		get_networking()->decrypt_encrypted_messages(message->from());
	}

	auto& np2ps_messages_waiting_for_credentials = get_networking()->np2ps_messages_waiting_for_credentials;
	if (np2ps_messages_waiting_for_credentials.count(message->from()) > 0) {
		/* process messages, that are waiting for sender's credentials */
		auto [bit, eit] = np2ps_messages_waiting_for_credentials.equal_range(message->from());
		std::vector<decltype(bit)> to_erase;
		for (; bit != eit; bit++) {
			auto message = bit->second;
			networking_->enroll_message_to_be_sent(message);
			to_erase.push_back(bit);
		}
		for (auto&& erase : to_erase) {
			np2ps_messages_waiting_for_credentials.erase(erase);
		}
	}
}

void Peer::handle_public_key_response(shared_ptr_message message) {
	networking_->ip_map_.update_rsa_public((pk_t)message->from(), message->public_key().key());
	if (newspapers_awaiting_confirmation_.find((pk_t)message->from()) != newspapers_awaiting_confirmation_.end()) {
		news_.insert({message->from(), newspapers_awaiting_confirmation_[(pk_t)message->from()]});
		newspapers_awaiting_confirmation_.erase(message->from());
		emit got_newspaper_confirmation(message->from());
	}
}

void Peer::handle_symmetric_key_response(shared_ptr_message message) {
	emit symmetric_key_exchanged(message->from());
}

void Peer::handle_update_margin_response(shared_ptr_message message) {
	auto article = find_article(message->update_margin().article_pk());
	for (int i = 0; i < message->update_margin().margin().margins_size(); i++) {
		article.value()->add_margin(message->from(), Margin(
			message->update_margin().margin().margins(i).type(),
			message->update_margin().margin().margins(i).content(),
			message->from()
		));
	}
}


void Peer::handle_article_solicitation_one_way(shared_ptr_message msg) {
	auto check_for_existence = networking_->soliciting_articles.find(msg->article_sol().article_hash()); //check if this article is in fact in solicitation
	if (check_for_existence == networking_->soliciting_articles.end()) {
		std::vector<pk_t> potential_owners;
		for (auto i = msg->article_sol().possible_owners().begin(); i != msg->article_sol().possible_owners().end(); i++) {
			potential_owners.push_back(*i); //get potential owners
		}
		networking_->soliciting_articles.insert({msg->article_sol().article_hash(), std::move(potential_owners)});
	}
	auto destination = networking_->soliciting_articles[msg->article_sol().article_hash()].back(); //get one owner
	networking_->soliciting_articles[msg->article_sol().article_hash()].pop_back(); //remove owner you just got
	networking_->ip_map_.update_preferred_stun_server(destination, msg->from()); //update his preferred stun server
	generate_article_all_message(destination, msg->article_sol().article_hash()); //send him article all message
}

void Peer::handle_symmetric_key_one_way(shared_ptr_message msg) {
	using namespace CryptoPP;
	std::string key_encrypted_encoded = msg->symmetric_key().key();
	std::string signature_str = msg->symmetric_key().signature();

	rsa_public_optional rsa_public = networking_->ip_map_.get_rsa_public(msg->from());
	rsa_private_optional rsa_private = networking_->ip_map_.private_rsa;
	ByteQueue queue;
	bool verification_result = 
		CryptoUtils::instance().verify_decrypt_symmetric_key(
			key_encrypted_encoded,
			signature_str,
			queue,
			rsa_public,
			rsa_private
		);

	std::cout << "Verification of received symmetric key";
	if (!verification_result) {
		std::cout << " FAILED" << std::endl;
	}
	else {
		std::cout << " SUCCEEDED" << std::endl;

		networking_->ip_map_.get_wrapper_for_pk(
			msg->from()
		)->second.add_eax_key(
			std::move(queue)
		);
	}

	//send response that symmetric key was received and processed
	shared_ptr_message _msg = std::make_shared<proto_message>();
	_msg->set_from(public_identifier_);
	_msg->set_to(msg->from());
	_msg->set_msg_ctx(np2ps::RESPONSE);
	_msg->set_msg_type(np2ps::SYMMETRIC_KEY);

	networking_->enroll_message_to_be_sent(
		std::move(_msg)
	);
}

void Peer::handle_public_key_one_way(shared_ptr_message msg) {
	networking_->ip_map_.update_rsa_public((pk_t)msg->from(), msg->public_key().key());
	
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextResponse(
			MFW::PublicKeyFactory(
				public_identifier_,
				msg->from(),
				networking_->ip_map_.my_ip().key_pair.first.value()
			)
		)
	);
}


void Peer::handle_article_data_update_one_way(shared_ptr_message msg) {
	// //reporter part
	// if (find_article(message->article_data_update().article_pk()).has_value()) {
	// 	if (message->article_data_update().article_action() == np2ps::DOWNLOAD) {
	// 		if (user_map.find(message->from()) == user_map.end()) {
	// 			auto uit = user_map.insert( {message->from(), PeerInfo(message->from())} );
	// 			readers_.insert( {message->from(), &(uit.first->second)} );
	// 		}
	// 		else {
	// 			auto uit = user_map.find(message->from());
	// 			readers_.insert( {message->from(), &(uit->second)} );
	// 		}
	// 	}
	// 	else if (message->article_data_update().article_action() == np2ps::REMOVAL) {
	// 		auto [bit, eit] = readers_.equal_range(message->article_data_update().article_pk());
	// 		for (auto it = bit; it != eit; it++) {
	// 			if (it->second->peer_key == message->from()) {
	// 				readers_.erase(it);
	// 				break;
	// 			}
	// 		}
	// 	}
	// }
	auto article_opt = find_article(msg->article_data_update().article_pk());
	if (!article_opt.has_value()) {
		return;
	}
	auto& article = article_opt.value();
	article->add_reader(msg->from());
}


void Peer::handle_credentials_one_way(shared_ptr_message msg) {
	IpWrapper& ipw = get_networking()->ip_map().get_wrapper_ref(msg->from());

	if (msg->credentials().method() == 1) {
		if (msg->credentials().has_rsa_public_key()) {
			ipw.set_rsa_hex_string(msg->credentials().rsa_public_key().key());
		}

		if (msg->credentials().has_eax_key()) {
			CryptoPP::ByteQueue dec_key;
			std::string enc_key = msg->credentials().eax_key().key();
			std::string signature = msg->credentials().eax_key().signature();
			auto public_rsa = ipw.get_rsa_optional();
			auto private_rsa = get_networking()->ip_map().private_rsa;
			bool verified = CryptoUtils::instance().verify_decrypt_symmetric_key(
				enc_key,
				signature,
				dec_key,
				public_rsa,
				private_rsa
			);
			ipw.add_eax_key(std::move(dec_key));
		}

		get_networking()->enroll_message_to_be_sent(
			MFW::OneWayCredentialsFactory(
				MFW::CredentialsFactory(
					public_identifier_,
					msg->from()
				),
				{},
				{},
				{},
				2
			)
		);
	}
	else if (msg->credentials().method() == 2) {
		emit symmetric_key_exchanged(msg->from());
	}
}


void Peer::handle_article_solicitation_error(shared_ptr_message msg) {
	auto check_for_existence = networking_->soliciting_articles.find(msg->article_sol().article_hash());
	if (check_for_existence != networking_->soliciting_articles.end()) {
		auto destination = networking_->soliciting_articles[msg->article_sol().article_hash()].back(); //get next owner
		networking_->soliciting_articles[msg->article_sol().article_hash()].pop_back(); //pop him
		generate_article_all_message(destination, msg->article_sol().article_hash()); //ask him
	}
	else {
		std::cout << "Requested article with hash " << msg->article_sol().article_hash() << " was not found inside the network." << std::endl;
		downloading_articles.erase(msg->article_sol().article_hash());
	}
}

void Peer::handle_article_header_error(shared_ptr_message msg) {
	std::cout << "Article " << msg->article_header().article_hash() << " failed to upload" << std::endl;
}

void Peer::handle_article_list_error(shared_ptr_message msg) {
	auto& news = get_news(msg->article_list().newspaper_id());
	news.remove_reader(msg->from());
}



bool Peer::remove_article(hash_t hash) {
	for (auto&& news : news_) {
		if (news.second.remove_article(hash))
			return true;
	}
	return false;
}

bool Peer::remove_article(hash_t hash, pk_t& newspaper_id) {
	for (auto&& news : news_) {
		if (news.second.remove_article(hash)) {
			newspaper_id = news.second.get_id();
			return true;
		}
	}
	return false;
}

void Peer::removed_external_article(hash_t article, pk_t to) {
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextRequest(
			MFW::ArticleDataChangeFactory(
				public_identifier_,
				to,
				article,
				false
			)
	));
}

void Peer::upload_external_article(Article article) {
	NewspaperEntry& news = get_news(article.news_id());
	for (auto&& coworker : news.get_coworkers()) {
		if (coworker == public_identifier_ || news.find_reader(coworker)) {
			continue;
		}
		networking_->enroll_message_to_be_sent(
			MFW::OneWayArticleAllFactory (
				MFW::ArticleDownloadFactory(
					public_identifier_, 
					coworker, 
					&article
				),
				article
			)
		);
	}
	for (auto&& reader : news.get_readers()) {
		networking_->enroll_message_to_be_sent(
			MFW::OneWayArticleAllFactory (
				MFW::ArticleDownloadFactory(
					public_identifier_, 
					reader, 
					&article
				),
				article
			)
		);
	}
}

void Peer::handle_article_all_error(shared_ptr_message message) {
	auto article_id = message->article_all().header().main_hash();
	auto news_id = message->article_all().header().news_id();

	auto& news = get_news(news_id);
	auto& article = news.get_article(article_id);

	/* Remove the reader, since he doesn't have the requested article. */

	article.remove_reader(message->from());
	article_all_send_more(article);
}

void Peer::generate_newspaper_entry_request(pk_t destination, pk_t newspaper_id) {
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextRequest(
			MFW::NewspaperEntryFactory(
				public_identifier_,
				destination,
				newspaper_id,
				""
			)
		)
	);
}

void Peer::handle_newspaper_entry_request(shared_ptr_message message) {
	std::uint64_t news_id = message->newspaper_entry().entry().news_id();
	std::string news_name = message->newspaper_entry().entry().news_name();

	try {
		auto& news = get_news(news_id);
		news.add_reader(message->from());
		networking_->enroll_message_to_be_sent(
			MFW::RespNewspaperEntryFactory(
				MFW::NewspaperEntryFactory(
					public_identifier_,
					message->to(),
					news_id,
					news_name
				),
				news,
				get_networking()->ip_map()
			)
		);
	}
	catch (unknown_newspaper_error& e) {
		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextError(
				MFW::NewspaperEntryFactory(
					public_identifier_,
					message->from(),
					news_id,
					news_name
				)
			)
		);
	}
}

void Peer::handle_newspaper_entry_response(shared_ptr_message message) {
	pk_t news_id = message->newspaper_entry().entry().news_id();
	try {
		auto& news = get_news(news_id);
		news.deserialize(message->newspaper_entry());
	}
	catch (unknown_newspaper_error& e) {
		add_new_newspaper(news_id, message->newspaper_entry().entry().news_name(), message->from());
	}

}

void Peer::handle_newspaper_entry_error(shared_ptr_message message) {
	pk_t news_id = message->newspaper_entry().entry().news_id();
	try {
		auto& news = get_news(news_id);
		news.remove_reader(message->from());
	}
	catch (unknown_newspaper_error& e) {
	}
}

void Peer::handle_newspaper_list_request(shared_ptr_message message) {
	news_database& my_news = get_news();
	if (message->newspaper_list().requested_ids().size() == 0) {
		for (auto&& news : my_news) {
			news.second.add_reader(message->from());
		}
		networking_->enroll_message_to_be_sent(
			MFW::RespNewspaperListFactory(
				MFW::NewspaperListFactory(
					get_public_id(),
					message->from()
				),
				my_news,
				get_networking()->ip_map(),
				(std::int16_t)message->newspaper_list().article_count()
			)
		);
	}
	else {
		news_database news_to_send;
		for (pk_t news_id : message->newspaper_list().requested_ids()) {
			if (my_news.count(news_id) > 0) {
				news_to_send.emplace(news_id, my_news[news_id]);
				my_news[news_id].add_reader(message->from());
			}
		}
		networking_->enroll_message_to_be_sent(
			MFW::RespNewspaperListFactory(
				MFW::NewspaperListFactory(
					get_public_id(),
					message->from()
				),
				news_to_send,
				get_networking()->ip_map()
			)
		);
	}
}

void Peer::add_new_newspaper_from_file(const std::string& path) {
	pk_t news_id;
	std::ifstream file(path);
	std::string line;
	if (std::getline(file, line)) {
		news_id = std::stoll(line);
	}
	else {
		return; //throw other_error("Empty newspaper file.");
	}
	NewspaperEntry news(news_id, &networking_->disconnected_readers_lazy_remove);
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string ip, port, public_id;
		if (!std::getline(ss, ip, ':') ||
		    !std::getline(ss, port, ':') ||
		    !std::getline(ss, public_id)) 
		{
			throw other_error("Invalid newspaper file format.");
		}
		else {
			pk_t id = std::stoll(public_id);
			news.add_reader(id);
			networking_->enroll_new_peer(ip, id);
		}
	}
	news_.insert({ news_id, std::move(news) });
	emit got_newspaper_confirmation(news_id);
}

void Peer::add_new_newspaper_pk(pk_t id) {
	news_.insert({ id, std::move(NewspaperEntry(id, &networking_->disconnected_readers_lazy_remove)) });
	emit got_newspaper_confirmation(id);
}

/**
 * For debugging purposes only. info
*/
void Peer::print_contents() {
	std::cout << "public_identifier_ " << public_identifier_ << std::endl;
	std::cout << "name_ " << name_ << std::endl;
	std::cout << "newspaper_id_ " << newspaper_id_ << std::endl;
	std::cout << "newspaper_name_ " << newspaper_name_ << std::endl;
	std::cout << "news_ count: " << news_.size() << std::endl;
	for (auto&& n : news_) {
		std::cout << "news: " << (n.second.get_name().empty() ? "EMPTY" : n.second.get_name()) << "; " << n.second.get_id() << std::endl;
		for (auto&& f : n.second.get_readers()) {
			std::cout << "  friend: " << f << std::endl;
		}
		for (auto&& a : n.second.get_all_articles()) {
			std::cout << "  Article: " << a.second.main_hash() << std::endl;
			std::cout << "  - readers: " << a.second.readers().size() << std::endl;
		}
		if (n.second.has_newspaper_public_key()) {
			std::cout << "  I have public key." << std::endl;
		}
		else {
			std::cout << "  I do NOT have public key." << std::endl;
		}
		if (n.second.has_newspaper_private_key()) {
			std::cout << "  I am journalist." << std::endl;
		} 
		else {
			std::cout << "  I am NOT journalist." << std::endl;
		}
	}
	auto connected = get_networking()->ip_map().select_connected(0);
	for (auto&& [c_pk, c_wrapper] : connected) {
		std::cout << "Peer " << c_pk << " is in relaying state: " << std::flush;
		switch(c_wrapper.relay_state) {
			case RelayState::Direct:
				std::cout << " Direct." << std::endl;
				break;
			case RelayState::Relayed:
				std::cout << " Relayed." << std::endl;
				break;
			case RelayState::Unknown:
				std::cout << " Unknown." << std::endl;
				break;
			default:
				std::cout << std::endl;
				break;
		}
	}
	std::cout << "Waiting messages: " << get_networking()->np2ps_messages_waiting_for_peer_online_short_term.size() << std::endl;
}

void Peer::generate_newspaper_list_request() {
	for (pk_t global_friend : get_friends()) {
		generate_newspaper_list_request(global_friend);
	}
}

void Peer::generate_newspaper_list_request(pk_t destination) {
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextRequest(
			MFW::NewspaperListFactory(
				get_public_id(),
				destination,
				config.list_size_default
			)
		)
	);
}

void Peer::handle_newspaper_list_response(shared_ptr_message message) {
	if (!message->has_newspaper_list()) {
		throw malformed_message_context_or_type("Message does not contain newspaper list.");
	}

	for (auto it = message->newspaper_list().news().begin(); it != message->newspaper_list().news().end(); it++) {
		auto check_articles = [&](NewspaperEntry& news) {
			if (it->articles_size() > 0) {
				for (auto article_it = it->articles().begin(); article_it != it->articles().end(); article_it++) {
					check_add_article_to_news(message, news, Article(*article_it));
				}
			}
			/* We can use sender as relay for this news. */
			get_networking()->ip_map().check_or_add_to_ip_map_relayed(
				news.get_id(),
				message->from(),
				RelayState::Keep
			);
		};
		std::cout << "Entry in list " << it->entry().news_id() << std::endl;
		NewspaperEntry received_news_entry(*it, &networking_->disconnected_readers_lazy_remove);
		if (received_news_entry.get_id() == get_public_id()) {
			continue;
		}
		if (!find_news(it->entry().news_id())) {
			/* We got new news! */
			news_potential.emplace(
				received_news_entry.get_id(), 
				std::make_tuple(
					received_news_entry,
					QHostAddress(it->network_info().ipv4()),
					it->network_info().port(),
					it->network_info().stun_port()
				)
			);
		}
		else {
			/* News were already enrolled, update metadata. */
			auto& news = get_news(it->entry().news_id());
			news.update_metadata(received_news_entry);
			check_articles(news);
		}
	}

	emit newspaper_list_received();
}

void Peer::slot_newspaper_from_list_added(pk_t news_id) {
	auto& news = news_potential[news_id];
	NewspaperEntry entry = std::get<0>(news);
	QHostAddress address = std::get<1>(news);
	port_t np2ps_port = std::get<2>(news);
	port_t stun_port = std::get<3>(news);
	news_potential.erase(news_id);
	news_id = entry.get_id();

	entry.clear_last_updated();

	add_new_newspaper(std::move(entry), std::move(address), np2ps_port, stun_port);
	generate_article_list_message(news_id);

	emit signal_newspaper_updated();
}

/**
 * @brief Set the name of the peer.
 * 
 * @param name Desired name, which will be set.
 */
void Peer::set_name(const my_string& name) {
	name_ = name;
}

/**
 * @brief Get ID of my news.
 * 
 * @return pk_t ID of my news
 */
pk_t Peer::get_my_news_id() {
	return newspaper_id_;
}

/**
 * @brief Set the ip of the peer.
 * 
 * @param ip Desired IP to be set.
 */
void Peer::set_my_ip(QString ip) {
	networking_->ip_map_.get_wrapper_ref(public_identifier_).ipv4 = QHostAddress(ip);
}

/**
 * @brief Generator for article list message, with support for categories.
 * 
 * @tparam Container Category container type.
 * @param destination ID of newspaper which article list we want.
 * @param categories Categories we want.
 */
void Peer::generate_article_list_message(pk_t destination, const std::vector<std::string>& categories) {
	networking_->enroll_message_to_be_sent(
		MFW::ReqArticleListFactory(
			MFW::ArticleListFactory(
				public_identifier_,
				destination
			),
			categories
		)
	);
}

/**
 * @brief Generator for add margin message.
 * 
 * For one margin.
 * 
 * @param article_hash Article hash of article, to which we want to add the margin.
 * @param type Type of the margin.
 * @param content Content of the margin.
 */
void Peer::add_margin(hash_t article_hash, my_string type, my_string content) {
	article_optional article = find_article(article_hash);
	article.value()->add_margin(public_identifier_, Margin(type, content, public_identifier_));
}

/**
 * @brief Generator for add margin message.
 * 
 * For multiple margins.
 * 
 * @param article_hash Article hash of article, to which we want to add the margins.
 * @param vm Margin vector containing the margins.
 */
void Peer::add_margin(hash_t article_hash, margin_vector& vm) {
	auto article = find_article(article_hash);
	if (article.has_value()) {
		auto author = article.value()->author_id();
		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextRequest(
				MFW::UpdateMarginAddFactory(
					public_identifier_,
					author,
					article_hash,
					vm
				)
			)
		);
	}
	else {
		throw other_error("Can't add margin to article that is not in DB.");
	}
}

/**
 * @brief Updates existing margin in given article.
 * 
 * For one margin only.
 * 
 * @param article_hash Article hash of article, where we want to update the margins.
 * @param id ID of margin we want to update.
 * @param type Type we want to set.
 * @param content Content we want to set.
 */
void Peer::update_margin(hash_t article_hash, unsigned int id, my_string type, my_string content) {
	auto article = find_article(article_hash);
	if (article.has_value()) {
		auto author = article.value()->author_id();
		std::vector<Margin> vm = { Margin(type, content, id) };
		
		auto [marginb, margine] = margins_added_.equal_range(article_hash);

		for (; marginb != margine; marginb++) {
			//update our margin database first
			if (marginb->second.id == id) {
				marginb->second.type += ' ' + type;
				marginb->second.content += ' ' + content;
				break;
			}
		}

		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextRequest(
				MFW::UpdateMarginUpdateFactory(
					public_identifier_,
					author,
					article_hash,
					vm
				)
			)
		);
	}
	else {
		throw other_error("Can't update margin of article that is not in DB.");
	}
}

/**
 * @brief Removes existing margin in given article.
 * 
 * @param article_hash Article hash of article, where we want to remove the margin.
 * @param id ID of margin to remove.
 */
void Peer::remove_margin(hash_t article_hash, unsigned int id) {
	auto article = find_article(article_hash);
	if (article.has_value()) {
		auto author = article.value()->author_id();
		Margin m;
		m.id = id;
		
		auto [marginb, margine] = margins_added_.equal_range(article_hash);

		for (; marginb != margine; marginb++) {
			if (marginb->second.id == id) {
				m.type = marginb->second.type;
				m.content = marginb->second.content;
				margins_added_.erase(marginb);
				break;
			}
		}

		std::vector<Margin> vm = {m};

		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextRequest(
				MFW::UpdateMarginRemoveFactory(
					public_identifier_,
					author,
					article_hash,
					vm
				)
			)
		);
	}
	else {
		throw other_error("Can't update margin of article that is not in DB.");
	}
}

/**
 * @brief Public identifier getter.
 * 
 * @return pk_t Public identifier.
 */
pk_t Peer::get_public_id() {
	return public_identifier_;
}

/**
 * @brief Name getter.
 * 
 * @return my_string Name
 */
my_string Peer::get_name() {
	return name_;
}

my_string& Peer::name() {
	return name_;
}

/**
 * @brief News database getter.
 * 
 * Returns non-const reference to newspaper database.
 * Use with caution.
 * 
 * @return news_database& Reference to requested news.
 */
news_database& Peer::get_news_db() {
	return news_;
}

/**
 * @brief News name getter. 
 * 
 * @return std::string News name.
 */
std::string Peer::get_my_news_name() {
	return newspaper_name_;
}

void Peer::networking_init_sender_receiver() {
	networking_->init_sender_receiver(&news_);
}

void Peer::stun_allocate() {
	//networking_->get_stun_client()->allocate_request();
}

void Peer::add_journalist(pk_t j) {
	generate_new_journalist(j);
}
void Peer::remove_journalist(pk_t j) {
	journalists_.erase(j);
}
Networking* Peer::get_networking() {
	return networking_.get();
}

std::unordered_set<hash_t>& Peer::get_downloading_articles() {
	return downloading_articles;
}

std::unordered_set<hash_t>& Peer::get_getting_article_list() {
	return getting_article_list;
}

/**
 * @brief Removes reader for given article.
 */
void Peer::remove_reader(hash_t article, pk_t reader) {
	auto article_opt = find_article(article);
	if (article_opt.has_value()) {
		article_opt.value()->readers().erase(reader);
	}
	else {
		throw other_error("Removing reader from non-existing article.");
	}
}

void Peer::serialize(np2ps::Peer* gpb_peer) {
	gpb_peer->set_name(name_);
	gpb_peer->set_public_identifier(public_identifier_);
	auto ip_map_gpb = gpb_peer->mutable_ip_map();
	networking_->ip_map_.serialize_ip_map(ip_map_gpb);
	for (auto&& newspapers : news_) {
		auto ne_gpb = gpb_peer->add_news();
		NewspaperEntry& ne = newspapers.second;
		ne.local_serialize_entry(ne_gpb);
	}
	serialize_config(gpb_peer->mutable_config());
}

void Peer::serialize_config(np2ps::PeerConfig* serialized_peer_config) {
	serialized_peer_config->set_gossip_randoms(config.gossip_randoms);
	serialized_peer_config->set_list_size_default(config.list_size_default);
	serialized_peer_config->set_list_size_first(config.list_size_first);
}

bool Peer::find_news(pk_t news_id) {
	return news_.find(news_id) != news_.end(); 
}

user_container& Peer::get_friends() {
	return friends_;
}

bool Peer::add_friend(pk_t id, QHostAddress address) {
	bool emplace_ok = friends_.emplace(id).second;
	return get_networking()->add_to_ip_map(id, std::move(address)) && emplace_ok;
}

bool Peer::add_friend(pk_t id, const QString& str_address) {
	QHostAddress address;
	if (!address.setAddress(str_address)) {
		throw other_error("Invalid IPv4 address.");
	}
	return add_friend(id, address);
}

bool Peer::add_friend(pk_t id, const std::string& ip) {
	return add_friend(id, QString::fromStdString(ip));
}

void Peer::allocate_next_newspaper() {
	if (newspapers_awaiting_confirmation_.size() > 0) {
		get_networking()->get_stun_client()->allocate_request(newspapers_awaiting_confirmation_.begin()->first);
	}
}

void Peer::generate_new_journalist(pk_t pid) {
	auto& my_news = get_my_newspaper();
	my_news.emplace_journalist(pid);
	get_networking()->enroll_message_to_be_sent(
		MFW::RespJournalistFactory(
			MFW::JournalistFactory(
				public_identifier_,
				pid
			),
			my_news.get_newspaper_private_key(),
			my_news,
			networking_->ip_map()
		)
	);
	for (auto&& journalist : my_news.get_journalists()) {
		if (journalist == public_identifier_ || journalist == pid) {
			continue;
		}
		get_networking()->enroll_message_to_be_sent(
			MFW::OneWayNewJournalistFactory(
				MFW::NewJournalistFactory(
					public_identifier_,
					journalist
				),
				pid,
				get_networking()->ip_map().get_wrapper_ref(pid)
			)
		);
	}
}

void Peer::generate_journalist_request(pk_t news_id) {
	get_networking()->enroll_message_to_be_sent(
		MFW::ReqJournalistFactory(
			MFW::JournalistFactory(
				public_identifier_,
				news_id
			),
			name()
		)
	);
}

void Peer::handle_journalist_response(shared_ptr_message message) {
	NewspaperEntry& news = get_news(message->journalist().entry().entry().news_id());
	news.set_newspaper_private_key(
		CryptoUtils::instance().hex_to_private(
			message->journalist().private_key()
		)
	);
	for (auto&& journalist : message->journalist().entry().journalists()) {
		std::cout << "Found journalist " << journalist.publicid() << std::endl;
		if (journalist.publicid() == 0) {
			continue;
		}
		news.emplace_journalist(journalist.publicid());
		IpWrapper wrapper(journalist);
		get_networking()->ip_map().add_to_ip_map(journalist.publicid(), wrapper);
	}
	news.emplace_journalist(public_identifier_);
	journalist_of_.emplace(news.get_id());
	emit signal_journalism_approved(news.get_id());
}

void Peer::handle_journalist_request(shared_ptr_message message) {
	std::string peer_name = message->journalist().peer_name();
	pk_t peer_id = message->from();
	pending_journalist_requests.emplace(peer_name, peer_id);
	emit signal_new_journalist_request(peer_id, peer_name);
}

void Peer::handle_journalist_error(shared_ptr_message message) {
	std::cout << "Peer: " << message->from() << " rejected being a journalist." << std::endl;
}

NewspaperEntry& Peer::get_my_newspaper() {
	return get_news(newspaper_id_);
}

void Peer::remove_news(pk_t to_remove) {
	news_.erase(to_remove);
}

void Peer::generate_user_info_message(pk_t to) {
	get_networking()->enroll_message_to_be_sent(
		MFW::AdvertUserInfoFactory(
			MFW::UserInfoFactory(
				public_identifier_,
				to
			),
			get_networking()->ip_map().my_mapped_users()
		)
	);
}

void Peer::handle_user_info_message_one_way(shared_ptr_message message) {
	switch(message->user_info().method()) {
		case np2ps::ADVERT_UI:
			handle_user_info_message_one_way_advert(message);
			break;
		case np2ps::REQUEST_UI:
			handle_user_info_message_one_way_request(message);
			break;
		case np2ps::RESPONSE_UI:
			handle_user_info_message_one_way_response(message);
			break;
		default:
			throw other_error("Unknown handle user info method.");
	}
}

void Peer::handle_user_info_message_one_way_advert(shared_ptr_message message) {
	std::set<pk_t> response_set;
	for (auto&& user : message->user_info().peers()) {
		if (!get_networking()->ip_map().has_wrapper(user)) {
			response_set.emplace(user);
		}
	}
	get_networking()->enroll_message_to_be_sent(
		MFW::ReqUserInfoFactory(
			MFW::UserInfoFactory(
				public_identifier_,
				message->from()
			),
			response_set
		)
	);
}

void Peer::handle_user_info_message_one_way_request(shared_ptr_message message) {
	std::list<std::pair< pk_t, IpWrapper>> response_list;
	for (auto&& user : message->user_info().peers()) {
		if (get_networking()->ip_map().has_wrapper(user)) {
			IpWrapper wrapper = get_networking()->ip_map().get_wrapper_ref(user);
			response_list.push_back({user, wrapper});
		}
	}
	get_networking()->enroll_message_to_be_sent(
		MFW::RespUserInfoFactory(
			MFW::UserInfoFactory(
				public_identifier_,
				message->from()
			),
			response_list
		)
	);
}

void Peer::handle_user_info_message_one_way_response(shared_ptr_message message) {
	for (auto&& user : message->user_info().peer_ip_wrappers()) {
		IpWrapper wrapper(user);
		get_networking()->ip_map().add_to_ip_map(user.publicid(), wrapper);
	}
}

void Peer::inform_coworkers() {
	for (auto&& news_id : journalist_of_) {
		NewspaperEntry& entry = get_news(news_id);
		pk_t to = entry.get_next_coworker();
		if (to == public_identifier_) {
			continue;
		}
		generate_user_info_message(to);
	}
}

void Peer::handle_new_journalist_one_way(shared_ptr_message message) {
	auto& news = get_news(message->from());
	news.emplace_journalist(message->new_journalist().journalist_ip_wrapper().publicid());
	IpWrapper wrapper(message->new_journalist().journalist_ip_wrapper());
	get_networking()->ip_map().add_to_ip_map(message->new_journalist().journalist_ip_wrapper().publicid(), wrapper);
	std::cout << "Got new journalist: " << message->new_journalist().journalist_ip_wrapper().publicid() << std::endl;
}

void Peer::handle_gossip_request(shared_ptr_message message) {
	auto gossips = get_networking()->ip_map().select_connected_randoms(config.gossip_randoms);
	get_networking()->enroll_message_to_be_sent(
		MFW::RespGossipFactory(
			MFW::GossipFactory(
				public_identifier_,
				message->from()
			),
			gossips
		)
	);
}

void Peer::handle_gossip_response(shared_ptr_message message) {
	for (auto&& gossip : message->gossip().peer_ip_wrappers()) {
		if (gossip.publicid() == message->from() || gossip.publicid() == public_identifier_) {
			continue;
		}
		std::cout << "Heard gossip: " <<  gossip.publicid() << std::endl;
		get_networking()->ip_map().add_to_ip_map(gossip.publicid(), IpWrapper(gossip));
	}
}

void Peer::handle_gossip_one_way(shared_ptr_message message) {
	for (auto&& gossip : message->gossip().peer_ip_wrappers()) {
		if (gossip.publicid() == message->from() || gossip.publicid() == public_identifier_) {
			continue;
		}
		std::cout << "Heard gossip: " <<  gossip.publicid() << std::endl;
		get_networking()->ip_map().add_to_ip_map(gossip.publicid(), IpWrapper(gossip));
	}
}

void Peer::generate_gossip_request(pk_t to) {
	get_networking()->enroll_message_to_be_sent(
		MFW::ReqGossipFactory(
			MFW::GossipFactory(
				public_identifier_,
				to
			)
		)
	);
}

void Peer::generate_gossip_one_way_all() {
	pk_t to = get_networking()->ip_map().select_connected_randoms(3).front().first;
	generate_gossip_one_way(to);
}

void Peer::generate_gossip_one_way(pk_t to) {
	get_networking()->enroll_message_to_be_sent(
		MFW::OneWayGossipFactory(
			MFW::GossipFactory(
				public_identifier_,
				to
			),
			get_networking()->ip_map().select_connected_randoms(config.gossip_randoms)
		)
	);
}

void Peer::generate_news_refresh() {
	for (auto&& connected_peer : get_networking()->ip_map().select_connected(0)) {
		get_networking()->enroll_message_to_be_sent(
			MFW::ReqNewspaperListFactory(
				MFW::NewspaperListFactory(
					get_public_id(),
					connected_peer.first
				),
				get_news()
			)
		);
	}
}

void Peer::handle_article_all_one_way(shared_ptr_message message) {
	Article recv_article(message->article_all().header(), message->article_all().article_actual());
	recv_article.lazy_remove_readers(networking_->disconnected_readers_lazy_remove.users);
	NewspaperEntry& news = get_news(recv_article.news_id());

	/* Inform every reader, that we downloaded article successfully */

	// generate_successful_download_message_all_readers(
	// 	recv_article.const_readers(),
	// 	message->from(),
	// 	message->article_all().article_hash()
	// );

	// if (recv_article.get_ancestor() != 0) {
	// 	news.remove_article(recv_article.get_ancestor());
	// }

	enroll_new_article(recv_article, false);
}

void Peer::slot_set_config_peer_gossips(int value) {
	config.gossip_randoms = value;
}

void Peer::slot_set_config_peer_article_list_first(int value) {
	config.list_size_first = value;
}

void Peer::slot_set_config_peer_article_list_default(int value) {
	config.list_size_default = value;
}

void Peer::slot_set_config_news_no_read_articles(pk_t news_id, int value) {
	NewspaperEntry& news = get_news(news_id);
	news.set_config_read_articles_to_keep(value);
}

void Peer::slot_set_config_news_no_unread_articles(pk_t news_id, int value) {
	NewspaperEntry& news = get_news(news_id);
	news.set_config_unread_articles_to_keep(value);
}

int Peer::slot_get_config_peer_gossips() {
	return config.gossip_randoms;
}

int Peer::slot_get_config_peer_article_list_first() {
	return config.list_size_first;
}

int Peer::slot_get_config_peer_article_list_default() {
	return config.list_size_default;
}


int Peer::slot_get_config_news_no_read_articles(pk_t news_id) {
	NewspaperEntry& news = get_news(news_id);
	return news.get_config_read_articles_to_keep();
}

int Peer::slot_get_config_news_no_unread_articles(pk_t news_id) {
	NewspaperEntry& news = get_news(news_id);
	return news.get_config_unread_articles_to_keep();
}

void Peer::update_article(pk_t news_id, hash_t article_hash, std::string path_to_tmp_file) {
	NewspaperEntry& news = get_news(news_id);
	Article& ancestor = news.get_article(article_hash);
	Article new_article;
	new_article.initialize_article(ancestor.categories_ref(), path_to_tmp_file, *this, news, ancestor.get_version() + 1);
	new_article.set_ancestor(article_hash);
	new_article.set_creation_time(ancestor.creation_time());
	upload_external_article(new_article);
	enroll_new_article(new_article, false);
	emit signal_article_updated();
}

std::map<std::string, pk_t>& Peer::get_pending_journalists() {
	return pending_journalist_requests;
}

void Peer::load_news_from_file(std::string path) {
	std::ifstream news_file(path);
	std::stringstream gpb_news_stringstream;
	gpb_news_stringstream << news_file.rdbuf();
	np2ps::LocalSerializedNewspaperEntry gpb_news;
	gpb_news.ParseFromString(gpb_news_stringstream.str());
	add_new_newspaper(
		NewspaperEntry(gpb_news, &get_networking()->disconnected_readers_lazy_remove),
		QHostAddress(gpb_news.network_info().ipv4()),
		gpb_news.network_info().port(),
		gpb_news.network_info().stun_port(),
		false
	);
	generate_article_list_message(gpb_news.entry().news_id());
	emit newspaper_list_received();
}

void Peer::save_news_to_file(std::string path, pk_t news_id, QHostAddress address) {
	NewspaperEntry& entry = get_news(news_id);
	np2ps::LocalSerializedNewspaperEntry gpb_news;
	entry.local_serialize_entry(&gpb_news, false);
	gpb_news.mutable_network_info()->set_ipv4(address.toIPv4Address());
	std::ofstream news_file(path);
	std::string gpb_news_string;
	gpb_news.SerializeToString(&gpb_news_string);
	news_file << gpb_news_string;
}

void Peer::slot_set_config_peer_article_list_first_percent(int value) {
	config.first_percent_autodownload = value;
}

void Peer::slot_set_config_peer_article_list_default_percent(int value) {
	config.default_percent_autodownload = value;
}

int Peer::slot_get_config_peer_article_list_first_percent() {
	return config.first_percent_autodownload;
}

int Peer::slot_get_config_peer_article_list_default_percent() {
	return config.default_percent_autodownload;
}

int Peer::slot_get_config_peer_article_list_first_total() {
	return config.list_size_default * config.default_percent_autodownload / 100;
}

int Peer::slot_get_config_peer_article_list_default_total() {
	return config.list_size_first * config.first_percent_autodownload / 100;
}

void Peer::generate_ping_one_way(pk_t to) {
	get_networking()->enroll_message_to_be_sent(
		MFW::SetMessageContextOneWay(
			MFW::PingFactory(
				get_public_id(),
				to
			)
		)
	);
}

void Peer::ping_direct_peers() {
	auto direct_peers = get_networking()->ip_map().select_direct(-1);
	for (auto &[peer_id, peer_wrapper] : direct_peers) {
		generate_ping_one_way(peer_id);
	}
}

void Peer::clear_abundant_articles() {
	std::function<void(pk_t, pk_t, hash_t)> remove_article_callback = [&] (pk_t news, pk_t author, hash_t article_id) {
		generate_article_removed_message(news, author, article_id);
	};
	for (auto&& news : news_) {
		news.second.clear_abundant_articles(remove_article_callback);
	}
}

void Peer::generate_article_removed_message(pk_t news_id, pk_t author, hash_t article_id) {
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextOneWay(
			MFW::ArticleDataChangeFactory(
				get_public_id(),
				news_id,
				article_id,
				false
			)
		)
	);
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextOneWay(
			MFW::ArticleDataChangeFactory(
				get_public_id(),
				author,
				article_id,
				false
			)
		)
	);

	Article& article = get_news(news_id).get_article(article_id);
	for (auto&& reader : article.readers()) {
		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextOneWay(
				MFW::ArticleDataChangeFactory(
					get_public_id(),
					reader,
					article_id,
					false
				)
			)
		);
	}
}

void Peer::generate_newspaper_list_request_connected() {
	auto connected_peers = get_networking()->ip_map().select_connected(0);
	for (auto& [peer_id, peer] : connected_peers) {
		generate_newspaper_list_request(peer_id);
	}
}

void Peer::handle_ping_one_way(shared_ptr_message message) {
	pk_t pinging_from = message->from();

	/* we need to re-establish a STUN connection */
	get_networking()->get_stun_client()->allocate_request(pinging_from);

	/* long time no see; now we can resend waiting messages */
	get_networking()->resend_np2ps_messages_waiting_for_peer_online_long_term(pinging_from);
}

void Peer::store_newspaper_potential(pk_t news_id, news_potential_tuple entry) {
	news_potential.emplace(news_id, entry);
}

void Peer::clear_newspaper_potential() {
	news_potential.clear();
}

news_potential_db& Peer::get_newspaper_potential() {
	return news_potential;
}
