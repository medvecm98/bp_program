#include "Peer.h"

/**
 * @brief Adds new article to its corresponding newspaper.
 * 
 * @param a Article to add.
 */
void Peer::enroll_new_article(Article a, bool header_only) {
	news_[a.news_id()].add_article(a.main_hash(),std::move(a));
	emit checked_display_article(a.news_id(), a.main_hash());
	emit new_article_list(a.news_id());
}

void Peer::identify_newspaper(QHostAddress address, const std::string& newspaper_name) {
	networking_->newspapers_awaiting_identification.emplace(address.toIPv4Address(), newspaper_name);
	networking_->get_stun_client()->identify(address);
}

void Peer::newspaper_identified(pk_t newspaper_key, my_string newspaper_name, std::string newspaper_ip_domain) {
	add_new_newspaper(newspaper_key, newspaper_name, newspaper_ip_domain);
}

/**
 * Adds new newspaper to the collection.
 * @param newspaper_key Public key of newspaper.
 * @param newspaper_ip_domain IP, or domain name, of the newspaper.
 */
void Peer::add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, const std::string &newspaper_ip_domain, bool allocate_now) {
	networking_->ip_map_.add_to_map(newspaper_key, IpWrapper(newspaper_ip_domain));

	auto news = NewspaperEntry(newspaper_key, newspaper_key, newspaper_name, &networking_->disconnected_readers_lazy_remove);
	news.await_confirmation = true;

	newspapers_awaiting_confirmation.emplace(newspaper_key, std::move(news));
	
	if (allocate_now) {
		networking_->get_stun_client()->allocate_request(newspaper_key);
	}
}

/**
 * Adds new newspaper to the collection.
 * @param newspaper_key Public key of newspaper.
 * @param newspaper_ip_domain IP of the newspaper.
 */
NewspaperEntry& Peer::add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, QHostAddress&& newspaper_ip_domain, bool allocate_now) {
	networking_->ip_map_.add_to_map(newspaper_key, IpWrapper(newspaper_ip_domain));
	if (allocate_now) {
		networking_->get_stun_client()->allocate_request(newspaper_key);
		return newspapers_awaiting_confirmation.emplace(newspaper_key, NewspaperEntry(newspaper_key, newspaper_key, newspaper_name, &networking_->disconnected_readers_lazy_remove)).first->second;
	}
	else {
		return news_.emplace(newspaper_key, NewspaperEntry(newspaper_key, newspaper_key, newspaper_name, &networking_->disconnected_readers_lazy_remove)).first->second;
	}
}

/**
 * Adds new newspaper to the collection.
 * @param newspaper_key Public key of newspaper.
 * @param sender Sender of this NewspaperEntry.
 */
void Peer::add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, pk_t sender) {
	networking_->get_stun_client()->identify(newspaper_key, sender);
	NewspaperEntry ne(newspaper_key, newspaper_key, newspaper_name, &networking_->disconnected_readers_lazy_remove);
	ne.add_friend(sender);
	newspapers_awaiting_confirmation.emplace(newspaper_key, std::move(ne));
}

