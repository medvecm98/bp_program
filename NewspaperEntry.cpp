//
// Created by michal on 26. 3. 2021.
//

#include "NewspaperEntry.h"

NewspaperEntry::NewspaperEntry(pk_t first_key) {
	_authorities.insert(first_key);
}

void NewspaperEntry::add_article(hash_t article_hash, Article&& article) {
	_articles.insert({article_hash, article});
}

void NewspaperEntry::remove_article(hash_t article_hash) {
	_articles.erase(article_hash);
}

std::optional<article_ptr> NewspaperEntry::find_article_header(hash_t article_hash) {
	auto find_result = _articles.find(article_hash);
	if (find_result == _articles.end()) {
		return article_optional ();
	}
	return article_optional (std::make_shared<Article>(find_result->second));
}

database_iterator_t NewspaperEntry::get_const_iterator_database() const {
	return _articles.cbegin();
}

database_iterator_t NewspaperEntry::get_const_iterator_database_end() const {
	return _articles.cend();
}
