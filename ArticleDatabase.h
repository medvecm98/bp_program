#include "GlobalUsing.h"
#include "Article.h"

#ifndef PROGRAM_ARTICLEDATABASE_H
#define PROGRAM_ARTICLEDATABASE_H

using database_container = std::map<hash_t, Article>;
using paths_container = std::set<my_string>;

class ArticleDatabase {
public:
	bool add_to_database(hash_t hash, Article article);
	bool headers_loaded() const { return _headers_loaded; }
	void load_from_headers();
private:
	bool _headers_loaded;
	database_container _database;
	paths_container _paths_to_headers;
	int counter = 0;
};

#endif //PROGRAM_ARTICLEDATABASE_H
