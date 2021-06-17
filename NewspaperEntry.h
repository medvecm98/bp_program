#ifndef PROGRAM_NEWSPAPERENTRY_H
#define PROGRAM_NEWSPAPERENTRY_H

#include "GlobalUsing.h"
#include "Article.h"
#include <optional>

using database_iterator_t = article_database_container::const_iterator;


/**
 * IDs saved as keys.
 */
class NewspaperEntry {
public:
	NewspaperEntry() = default;
	explicit NewspaperEntry(pk_t first_key);
	void add_article(hash_t article_hash, Article&& article);
	void remove_article(hash_t article_hash);
	std::optional<article_ptr> find_article_header(hash_t article_hash);
	database_iterator_t get_const_iterator_database() const;
	database_iterator_t get_const_iterator_database_end() const;
private:
	article_database_container _articles; //downloaded articles from these newspaper
	user_container _authorities; //authorities of these newspaper
};

#endif //PROGRAM_NEWSPAPERENTRY_H
