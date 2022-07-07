#include "Peer.h"

/**
 * @brief Adds new article to its corresponding newspaper.
 * 
 * @param a Article to add.
 */
void Peer::enroll_new_article(Article a) {
	news_[a.news_id()].add_article(a.main_hash(),std::move(a));
}

/**
 * Adds new newspaper to the collection.
 * @param newspaper_key Public key of newspaper.
 * @param newspaper_ip_domain IP, or domain name, of the newspaper.
 */
void Peer::add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, const std::string &newspaper_ip_domain) {
	std::cout << "Peer::add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, const std::string &newspaper_ip_domain)\n";
	networking_->ip_map_.add_to_map(newspaper_key, IpWrapper(newspaper_ip_domain));
	networking_->get_stun_client()->allocate_request(newspaper_key);

	newspapers_awaiting_confirmation.emplace(newspaper_key, NewspaperEntry(newspaper_key, newspaper_key, newspaper_name));
}

void Peer::newspaper_confirm(pk_t pid) {
	std::cout << "Peer::newspaper_confirm(pk_t pid)\n";
	auto ne = newspapers_awaiting_confirmation.find(pid);
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
			articles.insert(std::make_shared<Article>(cat.second->article));
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
	for (auto&& cat : articles_categories_) {
			articles.insert(std::make_shared<Article>(cat.second->article));
			article_counter++;
	}
	return article_counter;
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
size_t Peer::list_all_articles_by_me(article_container &articles, const std::set<category_t> &categories, pk_t news_id) {
	size_t article_counter = 0;
	std::function<news_database::const_iterator()> news_functor;
	AllTheNews al(news_.cbegin(), news_.cend());
	TheSameNews ts;

	news_database::const_iterator news;
	if (news_id == 0) {
		news_functor = al;
	}
	else {
		news = news_.find(news_id);
		if (news == news_.cend()) {
			return 0;
		}
		ts.entry = news;
		ts.end_iterator = news_.cend();
		news_functor = ts;
	}

	for (auto news_iterator = news_functor(); news_iterator != news_.cend(); news_iterator = news_functor()) {
		/* iterate through one or all news */

		auto i_end = news_iterator->second.get_const_iterator_database_end();
		for (auto i = news_iterator->second.get_const_iterator_database(); i != i_end; i++) {
			if (!categories.empty()) {
				for (auto &&category : categories) {
					if ((i->second.author_id() == public_identifier_) && (categories.empty() || i->second.is_in_category(category))) {
						articles.insert(std::make_shared<Article>(i->second));
						article_counter++;
						break;
					}
				}
			}
			else {
				if (i->second.author_id() == public_identifier_) {
					articles.insert(std::make_shared<Article>(i->second));
					article_counter++;
					std::cout << "added article" << std::endl;
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

/**
 * @brief Universal method to handle the message from the top of the message queue.
 * 
 * @param article_hash Shared pointer to message which needs to be handled.
 */
void Peer::handle_message(unique_ptr_message message) {
	std::cout << "message handling" << std::endl;
	if (message->msg_ctx() == np2ps::REQUEST) {
		std::cout << "message request" << std::endl;
		handle_requests( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::RESPONSE) {
		std::cout << "message response" << std::endl;
		handle_responses( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::ONE_WAY) {
		std::cout << "message one way" << std::endl;
		handle_one_way( std::move( message));
	}
	else if (message->msg_ctx() == np2ps::ERROR) {
		std::cout << "message error (context is error)" << std::endl;
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
void Peer::handle_requests(unique_ptr_message message) {
	auto type = message->msg_type();

	if (type == np2ps::ARTICLE_ALL) {
		//first we will determine what is the level of the peer that requested article
		//TODO: redo this without using .bucket()

		auto article_bucket = readers_.bucket(message->article_all().header().main_hash());
		auto article_readers_iterator = readers_.begin(article_bucket);

		bool user_not_found = true;
		level_t req_level = message->article_all().level();
		level_t found_level = 0;
		level_t final_level = 0;

		for (; user_not_found && (article_readers_iterator != readers_.end(article_bucket)); article_readers_iterator++) {
			//we search for given user, if it exists in our database and we will check for his level
			if (article_readers_iterator->second->peer_key == message->from()) {
				user_not_found = false;
				found_level = article_readers_iterator->second->peer_level;
			}
		}

		for (auto&& user : user_map) {
			if (user.first == message->from()) {
				user_not_found = false;
				found_level = user.second.peer_level;
			}
		}

		final_level = req_level < found_level ? req_level : found_level;

		if (user_not_found) {
			/* if no user was found in database, we need to check with authority, what his level is */

			auto user_check_msg = MFW::UpdateSeqNumber(
				MFW::ReqUserIsMemberFactory(
					MFW::UserIsMemberFactory(public_identifier_, 
						news_[ message->article_all().header().news_id()].get_id(), 
						message->from()), 
					message->article_all().level()),
				message->seq());

			networking_->store_to_map(std::move(message)); //stores current message, so it waits for answer
			networking_->enroll_message_to_be_sent(std::move(user_check_msg)); //sends request for user level in given
		}
		else {
			/* send user requested article */

			auto article = find_article(message->article_all().article_hash());
			if (article.has_value() && article.value()->article_present()) {
				std::string article_whole;
				//select only appropriate level
				article.value()->select_level(article_whole, final_level);

				unique_ptr_message article_msg = MFW::RespArticleDownloadFactory(
					MFW::ArticleDownloadFactory(
						public_identifier_, 
						message->from(), 
						article.value()->main_hash(), 
						final_level),
					article.value(), 
					std::move(article_whole)
				);

				//send message
				networking_->enroll_message_to_be_sent(article_msg);
			}
			else {
				//article not found in database

				std::vector<pk_t> article_peers;
				auto [readers_begin, readers_end] = readers_.equal_range(message->article_all().article_hash());
				if (readers_begin != readers_end) {
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
				else if (!journalists_.empty()) {
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
		}
	}
	else if (type == np2ps::ARTICLE_HEADER) {
		auto article_header = find_article(message->article_header().article_hash());
		if (article_header.has_value()) {
			networking_->enroll_message_to_be_sent(
				MFW::RespArticleHeaderFactory(
					MFW::ArticleHeaderFactory(
						public_identifier_,
						message->from(),
						message->article_header().article_hash()
					),
					article_header.value()
				)
			);
		}
		else {
			//TODO: send error
		}
	}
	else if (type == np2ps::ARTICLE_LIST) {
		article_container articles;

		pk_t news_id = 0;

		if (message->article_list().all_articles()) {
			list_all_articles_by_me(articles, news_id);
		}
		else {
			category_container categories;
			for (int i = 0; i < message->article_list().categories_size(); i++) {
				categories.insert(message->article_list().categories(i));
			}
			list_all_articles_by_me(articles, categories, news_id);
		}

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
	else if (type == np2ps::USER_IS_MEMBER) {
		//TODO: error, if user is not authority
		bool is_member = false;
		auto find_result = user_map.find(message->user_is_member().user_pk());
		if ((find_result != user_map.end()) && (find_result->second.peer_level >= message->user_is_member().level())) {
			is_member = true;
		}
		networking_->enroll_message_to_be_sent(
			MFW::RespUserIsMemberFactory(
				MFW::UserIsMemberFactory(
					public_identifier_,
					message->from(),
					message->user_is_member().user_pk()
				),
				is_member,
				message->user_is_member().level()
			)
		);
	}
	else if (type == np2ps::ARTICLE_DATA_UPDATE) {
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
				readers_.erase(message->from());
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
	else if (type == np2ps::UPDATE_MARGIN) {
		auto article = find_article(message->update_margin().article_pk());
		if (article.has_value() && (article.value()->author_id() == public_identifier_)) {
			if (message->update_margin().m_action() == np2ps::ADD) {
				for (int i = 0; i < message->update_margin().margin().margins_size(); i++) {
					article.value()->add_margin(message->from(), Margin(
						message->update_margin().margin().margins(i).type(),
						message->update_margin().margin().margins(i).content(),
						message->update_margin().margin().margins(i).id()
					));
				}
			}
			else if (message->update_margin().m_action() == np2ps::REMOVE) {
				auto [bi, bie] = article.value()->get_range_iterators(message->from());
				std::vector<decltype(bi)> margins_to_remove;
				for (int j = 0; j < message->update_margin().margin().margins_size(); j++) {
					auto proto_margin = message->update_margin().margin().margins(j);
					for (auto i = bi; i != bie; i++) {
						if (bi->second.id == proto_margin.id()) {
							margins_to_remove.push_back(i);
						}
					}
				}
				for (auto&& mtr : margins_to_remove) {
					article.value()->remove_margin(mtr);
				}
			}
			else if (message->update_margin().m_action() == np2ps::UPDATE) {
				auto [bi, bie] = article.value()->get_range_iterators(message->from());
				for (int j = 0; j < message->update_margin().margin().margins_size(); j++) {
					auto proto_margin = message->update_margin().margin().margins(j);
					for (auto i = bi; i != bie; i++) {
						if (bi->second.id == proto_margin.id()) {
							bi->second.type += ' ' + proto_margin.type();
							bi->second.content += ' ' + proto_margin.content();
						}
					}
				}
			}
		}
	}
	else if (type == np2ps::UPDATE_ARTICLE) {
		//TODO: send unsupported error
	}
	else if (type == np2ps::CREDENTIALS) {
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
		if (!req_my_credentials && message->credentials().req_eax_key()) {
			if (networking_->ip_map_.have_eax(message->from()))
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
}

/**
 * @brief Handles messages of context "response".
 * 
 * Further flow is determined by the type of the message. Usually, no message is
 * sent via network.
 * 
 * @param message Shared pointer to message (request) which needs to be handled. 
 */
void Peer::handle_responses(unique_ptr_message message) {
	auto type = message->msg_type();

	if (type == np2ps::ARTICLE_ALL) {
		if (message->has_article_all() && message->article_all().has_header() && message->article_all().has_article_actual()) {
			hash_t recv_article_hash = message->article_all().article_hash();
			Article recv_article(message->article_all().header(), message->article_all().article_actual());
			if (news_.find(recv_article.news_id()) == news_.end()) {
				//TODO: log error
			}
			auto& news_entry = news_[recv_article.news_id()];
			news_entry.add_article(recv_article_hash, std::move(recv_article));
		}
		else {
			//TODO: log error
		}
	}
	else if (type == np2ps::ARTICLE_HEADER) {
		if (message->has_article_header()) {
			article_headers_only.insert_or_assign(message->article_header().article_hash(), Article(message->article_header().article()));
		}
	}
	else if (type == np2ps::ARTICLE_LIST) {
		std::cout << message->article_list().response_size() << '\n';
		pk_t list_news_id = message->article_list().response().begin()->news_id();
		auto& article_list = news_[list_news_id].get_list_of_articles();
		
		for (auto it = message->article_list().response().begin(); it != message->article_list().response().end(); it++) {
			auto a = Article(*it);
			auto [cit, cite] = a.categories();
			for (; cit != cite; cit++) {
				if (article_list.categories.find(*cit) == article_list.categories.end()) {
					article_list.categories.insert(*cit);
				}
			}
			article_list.article_headers.insert({a.main_hash(), std::move(a)});
		}

		emit new_article_list(list_news_id);
	}
	else if (type == np2ps::USER_IS_MEMBER) {
		if (message->user_is_member().is_member() && (message->user_is_member().level() > 127)) {
			user_map.insert({message->user_is_member().user_pk(), PeerInfo(message->user_is_member().user_pk(), message->user_is_member().level())});
		}
		if (networking_->check_if_in_map(message->seq())) {
			if (message->user_is_member().is_member()) {
				//user was member of given level, message can be sent

				emit user_is_member_verification(message->seq(), true);
			}
			else {
				//message can't be sent, user wasn't member of given level

				emit user_is_member_verification(message->seq(), false);
				//TODO: log error
			}
		}
	}
	else if (type == np2ps::CREDENTIALS) {
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
	else if (type == np2ps::PUBLIC_KEY) {
		std::cout << "public key update" << std::endl;
		networking_->ip_map_.update_rsa_public((pk_t)message->from(), message->public_key().key());
		if (newspapers_awaiting_confirmation.find((pk_t)message->from()) != newspapers_awaiting_confirmation.end()) {
			news_.insert({message->from(), newspapers_awaiting_confirmation[(pk_t)message->from()]});
			newspapers_awaiting_confirmation.erase(message->from());
			emit got_newspaper_confirmation(message->from());
		}
	}
	else if (type == np2ps::SYMMETRIC_KEY) {
		emit symmetric_key_exchanged(message->from());
	}
}

void Peer::allocate_on_stun_server(pk_t target) {
	networking_->get_stun_client()->allocate_request(target);
}

/**
 * @brief Article all message generator.
 * 
 * Generator for `np2ps::ARTICLE_ALL`. Will send the message via network.
 * 
 * @param destination Who is the receiver of the message.
 * @param article_hash Article hash of desired article.
 */
void Peer::generate_article_all_message(pk_t destination, hash_t article_hash) {
	networking_->enroll_message_to_be_sent(
		MFW::SetMessageContextRequest(
			MFW::ArticleDownloadFactory(
				public_identifier_,
				destination,
				article_hash,
				(article_headers_only.find(article_hash) != article_headers_only.end()) 
					&& (news_.find(article_headers_only[article_hash].news_id()) != news_.end()) 
					? news_[article_headers_only[article_hash].news_id()].level() 
					: 127
			)
		)
	);
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
 * @brief Handler for messages of context "one way".
 * 
 * Even though response to such message makes no sense, responses are made,
 * because it requires different handling that `requests`.
 * 
 * @param msg Shared pointer to `one way` message.
 */
void Peer::handle_one_way(unique_ptr_message msg) {
	auto type = msg->msg_type();
	if (type == np2ps::ARTICLE_SOLICITATION) {
		auto check_for_existence = networking_->soliciting_articles.find(msg->article_sol().article_hash());
		if (check_for_existence == networking_->soliciting_articles.end()) {
			std::vector<pk_t> potential_owners;
			for (auto i = msg->article_sol().possible_owners().begin(); i != msg->article_sol().possible_owners().end(); i++) {
				potential_owners.push_back(*i);
			}
			networking_->soliciting_articles.insert({msg->article_sol().article_hash(), std::move(potential_owners)});
		}
		auto destination = networking_->soliciting_articles[msg->article_sol().article_hash()].back();
		networking_->soliciting_articles[msg->article_sol().article_hash()].pop_back();
		generate_article_all_message(destination, msg->article_sol().article_hash());
	}
	else if (type == np2ps::SYMMETRIC_KEY) {
		std::cout << "handling and saving a symmetric key" << std::endl;
		CryptoPP::AutoSeededRandomPool rng;
		std::string key_str = msg->symmetric_key().key();
		std::string signature_str = msg->symmetric_key().signature();

		CryptoPP::SecByteBlock key_encrypted(reinterpret_cast<const CryptoPP::byte*>(&key_str[0]), key_str.size());
		CryptoPP::SecByteBlock signature(reinterpret_cast<const CryptoPP::byte*>(&signature_str[0]), signature_str.size());

		signer_verifier::Verifier verifier(networking_->ip_map_.get_rsa_public(msg->from())->value());
		rsa_encryptor_decryptor::Decryptor rsa_decryptor(networking_->ip_map_.private_rsa.value());

		std::string key_decrypted_str;

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

		if (!verification_result) {
			std::cout << "Verification FAILED" << std::endl;
			//TODO: throw error
		}
		else {
			std::cout << "Verification SUCCEEDED" << std::endl;
			networking_->ip_map_.get_wrapper_for_pk(msg->from())->second.add_eax_key(std::move(key_decrypted));
		}

		unique_ptr_message _msg = std::make_shared<proto_message>();
		_msg->set_from(public_identifier_);
		_msg->set_to(msg->from());
		_msg->set_msg_ctx(np2ps::RESPONSE);
		_msg->set_msg_type(np2ps::SYMMETRIC_KEY);
		networking_->enroll_message_to_be_sent(
			std::move(_msg)
		);
	}
	else if (type == np2ps::PUBLIC_KEY) {
		std::cout << "got one-way public key" << std::endl;
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
}

/**
 * @brief Handler for messages of context "error".
 * 
 * @param msg Shared pointer to the message.
 */
void Peer::handle_error(unique_ptr_message msg) {
	auto type = msg->msg_type();
	if (type == np2ps::ARTICLE_SOLICITATION) {
		auto check_for_existence = networking_->soliciting_articles.find(msg->article_sol().article_hash());
		if (check_for_existence != networking_->soliciting_articles.end()) {
			auto destination = networking_->soliciting_articles[msg->article_sol().article_hash()].back();
			networking_->soliciting_articles[msg->article_sol().article_hash()].pop_back();
			generate_article_all_message(destination, msg->article_sol().article_hash());
		}
	}
}

void Peer::send_stun_binding_request() {
	networking_->get_stun_client()->binding_request();
}