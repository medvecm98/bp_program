#include "Peer.h"

/**
 * Adds new newspaper to the collection.
 * @param newspaper_key Public key of newspaper.
 * @param newspaper_ip_domain IP, or domain name, of the newspaper.
 */
void Peer::add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, const std::string &newspaper_ip_domain) {
	news_.insert({newspaper_key, NewspaperEntry(newspaper_key, newspaper_key, newspaper_name)});
	networking_.ip_map_.add_to_map(newspaper_key, IpWrapper(newspaper_ip_domain));
}

void Peer::load_ip_authorities(pk_t newspaper_key) {
	networking_.enroll_message_to_be_sent(std::move(MFW::IpAddressFactory(np2ps::REQUEST, public_key_, newspaper_key)));
}

bool Peer::request_margin_add(hash_t article, const Margin& margin) {
	auto found_article = find_article_in_database(article);

	if (found_article.has_value()) {
		networking_.enroll_message_to_be_sent(MFW::MarginAddFactory(np2ps::REQUEST, public_key_,
																	found_article.value()->get_author(),
																	article, margin));
		return true;
	}
	return false;
}

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
size_t Peer::list_all_articles_from_news(std::vector<article_ptr> &articles, const std::set<category_t> &categories) {
	size_t article_counter = 0;
	for (auto&& cat : articles_categories_) {
		if (categories.empty() || (categories.find(cat.first) != categories.end())) {
			for (auto&& [a_hash, a_peer] : cat.second) {
				articles.push_back(std::make_shared<Article>(a_peer.article));
				article_counter++;
			}
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
size_t Peer::list_all_articles_from_news(std::vector<article_ptr> &articles) {
	size_t article_counter = 0;
	for (auto&& cat : articles_categories_) {
		for (auto&& [a_hash, a_peer] : cat.second) {
			articles.push_back(std::make_shared<Article>(a_peer.article));
			article_counter++;
		}
	}
	return article_counter;
}

/**
 * Functor, that covers one news company.
 */
struct TheSameNews {
	TheSameNews() = default;
	news_database::const_iterator entry;
	news_database::const_iterator end_iterator;
	bool used = false;
	news_database::const_iterator operator() () {
		if (!used) {
			used = true;
			return entry;
		}
		else {
			return end_iterator;
		}
	}
};

/**
 * Functor, that covers all the various news companies.
 */
struct AllTheNews {
	explicit AllTheNews(news_database::const_iterator nd) {
		data = nd;
	}
	news_database::const_iterator data;
	news_database::const_iterator operator() () {
		return data++;
	}
};

size_t Peer::list_all_articles_by_me(std::vector<article_ptr> &articles, const std::set<category_t> &categories, pk_t news_id) {
	size_t article_counter = 0;
	std::function<news_database::const_iterator()> news_functor;
	AllTheNews al(news_.cbegin());
	TheSameNews ts;

	news_database::const_iterator news;
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
		auto i_end = news_iterator->second.get_const_iterator_database_end();
		for (auto i = news_iterator->second.get_const_iterator_database(); i != i_end; i++) {
			for (auto &&category : categories) {
				if ((i->second.get_author() == public_key_) && (categories.empty() || i->second.is_in_category(category))) {
					articles.push_back(std::make_shared<Article>(i->second));
					article_counter++;
					break;
				}
			}
		}
	}

	return article_counter;
}

size_t Peer::list_all_articles_by_me(std::vector<article_ptr> &articles, pk_t news_id) {

	return list_all_articles_by_me(articles, std::set<category_t>(), news_id);
}

article_optional Peer::find_article(hash_t article_hash) {
	for (auto&& news : news_) {
		auto this_news_search = news.second.find_article_header(article_hash);
		if (this_news_search.has_value())
			return this_news_search;
	}

	return article_optional();
}

void Peer::download_article(pk_t article_author, hash_t article_hash) {
	//TODO: set level properly
	networking_.enroll_message_to_be_sent(MFW::ArticleDownloadFactory(np2ps::REQUEST, public_key_, article_author,
																	  article_hash, 255));
}

void Peer::handle_message() {
	auto message = networking_.pop_message();
	auto type = message->msg_type();

	switch (type)
	{
		case np2ps::ARTICLE_ALL:

			break;
		
		default:
			break;
	}
}