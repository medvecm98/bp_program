#ifndef PROGRAM_NEWSPAPERENTRY_H
#define PROGRAM_NEWSPAPERENTRY_H

#include "GlobalUsing.h"
#include "Article.h"
#include <optional>

using database_iterator_t = article_database_container::iterator;
using article_data_vec = std::vector<database_iterator_t>;

struct ArticleListWrapper {
	std::set<my_string> categories;
	std::unordered_map<hash_t, Article> article_headers; 
};

class NewspaperEntry {
public:
	NewspaperEntry() = default;
	NewspaperEntry(pk_t first_key, pk_t id, const my_string& name);
	explicit NewspaperEntry(const np2ps::LocalSerializedNewspaperEntry& serialized_ne);
	explicit NewspaperEntry(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne);
	explicit NewspaperEntry(const std::string& path);
	void add_article(hash_t article_hash, Article&& article);
	timed_article_map_pair get_newest_articles(int count);
	timed_article_map_pair get_newest_articles(QDate date, int count);
	bool remove_article(hash_t article_hash);
	std::optional<article_ptr> find_article_header(hash_t article_hash);
	database_iterator_t get_iterator_database();
	database_iterator_t get_iterator_database_end();
	article_data_vec get_articles_for_time_span(my_clock::time_point time_span_begin, my_clock::time_point time_span_end);
	void add_friend(pk_t id);
	const user_container& get_friends();
	std::size_t friend_count() const;
	void remove_friend(pk_t id);
	Article& get_article(hash_t id);
	void deserialize(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne);

	user_container_citer get_first_authority() const {
		return _authorities.cbegin();
	}
	std::size_t get_authority_count() const {
		return _authorities.size();
	}

	pk_t get_id() {
		return news_id_;
	}

	const my_string& get_name() {
		return news_name_;
	}

	level_t level() {
		return level_;
	}

	ArticleListWrapper& get_list_of_articles() {
		return article_list_wrapper_;
	};

	void serialize_entry(np2ps::NewspaperEntry* entry) {
		std::cout << "serializing newspaper\n";
		entry->set_news_name(news_name_);
		entry->set_news_id(news_id_);
	}

	void network_serialize_entry(np2ps::NetworkSerializedNewspaperEntry* nserialized_ne) {
		serialize_entry(nserialized_ne->mutable_entry());
		// for (auto& [hash, art] : _articles) {
		// 	np2ps::Article* pa = nserialized_ne->add_articles();
		// 	art.network_serialize_article(pa);
		// }
	}

	void local_serialize_entry(np2ps::LocalSerializedNewspaperEntry* lserialized_ne) {
		serialize_entry(lserialized_ne->mutable_entry());
		for (auto& [hash, art] : _articles) {
			np2ps::SerializedArticle* pa = lserialized_ne->add_articles();
			art.local_serialize_article(pa);
		}
		for (auto&& f : friends_) {
			lserialized_ne->add_friends(f);
		}
	}

	void fill_time_sorted_articles() {
		if (time_sorted_articles.empty()) {
			for (auto&& article : _articles) {
				time_sorted_articles.emplace(article.first, article.second.creation_time());
			}
		}
	}

private:
	pk_t news_id_;
	my_string news_name_;
	article_database_container _articles; //(downloaded) articles from these newspapers
	user_container _authorities; //authorities of these newspapers
	level_t level_ = 127; //level for given newspaper
	ArticleListWrapper article_list_wrapper_; //saving requested article list
	timed_article_map time_sorted_articles;
	user_container friends_; //friends, that can share the articles with you
};

using news_database = std::unordered_map<pk_t, NewspaperEntry>;

/**
 * Functor, that covers one news company.
 */
struct TheSameNews {
	TheSameNews() = default;
	TheSameNews(news_database::iterator the_one, news_database::iterator end_i) :
		entry(the_one), end_iterator(end_i) {}
	news_database::iterator entry;
	news_database::iterator end_iterator;
	bool used = false;
	news_database::iterator operator() () {
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
	explicit AllTheNews(news_database::iterator nd, news_database::iterator e) {
		data = nd;
		enddata = e;
	}
	news_database::iterator data;
	news_database::iterator enddata;
	news_database::iterator operator() () {
		std::cout << "AllTheNews operator()" << std::endl;
		if (data == enddata) {
			return enddata;
		}
		else {
			return data++;
		}
	}
};

#endif //PROGRAM_NEWSPAPERENTRY_H
