#ifndef PROGRAM_PEER_H
#define PROGRAM_PEER_H

#include "GlobalUsing.h"
#include "Networking.h"
#include "NewspaperEntry.h"
#include <unordered_map>
#include "Message.h"
#include <functional>

struct PeerInfo {
	PeerInfo() = default;
	explicit PeerInfo(pk_t pk) {
		peer_key = pk;
		peer_level = 0;
	}
	PeerInfo(pk_t pk, level_t level) {
		peer_key = pk;
		peer_level = level;
	}
	pk_t peer_key = 0;
	level_t peer_level = 0;
	/* timestamp of info */
};


using MFW = MessageFactoryWrapper;
using reader_database = std::unordered_multimap<hash_t, PeerInfo>;

struct ArticleListWrapper {
	std::set<my_string> categories;
	std::vector<Article> article_headers; 
};

class Peer {
public:
	Peer() {
		CryptoPP::AutoSeededRandomPool prng;
		std::random_device rd("/dev/urandom");
		public_key_ = rd();
		pub_pri_pair_.second.GenerateRandomWithKeySize(prng, 2048);
		pub_pri_pair_.first = std::move(CryptoPP::RSA::PublicKey(pub_pri_pair_.second));
		newspaper_id_ = 0;
	}

	void load_ip_authorities(pk_t newspaper_key); //to load the IPs of authorities
	void enroll_new_article(Article article); //add new article to list of category -> article
	bool request_margin_add(hash_t article, margin_vector& margin); //request margin addition
	void add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, const std::string& newspaper_ip);
	size_t list_all_articles_from_news(article_container& articles, const std::set<category_t>& categories);
	size_t list_all_articles_from_news(article_container& articles);
	size_t list_all_articles_by_me(article_container& articles, const std::set<category_t>& categories, pk_t news_id = 0);
	size_t list_all_articles_by_me(article_container& articles, pk_t news_id = 0);
	article_optional find_article(hash_t article_hash);
	optional_author_peers find_article_in_article_categories_db(hash_t article_hash);
	optional_author_peers find_article_in_article_categories_db(hash_t article_hash, category_container categories);
	article_optional find_article_news(hash_t article_hash, category_container categories);
	void handle_message();
	void handle_requests(unique_ptr_message message);
	void handle_responses(unique_ptr_message message);
	void handle_one_way(unique_ptr_message message);
	void handle_error(unique_ptr_message message);
	void generate_article_all_message(pk_t destination, hash_t article_hash);
	void generate_article_header_message(pk_t destination, hash_t article_hash);

	void generate_article_list(pk_t destination) {
		networking_.enroll_message_to_be_sent(
			MFW::ReqArticleListFactory(
				MFW::ArticleListFactory(
					public_key_,
					destination
				),
				std::vector<my_string>()
			)
		);
	}
	
	template<typename Container>
	void generate_article_list(pk_t destination, const Container& categories) {
		networking_.enroll_message_to_be_sent(
			MFW::ReqArticleListFactory<Container>(
				MFW::ArticleListFactory(
					public_key_,
					destination
				),
				categories
			)
		);
	}

	void add_margin(hash_t article_hash, my_string type, my_string content) {
		auto article = find_article(article_hash);
		if (article.has_value()) {
			auto author = article.value()->author_id();
			std::vector<Margin> vm = { Margin(type, content) };
			margins_added_.insert({article_hash, vm.back()});
			networking_.enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
					MFW::UpdateMarginAddFactory(
						public_key_,
						author,
						article_hash,
						vm
					)
				)
			);
		}
		else {
			//TODO: log error
		}
	}

	void add_margin(hash_t article_hash, margin_vector& vm) {
		auto article = find_article(article_hash);
		if (article.has_value()) {
			auto author = article.value()->author_id();
			networking_.enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
					MFW::UpdateMarginAddFactory(
						public_key_,
						author,
						article_hash,
						vm
					)
				)
			);
		}
		else {
			//TODO: log error
		}
	}

	void update_margin(hash_t article_hash, unsigned int id, my_string type, my_string content) {
		auto article = find_article(article_hash);
		if (article.has_value()) {
			auto author = article.value()->author_id();
			std::vector<Margin> vm = { Margin(type, content, id) };
			
			auto [marginb, margine] = margins_added_.equal_range(article_hash);

			for (; marginb != margine; marginb++) {
				if (marginb->second.id == id) {
					marginb->second.type += ' ' + type;
					marginb->second.content += ' ' + content;
					break;
				}
			}

			networking_.enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
					MFW::UpdateMarginUpdateFactory(
						public_key_,
						author,
						article_hash,
						vm
					)
				)
			);
		}
		else {
			//TODO: log error
		}
	}

	void remove_margin(hash_t article_hash, unsigned int id) {
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

			networking_.enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
					MFW::UpdateMarginRemoveFactory(
						public_key_,
						author,
						article_hash,
						vm
					)
				)
			);
		}
		else {
			//TODO: log error
		}
	}

	pk_t get_public_key() {
		return public_key_;
	}

	my_string get_name() {
		return name_;
	}


private:
	//reader part
	pk_t public_key_;
	public_private_key_pair pub_pri_pair_;
	my_string name_;
	Networking networking_;
	news_database news_; //list of all downloaded articles, mapped by their Newspapers
	std::unordered_multimap<hash_t, Margin> margins_added_; //multimap of Article -> Margins, that this peer added, or requested to add
	std::unordered_map<pk_t, Article> article_headers_only; //only for article headers, so it won't interfere with regular ones
	ArticleListWrapper article_list_wrapper_; //saving requested article list

	//journalist part
	reader_database readers_; //list of article readers

	//authorities
	user_container basic_users;
	user_level_map user_map;

	//chief editor
	user_multimap_container newspaper_all_readers; //list of all readers of all articles
	category_multimap_container articles_categories_; //articles mapped by categories, maps to `newspaper_all_readers`
	my_string newspaper_name_;
	pk_t newspaper_id_;
	user_container authorities_;
	user_container journalists_;

	article_optional find_article_in_database(hash_t article_hash);
};

#endif //PROGRAM_PEER_H