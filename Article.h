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

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "StringSplitter.h"
#include "GlobalUsing.h"
#include "Margins.h"
#include "protobuf_source/articles.pb.h"

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

#define WHITESPACE " \t\r\n\v\f"

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
using margin_container = std::unordered_multimap<pk_t, Margin>;
using margin_vector = std::vector<Margin>;

class ParagraphIterator {
public:
//TODO: use stringstream
	ParagraphIterator(std::string source_file_path, level_t l, hashes_container::iterator h) :
		source_file_(source_file_path), level_(l), hash_iter_(std::move(h))
	{
		if (source_file_.is_open()) 
			loaded_ = true;
	}

	void operator++() {
		std::string line;
		paragraph_.clear();
		bool paragraph_found = false;
		bool wrong_level_para_found = false;
		while (std::getline(source_file_, line)) {
			if (line.find_first_not_of( WHITESPACE ) != std::string::npos) {
				if (hash_iter_->second.paragraph_level <= level_) {
					//found paragraph and level checks out
					paragraph_found = true;
					#if defined(_WIN32) 
						paragraph_.append(line).append("\r\n");
					#else
						paragraph_.append(line).append("\n");
					#endif
				}
				else {
					wrong_level_para_found = true;
				}
			}
			else if ((line.find_first_not_of( WHITESPACE ) == std::string::npos) && paragraph_found) {
				//found first empty after paragraph
				break;
			}
			else if ((line.find_first_not_of( WHITESPACE ) == std::string::npos) && wrong_level_para_found) {
				//found empty line, but paragarph wasn't processed
				wrong_level_para_found = false;
				hash_iter_++;
			}
		}
	}

	optional_my_string get() {
		if (!paragraph_.empty())
			return optional_my_string(paragraph_);
		else
			return optional_my_string();
	}

	level_t get_level() {
		return level_;
	}

	void reset() {
		if (source_file_.is_open())
			source_file_.seekg(0);
	}
private:
	std::fstream source_file_;
	my_string paragraph_;
	level_t level_;
	hashes_container::iterator hash_iter_;
	bool loaded_ = false;
};

class Article {
public:
	Article() = default;
	explicit Article(const np2ps::Article& protobuf_article);
	Article(const np2ps::Article& protobuf_article_header, const std::string& article_actual);
	void open_fstream(std::fstream& stream);
	void load_information();
	[[nodiscard]] article_format get_format() const {
		return _format;
	}
	category_container_const_iter get_categories();
	my_string get_path_to_file();
	
	void calculate_hashes(hashes_container&);

	/**
	 * \brief Used to initialize a new article, from "scratch".
	 *
	 * @tparam T Container of categories.
	 * @param path_header
	 * @param author_name
	 * @param author_id
	 * @param categories
	 */
	template<class Container, class Peer_t, class NewspaperEntry_t>
	void initialize_article(const Container &categories, const std::string& file_path,
		Peer_t& me, NewspaperEntry_t& news_entry )
	{

		_path_to_article_file = file_path;

		bool at_least_one_category = false;
		for (auto&& i : categories) {
			at_least_one_category = true;
			_categories.insert(i);
		}

		if (!at_least_one_category) {
			_categories.insert("no_cat");
		}

		_news_name = news_entry.get_name();
		_news_id = news_entry.get_id();
		_author_name = me.get_name();
		_author_id = me.get_public_key();
		_heading = "";

		/* main hash, hashes, length and heading are calculated and found here: */
		calculate_hashes(_hashes);

		if (!categories.empty()) {
			for (auto&& cat : categories) {
				_categories.insert(cat);
			}
		}


	}


	bool is_in_category(const std::string& category) const;

	pk_t author_id() const { return _author_id; }

	my_string author_name() const { return _author_name; }

	pk_t news_id() const { return _news_id; }

	my_string news_name() const { return _news_name; }

	hash_t main_hash() const { return _main_hash; }

	my_string heading() const { return _heading; }

	bool article_present() const { return article_present_; }

	std::pair<hashes_container::const_iterator, hashes_container::const_iterator> hashes() const { return {_hashes.cbegin(), _hashes.cend()}; }

	std::size_t length() const { return _length; }

	std::pair<category_container::const_iterator, category_container::const_iterator> categories() const { return {_categories.cbegin(), _categories.cend()}; }

	std::pair<margin_container::const_iterator, margin_container::const_iterator> margins() const { return {_margins.cbegin(), _margins.cend()}; }

	void select_level(my_string& rv, level_t level);

	void add_margin(pk_t pk, Margin&& margin) {
		_margins.insert({pk, std::move(margin)});
	}

	void remove_margin(margin_container::iterator what) {
		_margins.erase(what);
	}

	std::pair<margin_container::iterator, margin_container::iterator> get_range_iterators(pk_t pk) {
		return _margins.equal_range(pk);
	}

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
	margin_container _margins;
	my_string _notes;
	bool article_present_;
};

using article_ptr = std::shared_ptr<Article>;
using article_optional = std::optional<article_ptr>;
using article_container = std::unordered_set<article_ptr>;
using article_database_container = std::map<hash_t, Article>;

/**
 * @brief Struct for encapsulation of all the readers of one specific article.
 * 
 * Contains all the readers of given article and article header.
 */
struct ArticleReaders {
	Article article;
	std::unordered_map<pk_t, user_variant> readers;
	ArticleReaders(Article a, pk_t user, user_variant reader) 
	: article(std::move(a))
	{
		readers.insert({user, reader});
	}

	void add_reader(pk_t user, user_variant reader) {
		readers.insert({user, reader});
	}
};

/*
 * Category -> Article(hash) >-ArticleReaders-> Article(actual), Readers (those, who downloaded those articles)
 */
using category_t = std::string;
using user_multimap_container = std::unordered_map<hash_t, ArticleReaders>;
using category_multimap_container = std::unordered_multimap<my_string, user_multimap_container::iterator>;
using optional_author_peers = std::optional<std::shared_ptr<ArticleReaders>>;

#endif //PROGRAM_ARTICLE_H
