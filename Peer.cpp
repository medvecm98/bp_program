#include "Peer.h"

/**
 * @brief Adds new article to its corresponding newspaper.
 * 
 * @param a Article to add.
 */
void Peer::enroll_new_article(Article a, bool header_only) {
	news_[a.news_id()].add_article(a.main_hash(),std::move(a));
	if (!header_only) {
		PeerInfo* peer_info = &user_map[public_identifier_];
		readers_.emplace(a.main_hash(), peer_info);
	}
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
void Peer::add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, const std::string &newspaper_ip_domain) {
	networking_->ip_map_.add_to_map(newspaper_key, IpWrapper(newspaper_ip_domain));
	networking_->get_stun_client()->allocate_request(newspaper_key);

	newspapers_awaiting_confirmation.emplace(newspaper_key, NewspaperEntry(newspaper_key, newspaper_key, newspaper_name));
}

/**
 * Adds new newspaper to the collection.
 * @param newspaper_key Public key of newspaper.
 * @param sender Sender of this NewspaperEntry.
 */
void Peer::add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, pk_t sender) {
	networking_->get_stun_client()->identify(newspaper_key, sender);
	NewspaperEntry ne(newspaper_key, newspaper_key, newspaper_name);
	ne.add_friend(sender);
	newspapers_awaiting_confirmation.emplace(newspaper_key, std::move(ne));
}

