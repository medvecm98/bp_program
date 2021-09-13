#include "GlobalUsing.h"
#include "Article.h"

#ifndef PROGRAM_ARTICLEDATABASE_H
#define PROGRAM_ARTICLEDATABASE_H


using paths_container = std::set<my_string>;

class ArticleDatabase {
public:
	explicit ArticleDatabase(const my_string& path) {
		_paths_to_headers.insert(path);
		_headers_loaded = false;
	}
	bool add_to_database(hash_t hash, Article article);
	bool headers_loaded() const { return _headers_loaded; }
	const Article& get_article(hash_t);
	void list_articles();
	void remove_article(hash_t);

	template<typename T>
	void remove_articles(const T& list);

	void load_from_headers();
	void save_to_headers();

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & _headers_loaded;
		ar & _database;
		ar & _paths_to_headers;
		ar & counter;
	}
private:
	bool _headers_loaded;
	article_database_container _database;
	paths_container _paths_to_headers;
	int counter = 0;
};

#endif //PROGRAM_ARTICLEDATABASE_H
