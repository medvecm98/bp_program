#ifndef PROGRAM_PEER_H
#define PROGRAM_PEER_H

#include "GlobalUsing.h"
#include "Networking.h"
#include "NewspaperEntry.h"
#include <unordered_map>
#include "Message.h"
#include <functional>

using news_database = std::unordered_map<pk_t, NewspaperEntry>;
using MFW = MessageFactoryWrapper;
using reader_database = std::unordered_multimap<hash_t, pk_t>;

class Peer {
public:
	Peer() {
		public_key_ = 0;
		newspaper_id_ = 0;
	}
	void load_ip_authorities(pk_t newspaper_key); //to load the IPs of authorities
	void enroll_new_article(Article article); //add new article to list of category -> article
	bool request_margin_add(hash_t article, const Margin& margin); //request margin addition
	void add_new_newspaper(pk_t newspaper_key, const std::string& newspaper_ip);
	size_t list_all_articles_from_news(std::vector<article_ptr>& articles, const std::set<category_t>& categories);
	size_t list_all_articles_from_news(std::vector<article_ptr>& articles);
	size_t list_all_articles_by_me(std::vector<article_ptr>& articles, const std::set<category_t>& categories, pk_t news_id = 0);
	size_t list_all_articles_by_me(std::vector<article_ptr>& articles, pk_t news_id = 0);
	article_optional find_article(hash_t article_hash);
	void download_article(pk_t article_author, hash_t article_hash);

private:
	//reader part
	pk_t public_key_;
	my_string name_;
	Networking networking_;
	news_database news_; //list of all articles and their sources (their "News")

	//journalist part
	reader_database readers_; //list of article readers

	//chief editor
	category_multimap_container articles_categories_;
	my_string newspaper_name_;
	pk_t newspaper_id_;
	user_container authorities_;
	user_container journalists_;

	//other
	std::map<hash_t, Margin> margins_added_; //map of Article <-> Margin, that this peer added, or requested to add

	article_optional find_article_in_database(hash_t article_hash);
};

#endif //PROGRAM_PEER_H