void Peer::newspaper_confirm(pk_t pid) {
	auto ne = newspapers_awaiting_confirmation.find(pid);
	if (ne == newspapers_awaiting_confirmation.end()) //no newspaper were awaiting confirmation with this pid
		return;
	news_.emplace(pid, ne->second);
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
	networking_->enroll_message_to_be_sent(
		MFW::ReqCredentialsFactory(
			MFW::CredentialsFactory(
				public_identifier_, 
				newspaper_key
			),
			true, false, false, false,
			string_ptr_optional(std::make_shared<std::string>(networking_->ip_map_.my_ip.ipv4.toString().toStdString())),
			string_ptr_optional(), rsa_public_ptr_optional(), eax_ptr_optional()
		)
	);
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
	news_.insert({newspaper_id_, NewspaperEntry(public_identifier_, newspaper_id_, newspaper_name_)}); //our news are in same db as all the others
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
	if (message->msg_ctx() == np2ps::REQUEST) {
		handle_requests( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::RESPONSE) {
		handle_responses( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::ONE_WAY) {
		handle_one_way( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::ERROR) {
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
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextRequest(
			MFW::ArticleDownloadFactory(
				public_identifier_,
				article.author_id(),
				article_hash,
				255
			)
		)
	);

	/* Ask article readers, if any */

	if (article.readers_count() > 0) {
		for (auto&& reader_id : article.readers()) {
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

		auto& news = get_news_db();
		const user_container& news_friends = news[newspaper_id].get_friends();

		networking_->enroll_message_to_be_sent(
			MFW::ReqArticleListFactory(
				MFW::ArticleListFactory(
					public_identifier_,
					newspaper_id
				),
				newspaper_id,
				10,
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
					10,
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
	auto article = find_article(message->article_all().article_hash());

	if (article.has_value()) { //at least article header is in database
		if (article.value()->article_present()) { //whole article is present in the database
			std::string article_whole;

			article_whole = article.value()->read_contents();

			shared_ptr_message article_msg = MFW::RespArticleDownloadFactory(
				MFW::ArticleDownloadFactory(
					public_identifier_, 
					message->from(), 
					article.value()->main_hash(), 
					255),
				article.value(), 
				std::move(article_whole)
			);

			article.value()->add_reader(message->to());
			
			bool found_in_readers = false;
			auto [bit, eit] = readers_.equal_range(message->article_all().article_hash());
			for (; bit != eit; bit++) {
				if (bit->second->peer_key == message->from()) {
					found_in_readers = true;
					break;
				}
			}
			if (!found_in_readers) {
				readers_.emplace(message->article_all().article_hash(), &user_map[message->from()]);
			}

			//send message
			networking_->enroll_message_to_be_sent(article_msg);
		}
		else { //only article header is present in article database
			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextError(
					MFW::ArticleDownloadFactory(
						message->from(),
						public_identifier_,
						article.value()->main_hash(),
						255
					)
				)
			);
		}
	}
	else {
		//article not found in database
		if (message->article_all().header().author_id() == public_identifier_ || 
			message->article_all().header().news_id() == public_identifier_) 
		{ //if we are requested as an author of a chief editor
			std::vector<pk_t> article_peers;
			auto [readers_begin, readers_end] = readers_.equal_range(message->article_all().article_hash());
			if (readers_begin != readers_end) { //at least one reader exists
				for (; readers_begin != readers_end; readers_begin++) {
					article_peers.push_back(readers_begin->second->peer_key);
				}
				networking_->enroll_message_to_be_sent(
					MFW::SetMessageContextOneWay(
						MFW::ArticleSolicitationFactory(
							public_identifier_,
							message->from(),
							message->article_all().article_hash(),
							article_peers,
							message->article_all().level()
						)
					)
				);
			}
			else if (!journalists_.empty()) { //there are some journalists present
				//TODO: redo if ArtileReaders and such are properly implemented

				for (auto&& journalist : journalists_) {
					article_peers.push_back(journalist);
				}
				networking_->enroll_message_to_be_sent(
					MFW::SetMessageContextOneWay(
						MFW::ArticleSolicitationFactory(
							public_identifier_,
							message->from(),
							message->article_all().article_hash(),
							article_peers,
							message->article_all().level()
						)
					)
				);
			}
			else {
				networking_->enroll_message_to_be_sent(
					MFW::SetMessageContextError(
						MFW::ArticleSolicitationFactory(
							public_identifier_,
							message->from(),
							message->article_all().article_hash(),
							std::vector<pk_t>(),
							0
						)
					)
				);
			}
		}
		else { //only article header is present in article database
			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextError(
					MFW::ArticleDownloadFactory(
						message->from(),
						public_identifier_,
						article.value()->main_hash(),
						255
					)
				)
			);
		}
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
		list_all_articles_from_news(articles);

		networking_->enroll_message_to_be_sent(
			MFW::RespArticleListFactory(
				MFW::ArticleListFactory(
					public_identifier_,
					message->from()
				),
				articles
			)
		);
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
	//reporter part
	if (find_article(message->article_data_update().article_pk()).has_value()) {
		if (message->article_data_update().article_action() == np2ps::DOWNLOAD) {
			if (user_map.find(message->from()) == user_map.end()) {
				auto uit = user_map.insert( {message->from(), PeerInfo(message->from())} );
				readers_.insert( {message->from(), &(uit.first->second)} );
			}
			else {
				auto uit = user_map.find(message->from());
				readers_.insert( {message->from(), &(uit->second)} );
			}
		}
		else if (message->article_data_update().article_action() == np2ps::REMOVAL) {
			auto [bit, eit] = readers_.equal_range(message->article_data_update().article_pk());
			for (auto it = bit; it != eit; it++) {
				if (it->second->peer_key == message->from()) {
					readers_.erase(it);
					break;
				}
			}
		}
	}
	//authority part
	auto article_author_peers = find_article_in_article_categories_db(message->article_data_update().article_pk());
	if (article_author_peers.has_value()) {
		//AuthorPeer entry for given article exists

		auto user = user_map.find(message->from());
		if (message->article_data_update().article_action() == np2ps::DOWNLOAD) {
			if (user == user_map.end()) {
				auto ins = user_map.insert({message->from(), PeerInfo(message->from(), 127)});
				article_author_peers.value()->readers.insert({ins.first->first, &(ins.first->second)});
			}
			else {
				article_author_peers.value()->readers.insert({user->first, &(user->second)});
			}
		}
		else if (message->article_data_update().article_action() == np2ps::REMOVAL) {
			article_author_peers.value()->readers.erase(message->from());
		}
	}
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
	std::shared_ptr<rsa_public_optional> resp_rsa_public;
	std::shared_ptr<eax_optional> resp_eax;

	bool req_my_credentials = message->credentials().ipv4()[0] != 'r';
	pk_t requested_credentials = 0;
	if (req_my_credentials)
		requested_credentials = QString(message->credentials().ipv4().c_str()).mid(1).toULongLong();

	if (message->credentials().req_ipv4()) {
		if (req_my_credentials)
			resp_ip4 = networking_->ip_map_.my_ip.ipv4.toString();
		else if (networking_->ip_map_.have_ip4(requested_credentials))
			resp_ip4 = networking_->ip_map_.get_ip4(requested_credentials).toString();
	}
	if (message->credentials().req_ipv6()) {
		if (req_my_credentials)
			resp_ip6 = networking_->ip_map_.my_ip.ipv6.toString();
		else if (networking_->ip_map_.have_ip6(requested_credentials))
			resp_ip6 = networking_->ip_map_.get_ip6(requested_credentials).toString();
	}
	if (message->credentials().req_rsa_public_key()) {
		if (req_my_credentials)
			resp_rsa_public = std::make_shared<rsa_public_optional>(networking_->ip_map_.my_ip.key_pair.first);
		else if (networking_->ip_map_.have_rsa_public(requested_credentials))
			resp_rsa_public = networking_->ip_map_.get_rsa_public(requested_credentials);
	}
	if (message->credentials().req_eax_key()) {
		if (networking_->ip_map_.have_eax(message->from())) {
			networking_->generate_symmetric_key();
		}
		resp_eax = networking_->ip_map_.get_eax(message->from());
	}

	networking_->enroll_message_to_be_sent(
		MFW::RespCredentialsFactory(
			MFW::CredentialsFactory(
				public_identifier_,
				message->from()
			),
			resp_ip4, resp_ip6, resp_rsa_public, resp_eax
		)
	);
}


void Peer::handle_article_all_response(shared_ptr_message message) {
	if (message->has_article_all() && message->article_all().has_header() && message->article_all().has_article_actual()) {
		hash_t recv_article_id = message->article_all().article_hash();
		auto article_opt = find_article(recv_article_id);

		if (article_opt.has_value()) {

			/* Article (header) found in database */
			/* Verify article */

			if (article_opt.value()->verify(message->article_all().article_actual())) {

				/* Successful verification */

				std::cout << "Article verification succeded for " << recv_article_id << std::endl;
				if (article_opt.value()->is_header_only()) {

					/* Article in database contains only the header */

					article_opt.value()->set_path(message->article_all().article_actual());
				}
				else if (article_opt.value()->modification_time() <= message->article_all().header().modification_time()) {

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
			enroll_new_article(recv_article, false);
		}

		/* Article is now downloaded. */

		downloading_articles.erase(recv_article_id);
		emit check_selected_item();


		if (networking_->soliciting_articles.find(recv_article_id) != networking_->soliciting_articles.end()) { //remote the article from soliticing articles, since we got an answer
			networking_->soliciting_articles.erase(recv_article_id);
		}
		if (message->article_all().header().news_id() != message->from()) {
			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
				MFW::ArticleDataChangeFactory(
					public_identifier_,
					message->from(),
					recv_article_id,
					true
				))
			);
		}
	}
}

void Peer::handle_article_list_response(shared_ptr_message message) {
	auto list_size = message->article_list().response_size();
	if (list_size != 0) {
		pk_t list_news_id = message->article_list().response().begin()->news_id();

		for (auto it = message->article_list().response().begin(); it != message->article_list().response().end(); it++) {
			auto& news = get_news(list_news_id);

			auto add_to_news = [&](){
				auto a = Article(*it);
				a.add_reader(message->from());
				news.add_article(a.main_hash(), std::move(a));
			};

			try { //try finding received article from article list in article database
				auto& present_article = news.get_article(it->main_hash());
				if (present_article.modification_time() >= it->modification_time()) { //if received article is newer, replace it
					news.remove_article(it->main_hash());
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

		emit new_article_list(list_news_id);
	}
	else {
		std::cout << "Article List response for " << message->article_list().response().begin()->news_id() << "; empty list" << std::endl;
	}
	getting_article_list.erase(message->from());
	emit check_selected_item();
}

void Peer::handle_credentials_response(shared_ptr_message message) {
	if (message->credentials().req_ipv4()) {
		if (message->credentials().req_ipv6()) {
			networking_->ip_map_.update_ip((pk_t)message->from(), QHostAddress(QString(message->credentials().ipv4().c_str())), QHostAddress(QString(message->credentials().ipv6().c_str())));
		}
		else {
			networking_->ip_map_.update_ip((pk_t)message->from(), QHostAddress(QString(message->credentials().ipv4().c_str())));
		}
	}
	if (message->credentials().req_rsa_public_key()) {
		networking_->ip_map_.update_rsa_public((pk_t)message->from(), message->credentials().rsa_public_key());
		
	}
	if (message->credentials().req_eax_key()) {
		networking_->ip_map_.update_eax((pk_t)message->from(), message->credentials().eax_key());
		emit new_symmetric_key(message->from());
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

	CryptoPP::AutoSeededRandomPool rng;
	std::string key_str = msg->symmetric_key().key();
	std::string signature_str = msg->symmetric_key().signature();

	std::string key_decrypted_str, key_encrypted_decoded;
	StringSource ss0(
		key_str,
		true,
		new HexDecoder(
			new StringSink(
				key_encrypted_decoded
			)
		)
	);

	CryptoPP::SecByteBlock key_encrypted(reinterpret_cast<const CryptoPP::byte*>(&key_encrypted_decoded[0]), key_encrypted_decoded.size());
	CryptoPP::SecByteBlock signature(reinterpret_cast<const CryptoPP::byte*>(&signature_str[0]), signature_str.size());

	signer_verifier::Verifier verifier(networking_->ip_map_.get_rsa_public(msg->from())->value());
	rsa_encryptor_decryptor::Decryptor rsa_decryptor(networking_->ip_map_.private_rsa.value());

	CryptoPP::StringSource ss1(
		key_encrypted.data(),
		key_encrypted.size(),
		true,
		new CryptoPP::PK_DecryptorFilter(
			rng,
			rsa_decryptor,
			new CryptoPP::StringSink(key_decrypted_str)
		)
	);

	CryptoPP::SecByteBlock key_decrypted(reinterpret_cast<const CryptoPP::byte*>(&key_decrypted_str[0]), key_decrypted_str.size());

	bool verification_result = verifier.VerifyMessage(
		key_decrypted.data(),
		key_decrypted.SizeInBytes(),
		signature,
		signature.size()
	);

	std::cout << "Verification of received symmetric key";
	if (!verification_result) {
		std::cout << " FAILED" << std::endl;
	}
	else {
		std::cout << " SUCCEEDED" << std::endl;
		networking_->ip_map_.get_wrapper_for_pk(msg->from())->second.add_eax_key(std::move(key_decrypted));
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
	user_map.insert({(pk_t)msg->from(), PeerInfo((pk_t)msg->from())});
	
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextResponse(
			MFW::PublicKeyFactory(
				public_identifier_,
				msg->from(),
				networking_->ip_map_.my_ip.key_pair.first.value()
			)
		)
	);
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
				newspaper_id
			)
		)
	);
}

void Peer::handle_newspaper_entry_request(shared_ptr_message message) {
	std::uint64_t news_id = message->newspaper_entry().entry().news_id();

	try {
		auto& news = get_news(news_id);
		networking_->enroll_message_to_be_sent(
			MFW::RespNewspaperEntryFactory(
				MFW::NewspaperEntryFactory(
					public_identifier_,
					message->to(),
					news_id
				),
				news
			)
		);
	}
	catch (unknown_newspaper_error& e) {
		networking_->enroll_message_to_be_sent(
			MFW::SetMessageContextError(
				MFW::NewspaperEntryFactory(
					public_identifier_,
					message->from(),
					news_id
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
			get_news()
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
	NewspaperEntry news(news_id);
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
	news_.insert({ id, std::move(NewspaperEntry(id)) });
	emit got_newspaper_confirmation(id);
}

/**
 * For debugging purposes only.
*/
void Peer::print_contents() {
	std::cout << "public_identifier_ " << public_identifier_ << std::endl;
	std::cout << "name_ " << name_ << std::endl;
	std::cout << "newspaper_id_ " << newspaper_id_ << std::endl;
	std::cout << "newspaper_name_ " << newspaper_name_ << std::endl;
	std::cout << "news_ count: " << news_.size() << std::endl;
	std::cout << "Reader count: " << readers_.size() << std::endl;
	std::cout << "User count: " << user_map.size() << std::endl;
	for (auto&& n : news_) {
		std::cout << "news: " << (n.second.get_name().empty() ? "EMPTY" : n.second.get_name()) << "; " << n.second.get_id() << std::endl;
		for (auto&& f : n.second.get_friends()) {
			std::cout << "  friend: " << f << std::endl;
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
		if (!find_news(it->news_id())) {
			add_new_newspaper(it->news_id(), it->news_name(), message->from());
		}
	}
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
	networking_->ip_map_.my_ip.ipv4 = QHostAddress(ip);
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
	journalists_.insert(j);
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
	auto [ait, eit] = readers_.equal_range(article);
	for (; ait != eit; ait++) {
		if (ait->second->peer_key == reader) {
			readers_.erase(ait);
			break;
		}
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

bool Peer::add_friend(pk_t id, const std::string& ip) {
	return friends_.emplace(id).second;
	// Networking* networking = get_networking();
	// return true;
}
