

#ifndef PROGRAM_ARTICLE_H
#define PROGRAM_ARTICLE_H

#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <map>
#include <regex>
#include "StringSplitter.h"
#include "GlobalUsing.h"
#include "Margins.h"

#define COMMENT_ "[NScomment]:"
#define BEGIN_HEADER_ "**BEGIN_HEADER**"
#define END_HEADER_ "**END_HEADER**"
#define BEGIN_HASH_ "**BEGIN_HASH**"
#define END_HASH_ "**END_HASH**"
#define HEADER_MAIN_HASH_ "*MAIN_HASH*"
#define HEADER_FILE_PATH_ "*FILE_PATH*"
#define HEADER_CATEGORIES_ "*CATEGORY*"
#define HEADER_AUTHOR_NAME_ "*AUTHOR_NAME*"
#define HEADER_AUTHOR_ID_ "*AUTHOR_ID*"
#define COMMENT_DELIM_ '#'
#define HASH_ "[NShash]:"
#define CATEGORY_DELIMITER_ ';'

enum article_format {
	NoFormat = -1,
	PlainText = 0,
	Markdown = 1,
	OrgMode = 2,
	Tex = 3,
	Html = 4
};

struct HashWrapper {
	HashWrapper(hash_t h, level_t l) {
		hash = h;
		paragraph_level = l;
	}
	hash_t hash;
	level_t paragraph_level;
};

using hashes_container = std::map<std::int32_t, HashWrapper>;
using hashes_vt = hashes_container::value_type;
using string_hash = std::hash<std::string>;



class Article {
public:
	explicit Article() {};
	explicit Article(const my_string&);
	void open_fstream();
	void load_information();
	std::uint64_t get_length();
	article_format get_format();
	std::set<my_string> get_categories();
	my_string get_path_to_file();
	std::ifstream& get_ifstream_file();
	std::ofstream& get_ofstream_file();
	hash_t get_main_hash() const {
		return _main_hash;
	}
	void calculate_hashes(hashes_container&);
	template<typename T>
	void initialize_article(const my_string&, const my_string&, std::size_t, const T&);

	friend class ArticleDatabase;

private:
	my_string _author_name;
	pk_t _author_id;
	my_string _news_name;
	pk_t _news_id;
	hash_t _main_hash;
	my_string _heading;
	hashes_container _hashes;
	std::uint64_t _length;
	article_format _format;
	std::set<my_string> _categories;
	my_string _path_to_article_file;
	std::map<pk_t, Margin> _margins;
	my_string _notes;
};

using article_database_container = std::map<hash_t, Article>;

#endif //PROGRAM_ARTICLE_H
