#include "NewspaperEntry.h"

NewspaperEntry::NewspaperEntry(pk_t first_key, pk_t id, const my_string& name) {
	_authorities.insert(first_key);
	news_id_ = id;
	news_name_= name;
}

void NewspaperEntry::add_article(hash_t article_hash, Article&& article) {
	_articles.insert_or_assign(article_hash, article);
}

bool NewspaperEntry::remove_article(hash_t article_hash) {
	return _articles.erase(article_hash) > 0;
}

std::optional<article_ptr> NewspaperEntry::find_article_header(hash_t article_hash) {
	auto find_result = _articles.find(article_hash);
	if (find_result == _articles.end()) {
		return article_optional ();
	}
	return article_optional (&find_result->second);
}

database_iterator_t NewspaperEntry::get_iterator_database() {
	return _articles.begin();
}

database_iterator_t NewspaperEntry::get_iterator_database_end() {
	return _articles.end();
}

