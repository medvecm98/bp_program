#include "ArticleDatabase.h"

/** \brief Adds article to article database.
 *
 * Article will be added only if it wasn't already present.
 * @param hash Key of the article.
 * @param article Article to add.
 * @return If addition took place.
 */
bool ArticleDatabase::add_to_database(hash_t hash, Article article) {
	return _database.insert(article_database_container::value_type(hash, std::move(article))).second;
}

/** \brief Fills database from header files.
 */
void ArticleDatabase::load_from_headers() {
	my_string line;
	int index = 0;
	bool in_header = false, in_hash = false;
	std::pair<article_database_container::iterator, bool> new_article;
	for (auto&& path : _paths_to_headers) { //iterate through all header files
		std::ifstream file = std::ifstream(path);
		while (std::getline(file, line)) {
			if (StringHelpers::Trim(line).empty()) {
				continue;
			}

			if (!in_header && line.find(BEGIN_HEADER_) != my_string::npos) {
				in_header = true;
				new_article = _database.insert(article_database_container::value_type(counter++, Article()));
			}
			else if (in_header && line.find(END_HEADER_) != my_string::npos) {
				in_header = false;
			}
			else if (!in_hash && line.find(BEGIN_HASH_) != my_string::npos) {
				in_hash = true;
			}
			else if (in_hash && line.find(END_HASH_) != my_string::npos) {
				in_hash = false;
			}

			if (in_header) {
				if ((index = line.find(HEADER_CATEGORIES_)) != my_string::npos) {
					auto cats = StringHelpers::Split(line.substr(index + my_string(HEADER_CATEGORIES_).length()),
						  CATEGORY_DELIMITER_, 0, true);
					for (auto&& cat : cats) {
						new_article.first->second._categories.insert(StringHelpers::Trim(cat));
					}
				}
				else if ((index = line.find(HEADER_AUTHOR_NAME_)) != my_string::npos) {
					new_article.first->second._author_name = StringHelpers::Trim(line.substr(index + sizeof(HEADER_AUTHOR_NAME_)));
				}
				else if ((index = line.find(HEADER_AUTHOR_ID_)) != my_string::npos) {
					new_article.first->second._author_id = std::stoi(StringHelpers::Trim(line.substr(index + sizeof(HEADER_AUTHOR_NAME_))));
				}
				else if ((index = line.find(HEADER_MAIN_HASH_)) != my_string::npos) {
					new_article.first->second._main_hash = std::stol(StringHelpers::Trim(line.substr(index + sizeof(HEADER_MAIN_HASH_))));
				}
				else if ((index = line.find(HEADER_FILE_PATH_)) != my_string::npos) {
					new_article.first->second._path_to_article_file = StringHelpers::Trim(line.substr(index + sizeof(HEADER_FILE_PATH_)));
				}
			}
			else if (in_hash) {
				auto hash_line_split = StringHelpers::Split(line, '*', 0, true);

				std::int32_t which_position = std::stoi(hash_line_split[0]);
				level_t pl = std::stoi(hash_line_split[1]);
				hash_t h = std::stoi(hash_line_split[2]);

				new_article.first->second._hashes.insert(hashes_container::value_type(which_position, HashWrapper(h, pl)));
			}
		}
	}
	_headers_loaded = true;
}

const Article& ArticleDatabase::get_article(hash_t a) {
	return _database[a];
}

template<typename T>
void ArticleDatabase::remove_articles(const T& list) {
	for (auto&& item : list) {
		remove_article(item);
	}
}

void ArticleDatabase::remove_article(hash_t a) {
	_database.erase(a);
}

void ArticleDatabase::list_articles() {
	if (!_headers_loaded) {
		load_from_headers();
	}
	for (auto&& article : _database) {
		std::cout << article.second._path_to_article_file << std::endl;
	}
}

void ArticleDatabase::save_to_headers() {

}