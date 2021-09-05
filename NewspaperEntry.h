#ifndef PROGRAM_NEWSPAPERENTRY_H
#define PROGRAM_NEWSPAPERENTRY_H

#include "GlobalUsing.h"
#include "Article.h"
#include <optional>

using database_iterator_t = article_database_container::const_iterator;

class NewspaperEntry {
public:
	NewspaperEntry() = default;
	NewspaperEntry(pk_t first_key, pk_t id, const my_string& name);
	void add_article(hash_t article_hash, Article&& article);
	void remove_article(hash_t article_hash);
	std::optional<article_ptr> find_article_header(hash_t article_hash);
	database_iterator_t get_const_iterator_database() const;
	database_iterator_t get_const_iterator_database_end() const;
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
private:
	pk_t news_id_;
	my_string news_name_;
	article_database_container _articles; //downloaded articles from these newspapers
	user_container _authorities; //authorities of these newspapers
	level_t level_; //level for given newspaper
};

using news_database = std::unordered_map<pk_t, NewspaperEntry>;

/**
 * Functor, that covers one news company.
 */
struct TheSameNews {
	TheSameNews() = default;
	TheSameNews(news_database::const_iterator the_one, news_database::const_iterator end_i) :
		entry(the_one), end_iterator(end_i) {}
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

#endif //PROGRAM_NEWSPAPERENTRY_H
