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

	pk_t get_id() {
		return news_id_;
	}

	const my_string& get_name() {
		return news_name_;
	}
private:
	pk_t news_id_;
	my_string news_name_;
	article_database_container _articles; //downloaded articles from these newspaper
	user_container _authorities; //authorities of these newspaper
};

#endif //PROGRAM_NEWSPAPERENTRY_H
