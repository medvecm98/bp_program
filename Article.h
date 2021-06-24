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
#include <optional>

#include "StringSplitter.h"
#include "GlobalUsing.h"
#include "Margins.h"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/set.hpp>

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
using category_container = std::set<my_string>;
using category_container_const_ref = const category_container&;
using category_container_const_iter = category_container::const_iterator;

class ParagraphIterator {
public:
	ParagraphIterator(std::string source_file_path, level_t l, hashes_container::iterator h) :
		source_file(source_file_path), level(l), hash_iter_(std::move(h))
	{
		if (source_file.is_open()) 
			loaded = true;
	}

	void operator++() {
		std::string line;
		bool paragraph_found = false;
		bool wrong_level_para_found = false;
		while (std::getline(source_file, line)) {
			if ((line.find_first_not_of(" \t\r\n\v\f") != std::string::npos) && 
				(hash_iter_->second.paragraph_level <= level)) 
			{
				if (hash_iter_->second.paragraph_level <= level) {
					//found paragraph and level checks out
					paragraph_found = true;
					paragraph.append(line);
				}
				else {
					
				}
			}
			else if ((line.find_first_of(" \t\r\n\v\f") != std::string::npos) && paragraph_found) {
				//found first empty after paragraph
				break;
			}
			else if ((line.find_first_of(" \t\r\n\v\f") != std::string::npos) && !paragraph_found) {
				//found empty line, but paragarph wasn't processed
				hash_iter_++;
			}
		}
	}
private:
	std::fstream source_file;
	my_string paragraph;
	level_t level;
	hashes_container::iterator hash_iter_;
	bool loaded = false;
};

class Article {
public:
	explicit Article() = default;
	explicit Article(const my_string&);
	pk_t get_author() { return _author_id; }
	void open_fstream(std::fstream& stream);
	void load_information();
	std::uint64_t get_length();
	[[nodiscard]] article_format get_format() const {
		return _format;
	}
	category_container_const_iter get_categories();
	my_string get_path_to_file();
	[[nodiscard]] hash_t get_main_hash() const {
		return _main_hash;
	}
	void calculate_hashes(hashes_container&);

	template<class Peer_t, class NewspaperEntry_t>
	void initialize_article(const category_container &categories, const std::string& file_path, 
							const Peer_t& me, const NewspaperEntry_t& news_entry );

	[[nodiscard]] bool is_in_category(const std::string& category) const;
	[[nodiscard]] pk_t get_author() const {
		return _author_id;
	}

	my_string select_level(level_t level);

	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & _author_name;
		ar & _author_id;
		ar & _news_name;
		ar & _news_id;
		ar & _main_hash;
		ar & _heading;
		ar & _hashes;
		ar & _length;
		ar & _format;
		ar & _categories;
		ar & _path_to_article_file;
		ar & _margins;
		ar & _notes;
	}

	friend class ArticleDatabase;

private:
	my_string _author_name;
	pk_t _author_id;
	my_string _news_name;
	pk_t _news_id;
	hash_t _main_hash;
	my_string _heading;
	//map <paragraph_position, hash>
	hashes_container _hashes;
	std::uint64_t _length;
	article_format _format;
	std::set<my_string> _categories;
	my_string _path_to_article_file;
	std::map<pk_t, Margin> _margins;
	my_string _notes;
};

using article_ptr = std::shared_ptr<Article>;
using article_optional = std::optional<article_ptr>;

using article_database_container = std::map<hash_t, Article>;

struct AuthorPeers {
	Article article;
	std::set<pk_t> peers;
	explicit AuthorPeers(Article a) :
		article(std::move(a)) {}
};

/*
 * Category -> Article(hash) -> Article(actual), Readers (those, who downloaded those articles)
 */
using category_t = std::string;
using user_multimap_container = std::unordered_map<hash_t, AuthorPeers>;
using category_multimap_container = std::unordered_multimap<my_string, user_multimap_container>;

#endif //PROGRAM_ARTICLE_H
