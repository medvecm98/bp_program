#include "NewspaperEntry.h"

NewspaperEntry::NewspaperEntry(pk_t first_key, pk_t id, const my_string& name) {
	_authorities.insert(first_key);
	news_id_ = id;
	news_name_= name;
}

NewspaperEntry::NewspaperEntry(const np2ps::LocalSerializedNewspaperEntry& serialized_ne) : 
	news_id_(serialized_ne.entry().news_id()),
	news_name_(serialized_ne.entry().news_name()) 
{
	for(const np2ps::SerializedArticle& gpb_articles : serialized_ne.articles()) {
		add_article(gpb_articles.article().main_hash(), Article(gpb_articles));
	}
}

NewspaperEntry::NewspaperEntry(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne) :
	news_id_(serialized_ne.entry().news_id()),
	news_name_(serialized_ne.entry().news_name())
{
	for(const np2ps::Article& gpb_articles : serialized_ne.articles()) {
		_articles.emplace(gpb_articles.main_hash(), Article(gpb_articles));
	}
}

void NewspaperEntry::add_article(hash_t article_hash, Article&& article) {
	time_sorted_articles.emplace(article_hash, article.get_creation_time());
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

article_data_vec NewspaperEntry::get_articles_for_time_span(my_clock::time_point time_span_begin, my_clock::time_point time_span_end) {
	article_data_vec rv;
	auto ms_begin = std::chrono::duration_cast<std::chrono::milliseconds>(time_span_begin.time_since_epoch()).count();
	auto ms_end = std::chrono::duration_cast<std::chrono::milliseconds>(time_span_end.time_since_epoch()).count();
	for (auto article_it = get_iterator_database(); article_it != get_iterator_database_end(); article_it++) {
		auto& [article_hash, article_content] = *article_it;
		auto creation_time = article_content.get_creation_time();
		if (creation_time >= ms_begin && creation_time < ms_end) {
			rv.push_back(article_it);
		}
	}
	return rv;
}

timed_article_map_pair NewspaperEntry::get_newest_articles(std::size_t count) {
	timed_article_map_iter bit = time_sorted_articles.begin();
	timed_article_map_iter eit = time_sorted_articles.end();
	timed_article_map_iter it = bit;
	std::size_t i = 0;

	while (it != eit && i < count) {
		i++;
		it++;
	}

	return { bit, it };
}

/** Get `count` newest articles from start of given day.
*/
timed_article_map_pair NewspaperEntry::get_newest_articles(QDate date, std::size_t count) {
	QDateTime date_time = date.startOfDay(Qt::LocalTime);
	std::int64_t epoch = date_time.toMSecsSinceEpoch();

	timed_article_map_iter bit = time_sorted_articles.begin();
	timed_article_map_iter eit = time_sorted_articles.end();
	timed_article_map_iter it = bit;
	std::size_t i = 0;

	while (it != eit && i < count) {
		i++;
		it++;
	}

	return { bit, it };
}

timed_article_map_pair NewspaperEntry::get_newest_articles(QDate date, std::size_t count) {
	timed_article_map_iter bit = time_sorted_articles.begin();
	timed_article_map_iter eit = time_sorted_articles.end();
	timed_article_map_iter it = bit;
	std::size_t i = 0;

	while (it != eit && i < count) {
		i++;
		it++;
	}

	return { bit, it };
}