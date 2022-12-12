#include "NewspaperEntry.h"

NewspaperEntry::NewspaperEntry(pk_t id) : news_id_(id) {}

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
	for (auto f : serialized_ne.friends()) {
		add_friend(f);
	}
}

NewspaperEntry::NewspaperEntry(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne) :
	news_id_(serialized_ne.entry().news_id()),
	news_name_(serialized_ne.entry().news_name())
{
	// for(const np2ps::Article& gpb_articles : serialized_ne.articles()) {
	// 	add_article(gpb_articles.main_hash(), Article(gpb_articles));
	// }
}

NewspaperEntry::NewspaperEntry(const std::string& path) {
	std::ifstream file(path);
	std::string line;
	if (std::getline(file, line)) {
		news_id_ = std::stoll(line);
	}
	else {
		throw other_error("Empty newspaper file.");
	}
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string ip, port, pk;
		if (!std::getline(ss, ip, ':') ||
		    !std::getline(ss, port, ':') ||
		    !std::getline(ss, pk)) {
				throw other_error("Invalid newspaper file format.");
			}
	}
}

void NewspaperEntry::deserialize(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne) {
	if (news_name_.empty()) {
		news_name_ = serialized_ne.entry().news_name();
	}
}

void NewspaperEntry::add_article(hash_t article_hash, Article&& article) {
	time_sorted_articles.emplace(article_hash, article.creation_time());
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
		auto creation_time = article_content.creation_time();
		if (creation_time >= ms_begin && creation_time < ms_end) {
			rv.push_back(article_it);
		}
	}
	return rv;
}

timed_article_map_pair NewspaperEntry::get_newest_articles(int count) {
	timed_article_map_iter bit = time_sorted_articles.begin();
	timed_article_map_iter eit = time_sorted_articles.end();

	if (count <= 0) {
		return { bit, eit };
	}

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
timed_article_map_pair NewspaperEntry::get_newest_articles(QDate date, int count) {
	QDateTime date_time = date.startOfDay(Qt::LocalTime);
	std::int64_t epoch = date_time.toMSecsSinceEpoch();

	timed_article_map_iter bit = time_sorted_articles.begin();
	timed_article_map_iter eit = time_sorted_articles.end();
	timed_article_map_iter it = bit;

	if (count <= 0) {
		while (it != eit) {
			if (it->first <= epoch) {
				return { it, eit };
			}
		}
	}

	std::size_t i = 0;

	while (it != eit && i < count) {
		if (it->first <= epoch) {
			if (i == 0) {
				bit = it;
				if (count <= 0) {
					return { bit, eit };
				}
			}
			i++;
		}
		it++;
	}

	return { bit, it };
}

const user_container& NewspaperEntry::get_friends() {
	return friends_;
}

void NewspaperEntry::remove_friend(pk_t id) {
	auto it = friends_.find(id);
	if (it != friends_.end()) {
		friends_.erase(it);
	}
}

void NewspaperEntry::add_friend(pk_t id) {
	friends_.emplace(id);
}

Article& NewspaperEntry::get_article(hash_t id) {
	auto it = _articles.find(id);
	if (it != _articles.end()) {
		return _articles[id];
	}
	throw article_not_found_database("Article not found in user database.");
}

std::size_t NewspaperEntry::friend_count() const {
	return friends_.size();
}