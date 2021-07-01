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

using news_database = std::unordered_map<pk_t, NewspaperEntry>;
using MFW = MessageFactoryWrapper;
using reader_database = std::unordered_multimap<hash_t, PeerInfo>;


class Peer {
public:
	Peer() {
		public_key_ = 0;
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
	void download_article(pk_t article_author, hash_t article_hash);
	void handle_message();
	void handle_requests(unique_ptr_message message);

	pk_t get_public_key() {
		return public_key_;
	}

	my_string get_name() {
		return name_;
	}


private:
	//reader part
	pk_t public_key_;
	my_string name_;
	Networking networking_;
	news_database news_; //list of all articles and their sources (their "News")

	//journalist part
	reader_database readers_; //list of article readers

	//authorities
	user_container basic_users;
	user_level_map user_map;

	//chief editor
	category_multimap_container articles_categories_;
	my_string newspaper_name_;
	pk_t newspaper_id_;
	user_container authorities_;
	user_container journalists_;

	//other
	std::unordered_multimap<hash_t, Margin> margins_added_; //multimap of Article -> Margins, that this peer added, or requested to add

	article_optional find_article_in_database(hash_t article_hash);
};

#endif //PROGRAM_PEER_H