NewspaperEntry& Peer::add_new_newspaper(NewspaperEntry&& newspaper_entry, QHostAddress&& address, bool allocate_now) {
	pk_t news_pid = newspaper_entry.get_id();
	networking_->ip_map().add_to_map(news_pid, IpWrapper(address));
	if (allocate_now) {
		networking_->get_stun_client()->allocate_request(news_pid);
		return newspapers_awaiting_confirmation.emplace(
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
	auto ne = newspapers_awaiting_confirmation.find(pid);
	if (ne == newspapers_awaiting_confirmation.end()) { //no newspaper were awaiting confirmation with this pid
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
	
	newspapers_awaiting_confirmation.erase(ne);
	emit got_newspaper_confirmation(pid);
}

/**
 * @brief Loads the IPs of all the authorities of given newspaper key.
 * 
 * Message is sent to newspaper chief editor.
 * 
 * @param newspaper_key Newspaper public ID for which we need the authorities.
 */
void Peer::load_ip_authorities(pk_t newspaper_key) {
	throw deprecated_feature("Loading IP authorities is deprecated.");
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

/**
 * List of all articles that given news have, selecting only provided categories.
 * Chief editors only.
 * @param articles Where to put the articles.
 * @param categories What categories to use.
 * @return How many articles were found.
 */
size_t Peer::list_all_articles_from_news(article_container &articles, const std::set<category_t> &categories) {
	size_t article_counter = 0;
	for (auto&& cat : articles_categories_) {
		if (categories.empty() || (categories.find(cat.first) != categories.end())) {
			articles.insert(&cat.second->article);
			article_counter++;
		}
	}
	return article_counter;
}

/**
 * List of all articles that given news have.
 * Chief editors only.
 * @param articles Where to put the articles.
 * @return How many articles were found.
 */
size_t Peer::list_all_articles_from_news(article_container &articles) {
	size_t article_counter = 0;
	
	auto& news = news_[get_my_news_id()];
	auto bit = news.get_iterator_database();
	auto eit = news.get_iterator_database_end();

	for (; bit != eit; bit++) {
		articles.insert(&bit->second);
		article_counter++;
	}

	return article_counter;
}

/**
 * List of all articles that given news have.
 * Chief editors only.
 * @param articles Where to put the articles.
 * @return How many articles were found.
 */
size_t Peer::list_all_articles_from_news(article_container &articles, pk_t newspaper_id, int count) {
	size_t article_counter = 0;
	
	auto& news = get_news(newspaper_id);
	if (count > 0) {
		auto [bit, eit] = news.get_newest_articles(count); 
		while (bit != eit) {
			Article& article = news.get_article(bit->second);
			articles.emplace(&article);
		}
		return 1;
	}
	else {
		auto bit = news.get_iterator_database();
		auto eit = news.get_iterator_database_end();

		for (; bit != eit; bit++) {
			articles.insert(&bit->second);
			article_counter++;
		}

		return article_counter;
	}
}

/**
 * List of all articles that given news have.
 * Chief editors only.
 * @param articles Where to put the articles.
 * @return How many articles were found.
 */
size_t Peer::list_all_articles_from_news(article_container &articles, pk_t newspaper_id, int count, QDate date) {
	size_t article_counter = 0;
	
	auto& news = get_news(newspaper_id);
	if (count > 0) {
		auto [bit, eit] = news.get_newest_articles(date, count); 
		while (bit != eit) {
			Article& article = news.get_article(bit->second);
			articles.emplace(&article);
		}
		return 1;
	}
	else {
		auto bit = news.get_iterator_database();
		auto eit = news.get_iterator_database_end();

		for (; bit != eit; bit++) {
			articles.insert(&bit->second);
			article_counter++;
		}

		return article_counter;
	}
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

/**
 * @brief Returns pointer to ArticleReaders, if it was found in main category, article, readers database.
 * 
 * @param article_hash Hash of article.
 * @return std::optional. AuthorPeer pointer, if it was found, no value, if it didn't.
 */
optional_author_peers Peer::find_article_in_article_categories_db(hash_t article_hash, category_container categories) {
	for (auto&& cat : categories) {
		if (articles_categories_.find(cat) != articles_categories_.end()) {
			auto bucket_begin = articles_categories_.begin(articles_categories_.bucket(cat));
			auto bucket_end = articles_categories_.end(articles_categories_.bucket(cat));
			for (; bucket_begin != bucket_end; bucket_begin++) {
				if (bucket_begin->second->article.main_hash() == article_hash) {
					return optional_author_peers(bucket_begin->second);
				}
			}
		}
	}
	return {};
}

/**
 * @brief Returns pointer to ArticleReaders, if it was found in main category, article, readers database.
 * 
 * @param article_hash Hash of article.
 * @return std::optional. AuthorPeer pointer, if it was found, no value, if it didn't.
 */
optional_author_peers Peer::find_article_in_article_categories_db(hash_t article_hash) {
	for (auto&& cat : articles_categories_) {
		auto bucket_begin = articles_categories_.begin(articles_categories_.bucket(cat.first));
		auto bucket_end = articles_categories_.end(articles_categories_.bucket(cat.first));
		for (; bucket_begin != bucket_end; bucket_begin++) {
			if (bucket_begin->second->article.main_hash() == article_hash) {
				return optional_author_peers(bucket_begin->second);
			}
		}
	}
	return {};
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

	/* Ask author */

	auto& news = get_news(news_id);
	auto& article = news.get_article(article_hash);
	pk_t author_id = article.author_id();
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextRequest(
			MFW::ArticleDownloadFactory(
				public_identifier_,
				author_id,
				article_hash,
				255
			)
		)
	);

	/* Ask article readers, if any */

	if (article.readers_count() > 0) {
		std::cout << "Readers: " << article.readers_count() << std::endl;
		for (auto&& reader_id : article.readers()) {
			if (get_networking()->ip_map().has_wrapper(reader_id)) {
				auto& wrapper = get_networking()->ip_map().get_wrapper_ref(reader_id);
				std::cout << "  reader: " << reader_id << std::endl;
				if (reader_id == author_id
					|| reader_id == public_identifier_
					|| !wrapper.np2ps_socket_connected()
				) {
					continue;
				}
				networking_->enroll_message_to_be_sent(
					MFW::SetMessageContextRequest(
						MFW::ArticleDownloadFactory(
							public_identifier_,
							reader_id,
							article_hash,
							255
						)
					)
				);
			}
		}
	}
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
void Peer::generate_article_list_message(pk_t newspaper_id) {
		getting_article_list.insert(newspaper_id);
		emit check_selected_item();

		auto& news = get_news(newspaper_id);
		const user_container& news_friends = news.get_friends();

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

		for (auto&& user : news_friends) { //send the same request to all the friends
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
		if (article_opt.value()->article_present()) { //whole article is present in the database
			std::string article_whole;

			article_whole = article->read_contents();
			article->add_reader(message->from());

			shared_ptr_message article_msg = MFW::RespArticleDownloadFactory(
				MFW::ArticleDownloadFactory(
					public_identifier_, 
					message->from(), 
					article_opt.value()->main_hash(), 
					255
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
						article_opt.value()->main_hash(),
						255
					),
					article_opt.value()
				)
			);
		}
	}
	else {
		//article not found in database
		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextError(
				MFW::ArticleDownloadFactory(
					message->from(),
					public_identifier_,
					article_opt.value()->main_hash(),
					255
				)
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

	try {
		auto& news_ref = get_news(req_news_id);

		article_container articles;
		// list_all_articles_from_news(articles);
		for (std::size_t i = 0; i < news_ref.get_all_articles().size(); i += config.list_size_default) {
			news_ref.get_newest_articles(
				articles,
				i,
				i + config.list_size_default,
				message->article_list().timestamp()
			);

			networking_->enroll_message_to_be_sent(
				MFW::RespArticleListFactory(
					MFW::ArticleListFactory(
						public_identifier_,
						message->from()
					),
					articles
				)
			);
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
	// //authority part
	// auto article_author_peers = find_article_in_article_categories_db(message->article_data_update().article_pk());
	// if (article_author_peers.has_value()) {
	// 	//AuthorPeer entry for given article exists

	// 	auto user = user_map.find(message->from());
	// 	if (message->article_data_update().article_action() == np2ps::DOWNLOAD) {
	// 		if (user == user_map.end()) {
	// 			auto ins = user_map.insert({message->from(), PeerInfo(message->from(), 127)});
	// 			article_author_peers.value()->readers.insert({ins.first->first, &(ins.first->second)});
	// 		}
	// 		else {
	// 			article_author_peers.value()->readers.insert({user->first, &(user->second)});
	// 		}
	// 	}
	// 	else if (message->article_data_update().article_action() == np2ps::REMOVAL) {
	// 		article_author_peers.value()->readers.erase(message->from());
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

	if (get_networking()->ip_map().have_rsa_public(message->from())) {
		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextError(
				MFW::CredentialsFactory(
					public_identifier_,
					message->from()
				)
			)
		);
	}
	else {
		bool req_my_credentials = message->credentials().ipv4()[0] != 'r';
		pk_t requested_credentials = 0;

		if (req_my_credentials)
			requested_credentials = QString(message->credentials().ipv4().c_str()).mid(1).toULongLong();

		if (message->credentials().req_ipv4()) {
			if (req_my_credentials)
				resp_ip4 = networking_->ip_map_.my_ip().ipv4.toString();
			else if (networking_->ip_map_.have_ip4(requested_credentials))
				resp_ip4 = networking_->ip_map_.get_ip4(requested_credentials).toString();
		}
		if (message->credentials().req_ipv6()) {
			if (req_my_credentials)
				resp_ip6 = networking_->ip_map_.my_ip().ipv6.toString();
			else if (networking_->ip_map_.have_ip6(requested_credentials))
				resp_ip6 = networking_->ip_map_.get_ip6(requested_credentials).toString();
		}
		if (message->credentials().req_rsa_public_key()) {
			if (req_my_credentials)
				resp_rsa_public = networking_->ip_map_.my_ip().get_rsa();
			else if (networking_->ip_map_.have_rsa_public(requested_credentials))
				resp_rsa_public = networking_->ip_map_.get_wrapper_ref(requested_credentials).get_rsa();
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

		if (message->credentials().has_rsa_public_key()) {
			IpWrapper& wrapper = get_networking()->ip_map().get_wrapper_ref(message->from());
			auto hex_key = message->credentials().rsa_public_key().key();
			wrapper.set_rsa_hex_string(hex_key);
		}

		CredentialsPayload payload;
		payload.set_private(get_networking()->ip_map().private_rsa);
		payload.set_public(get_networking()->ip_map().get_wrapper_ref(message->from()).get_rsa_optional());

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

			if (article.verify(message->article_all().article_actual())
				&& article.verify_news_signature(news.get_newspaper_public_key())
			) {

				/* Successful verification */

				std::cout << "Article verification succeded for " << recv_article_id << std::endl;
				
				article.add_readers(message->article_all().header().readers());

				if (article.is_header_only()) {

					/* Article in database contains only the header */
					
					Article article_from_message(message->article_all().header(), "");
					article.update_metadata(article_from_message);
					article.set_path(message->article_all().article_actual());

					generate_successful_download_message_all_readers(
						article.const_readers(),
						message->from(),
						recv_article_id
					);

					emit checked_display_article(message->article_all().header().news_id(), article.main_hash());
				}
				else if (article.modification_time() <= message->article_all().header().modification_time()) {

					/* Received article is newer than one in database */

					get_news(message->article_all().header().news_id()).remove_article(message->article_all().header().main_hash());
					Article recv_article(message->article_all().header(), message->article_all().article_actual());
					enroll_new_article(recv_article, false);
				}
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

				return;
			}
		}
		else {

			/* Article not found in database. */

			Article recv_article(message->article_all().header(), message->article_all().article_actual());
			recv_article.lazy_remove_readers(networking_->disconnected_readers_lazy_remove.users);

			/* Inform every reader, that we downloaded article successfully */

			generate_successful_download_message_all_readers(
				recv_article.const_readers(),
				message->from(),
				recv_article_id
			);

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
		if (reader == from || reader == get_public_key() || friends_.count(reader) > 0) {
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
		auto& present_article = news.get_article(article.main_hash());
		if (present_article.modification_time() >= article.modification_time()) { //if received article is newer, replace it
			news.remove_article(article.main_hash());
			add_to_news();
		}
		else {
			present_article.add_reader(message->from());
		}
	}
	catch (article_not_found_database& e) { //if it wasn't found, add it
		add_to_news();
	}
}

void Peer::handle_article_list_response(shared_ptr_message message) {
	auto list_size = message->article_list().response_size();
	if (list_size != 0) {
		std::cout << "Received article list size: " << list_size << std::endl;
		pk_t list_news_id = message->article_list().response().begin()->news_id();
		auto& news = get_news(list_news_id);
		for (auto it = message->article_list().response().begin(); it != message->article_list().response().end(); it++) {
			check_add_article_to_news(message, news, Article(*it));
		}
		news.update();
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
			networking_->ip_map_.update_ip((pk_t)message->from(), QHostAddress(QString(message->credentials().ipv4().c_str())));
		}
	}
	if (message->credentials().req_rsa_public_key()) {
		networking_->ip_map_.update_rsa_public((pk_t)message->from(), message->credentials().rsa_public_key().key());
	}
	if (message->credentials().req_eax_key()) {
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
		networking_->ip_map_.update_eax((pk_t)message->from(), dec_key);
	}

	std::multimap<pk_t, shared_ptr_message>& messages_waiting_for_credentials = get_networking()->messages_waiting_for_credentials;
	if (messages_waiting_for_credentials.count(message->from()) > 0) {
		auto [bit, eit] = messages_waiting_for_credentials.equal_range(message->from());
		auto message = bit->second;
		messages_waiting_for_credentials.erase(bit);

		networking_->enroll_message_to_be_sent(message);
	}
}

void Peer::handle_public_key_response(shared_ptr_message message) {
	networking_->ip_map_.update_rsa_public((pk_t)message->from(), message->public_key().key());
	if (newspapers_awaiting_confirmation.find((pk_t)message->from()) != newspapers_awaiting_confirmation.end()) {
		news_.insert({message->from(), newspapers_awaiting_confirmation[(pk_t)message->from()]});
		newspapers_awaiting_confirmation.erase(message->from());
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
	// //authority part
	// auto article_author_peers = find_article_in_article_categories_db(message->article_data_update().article_pk());
	// if (article_author_peers.has_value()) {
	// 	//AuthorPeer entry for given article exists

	// 	auto user = user_map.find(message->from());
	// 	if (message->article_data_update().article_action() == np2ps::DOWNLOAD) {
	// 		if (user == user_map.end()) {
	// 			auto ins = user_map.insert({message->from(), PeerInfo(message->from(), 127)});
	// 			article_author_peers.value()->readers.insert({ins.first->first, &(ins.first->second)});
	// 		}
	// 		else {
	// 			article_author_peers.value()->readers.insert({user->first, &(user->second)});
	// 		}
	// 	}
	// 	else if (message->article_data_update().article_action() == np2ps::REMOVAL) {
	// 		article_author_peers.value()->readers.erase(message->from());
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
	news.remove_friend(msg->from());
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

void Peer::upload_external_article(Article a) {
	enroll_new_article(a, false);
	networking_->enroll_message_to_be_sent(
		MFW::ReqArticleHeaderFactory (
			MFW::ArticleHeaderFactory(
				public_identifier_,
				a.news_id(),
				a.main_hash()
			),
			&a
		)
	);
}

void Peer::handle_article_all_error(shared_ptr_message message) {
	auto article_id = message->article_all().header().main_hash();
	auto news_id = message->article_all().header().news_id();

	auto& news = get_news(news_id);
	auto& article = news.get_article(article_id);

	/* Remove the reader, since he doesn't have the requested article. */

	article.remove_reader(message->from());
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
		news.remove_friend(message->from());
	}
	catch (unknown_newspaper_error& e) {
	}
}

void Peer::handle_newspaper_list_request(shared_ptr_message message) {
	networking_->enroll_message_to_be_sent(
		MFW::RespNewspaperListFactory(
			MFW::NewspaperListFactory(
				get_public_key(),
				message->from()
			),
			get_news(),
			get_networking()->ip_map()
		)
	);
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
		std::string ip, port, pk;
		if (!std::getline(ss, ip, ':') ||
		    !std::getline(ss, port, ':') ||
		    !std::getline(ss, pk)) 
		{
			throw other_error("Invalid newspaper file format.");
		}
		else {
			pk_t id = std::stoll(pk);
			news.add_friend(id);
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
		for (auto&& f : n.second.get_friends()) {
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
				get_public_key(),
				destination
			)
		)
	);
}

void Peer::handle_newspaper_list_response(shared_ptr_message message) {
	if (!message->has_newspaper_list()) {
		throw malformed_message_context_or_type("Message does not contain newspaper list.");
	}

	for (auto it = message->newspaper_list().news().begin(); it != message->newspaper_list().news().end(); it++) {
		auto check_articles = [&](NewspaperEntry& news){
			if (it->articles_size() > 0) {
				for (auto article_it = it->articles().begin(); article_it != it->articles().end(); article_it++) {
					check_add_article_to_news(message, news, Article(*article_it));
				}
			}	
		};

		if (!find_news(it->entry().news_id())) {
			auto& news = add_new_newspaper(
				NewspaperEntry(*it, &networking_->disconnected_readers_lazy_remove),
				QHostAddress(it->network_info().ipv4())
			);
			check_articles(news);
		}
		else {
			auto& news = get_news(it->entry().news_id());
			check_articles(news);
		}
	}

	emit newspaper_list_received();
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
template<typename Container>
void Peer::generate_article_list_message(pk_t destination, const Container& categories) {
	networking_->enroll_message_to_be_sent(
		MFW::ReqArticleListFactory<Container>(
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
pk_t Peer::get_public_key() {
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

my_string Peer::name() {
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

void Peer::serialize(np2ps::Peer* p) {
	p->set_name(name_);
	p->set_public_identifier(public_identifier_);
	auto ip_map_gpb = p->mutable_ip_map();
	networking_->ip_map_.serialize_ip_map(ip_map_gpb);
	for (auto&& newspapers : news_) {
		auto ne_gpb = p->add_news();
		NewspaperEntry& ne = newspapers.second;
		ne.local_serialize_entry(ne_gpb);
	}
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
	if (newspapers_awaiting_confirmation.size() > 0) {
		get_networking()->get_stun_client()->allocate_request(newspapers_awaiting_confirmation.begin()->first);
	}
}

void Peer::generate_new_journalist(pk_t pid) {
	auto& my_news = get_my_newspaper();
	get_networking()->enroll_message_to_be_sent(
		MFW::ReqJournalistFactory(
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
		if (journalist == public_identifier_) {
			continue;
		}
		get_networking()->enroll_message_to_be_sent(
			MFW::OneWayNewJournalistFactory(
				MFW::NewJournalistFactory(
					public_identifier_,
					journalist
				),
				journalist,
				get_networking()->ip_map().get_wrapper_ref(journalist)
			)
		);
	}
	my_news.emplace_journalist(pid);
}

void Peer::handle_journalist_request(shared_ptr_message message) {
	try {
		NewspaperEntry& news = get_news(message->journalist().entry().entry().news_id());
		news.set_newspaper_private_key(
			CryptoUtils::instance().hex_to_private(
				message->journalist().private_key()
			)
		);
		for (auto&& journalist : message->journalist().entry().journalists()) {
			news.emplace_journalist(journalist.publicid());
			IpWrapper wrapper(journalist);
			get_networking()->ip_map().add_to_ip_map(journalist.publicid(), wrapper);
		}
		news.emplace_journalist(public_identifier_);
		journalist_of.emplace(news.get_id());
		get_networking()->enroll_message_to_be_sent(
			MFW::RespJournalistFactory(
				MFW::JournalistFactory(
					public_identifier_,
					message->from()
				)
			)
		);
	}
	catch (unknown_newspaper_error& err) {
		std::cout << err.what() << std::endl;
		get_networking()->enroll_message_to_be_sent(
			MFW::ErrorJournalistFactory(
				MFW::JournalistFactory(
					public_identifier_,
					message->from()
				)
			)
		);
	}
}

void Peer::handle_journalist_response(shared_ptr_message message) {
	auto& my_news = get_my_newspaper();
	my_news.emplace_journalist(message->from());
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
	for (auto&& news_id : journalist_of) {
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
		std::cout << "Heard gossip: " <<  gossip.publicid() << std::endl;
		get_networking()->ip_map().add_to_ip_map(gossip.publicid(), IpWrapper(gossip));
	}
}

void Peer::handle_gossip_one_way(shared_ptr_message message) {
	for (auto&& gossip : message->gossip().peer_ip_wrappers()) {
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

void Peer::generate_gossip_one_way() {
	pk_t to = get_networking()->ip_map().select_connected_randoms(1).front().first;
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
