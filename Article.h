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
#include <chrono>

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>

#include "StringSplitter.h"
#include "CryptoUtils.hpp"
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

#define WHITESPACE " \t\r\n\v\f"

enum article_format {
	PlainText = 0,
	Markdown = 1,
	Html = 4
};

struct HashWrapper {
	HashWrapper() = default;
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

/**
 * @brief Class for newspaper article representation.
 * 
 * This class contains all neccessary metadata about newspaper article.
 * 
 */
class Article {
public:
	/**
	 * @brief Construct a new Article object
	 * 
	 * Default constructor
	 * 
	 */
	Article() = default;
	explicit Article(const np2ps::Article& protobuf_article);
	explicit Article(const np2ps::SerializedArticle& protobuf_article);
	Article(const np2ps::Article& protobuf_article_header, const std::string& article_actual);
	void open_fstream(std::fstream& stream);

	/**
	 * @brief Gets the article format.
	 * 
	 * Plain text or Markdown.
	 * 
	 * @return article_format Format of the article.
	 */
	article_format get_format() const {
		return _format;
	}

	category_container_const_iter get_categories();
	my_string get_path_to_file();
	
	void calculate_hashes();

	/**
	 * @brief For initialization of new articles from "scratch".
	 * 
	 * Used when peer is uploading his own article into the network.
	 * 
	 * @tparam Container Type of container for categories.
	 * @tparam Peer_t Type of peer that wrote this article.
	 * @tparam NewspaperEntry_t Type of newspaper that this article will be inserted in.
	 * @param categories Categories to include article in.
	 * @param file_path Path to article file.
	 * @param me Reference to peer that wrote the article.
	 * @param news_entry Reference to newspaper entry that this article will be inserted in.
	 */
	template<class Container, class Peer_t, class NewspaperEntry_t>
	void initialize_article(const Container &categories, const std::string& file_path, Peer_t& me, NewspaperEntry_t& news_entry, std::size_t version__ = 1, hash_t hash__ = 0 )
	{

		_path_to_article_file = file_path;
		ancestor_ = 0;

		{
			//parse article type from provided article file extension
			//... via splitting it using periods and reading the last entry
			//... of the split string

			QString qpath = QString::fromStdString(_path_to_article_file);

			if (qpath.split('.').last().toLower() == "md") {
				_format = article_format::Markdown;
			}
			else if (qpath.split('.').last().toLower() == "html") {
				_format = article_format::Html;
			}
			else {
				_format = article_format::PlainText;
			}
		}

		bool at_least_one_category = false;
		for (auto&& category : categories) { // insert all the categories into article category container
			std::cout << category << std::endl;
			at_least_one_category = true;
			_categories.insert(category);
		}

		if (!at_least_one_category) { //if there was no category inserted, insert the "no_cat" category
			_categories.insert("no_cat");
		}

		_news_name = news_entry.get_name();
		_news_id = news_entry.get_id();
		_author_name = me.get_name();
		_author_id = me.get_public_key();
		_heading = "";

		set_version(version__);
		QFile article_file;
		QString qfile_name = QString::fromStdString(_path_to_article_file);
		article_file.setFileName(qfile_name);
		article_file.open(QIODevice::ReadOnly);
		QTextStream file_stream(&article_file);
		QString qline = file_stream.readLine();
		while (!qline.isNull()) {
			qsizetype pos;
			if (_format == article_format::Markdown && (pos = qline.indexOf('#')) != -1) {
				_heading = qline.mid(pos + 1).trimmed().toStdString();
				break;
			}
			else if (_format == article_format::PlainText && qline.trimmed().size() > 0) {
				_heading = qline.toStdString();
				break;
			}
			qline = file_stream.readLine();
		}
		article_file.close();


		/* main hash, is calculated and found here: */

		std::string article_full = normalize_article_and_calculate_crypto_hash().toStdString();
		CryptoPP::AutoSeededRandomPool prng;
		if (hash__ == 0) {
			std::hash<std::string> hasher;
			_main_hash = hasher(article_full);
		}
		else {
			_main_hash = hash__;
		}
		set_path(article_full, get_version());

		creation_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();
		modification_time_ = creation_time_;
		

		article_present_ = true; //we provided the article file in function arguments, so the article's contents is 
								 //... naturally present
		

		sign_article_hash_newspaper(me.get_my_newspaper().get_newspaper_private_key());

		readers_.emplace(me.get_public_key());

	}

	/**
	 * @brief Checks if article belongs to the category.
	 * 
	 * @param category Category to check.
	 * @return true Article belongs to category.
	 * @return false Article does not belong to category.
	 */
	bool is_in_category(const std::string& category) const;

	/**
	 * @brief Gets the public identifier of the author of the article.
	 * 
	 * @return pk_t Public identifier of author.
	 */
	pk_t author_id() const { return _author_id; }

	/**
	 * @brief Gets the name of the author of the article.
	 * 
	 * @return pk_t Name of author.
	 */
	my_string author_name() const { return _author_name; }

	/**
	 * @brief Gets the public identifier of the author of the news.
	 * 
	 * @return pk_t Public identifier of news.
	 */
	pk_t news_id() const { return _news_id; }

	/**
	 * @brief Gets the name of the author of the news.
	 * 
	 * @return pk_t Name of news.
	 */
	my_string news_name() const { return _news_name; }

	/**
	 * @brief Gets the main hash of the article.
	 * 
	 * @return hash_t Main hash of the article.
	 */
	hash_t main_hash() const { return _main_hash; }

	/**
	 * @brief Gets the heading of the article.
	 * 
	 * @return my_string Heading of the article.
	 */
	my_string heading() const { return _heading; }

	/**
	 * @brief Checks if there is article's contents present and saved in file.
	 * 
	 * @return true If there is article's contents present.
	 * @return false If there is not article's contents present.
	 */
	bool article_present() const { return article_present_; }

	/**
	 * @brief Gets both begin and end iterators to `_hashes` container.
	 * 
	 * @return Pair of iterators.
	 */
	std::pair<hashes_container::const_iterator, hashes_container::const_iterator> hashes() const { return {_hashes.cbegin(), _hashes.cend()}; }

	/**
	 * @brief Gets the length of article.
	 * 
	 * @return std::size_t Length of article.
	 */
	std::size_t length() const { return _length; }

	/**
	 * @brief Gets both begin and end iterators to `_categories` container.
	 * 
	 * @return Pair of iterators.
	 */
	std::pair<category_container::const_iterator, category_container::const_iterator> categories() const { return {_categories.cbegin(), _categories.cend()}; }

	const std::set<my_string>& categories_ref() const {
		return _categories;
	}

	/**
	 * @brief Gets both begin and end iterators to `_margins` container.
	 * 
	 * @return Pair of iterators.
	 */
	std::pair<margin_container::const_iterator, margin_container::const_iterator> margins() const { return {_margins.cbegin(), _margins.cend()}; }

	void select_level(my_string& rv, level_t level);

	/**
	 * @brief Adds margin into `_margins` container.
	 * 
	 * @param pk Author of the margin.
	 * @param margin Margin to insert.
	 */
	void add_margin(pk_t pk, Margin&& margin) {
		_margins.emplace(pk, std::move(margin));
	}

	/**
	 * @brief Removes margin into `_margins` container.
	 * 
	 * @param what Iterator to margin container containing margin, that should be deleted.
	 */
	void remove_margin(margin_container::iterator what) {
		_margins.erase(what);
	}

	/**
	 * @brief Gets the iterators for margins from given author.
	 * 
	 * @param pk Author of the margins to find.
	 * @return Pair of iterators.
	 */
	std::pair<margin_container::iterator, margin_container::iterator> get_range_iterators(pk_t pk) {
		return _margins.equal_range(pk);
	}

	void set_path(const std::string& article_actual, std::size_t version = 1);

	QString normalize_article_and_calculate_crypto_hash();

	bool verify(const std::string& to_check);

	std::string get_crypto_hash();

	std::uint64_t creation_time() {
		return creation_time_;
	}

	std::uint64_t modification_time() {
		return modification_time_;
	}

	friend class ArticleDatabase;

	std::string read_contents() {
		QString path(_path_to_article_file.c_str());
		QFile file;
		file.setFileName(path);
		file.open(QIODevice::ReadOnly); //opens the file
		QTextStream text_stream(&file);
		QString line, contents;
		if (!text_stream.atEnd()) {
			line = text_stream.readLine();
			contents.append(line);
			contents.append('\n');
		}
		while (!text_stream.atEnd()) { 
			line = text_stream.readLine();
			contents.append(line); //loads the article line by line
			contents.append('\n');
		}
		return contents.toStdString();
	}

	void network_serialize_article(np2ps::Article* art) const;

	void local_serialize_article(np2ps::SerializedArticle* art) const;

	bool is_header_only() {
		return !article_present_;
	}

	const user_container& const_readers() const {
		return readers_;
	}

	user_container& readers() {
		return readers_;
	}

	void remove_reader(pk_t id) {
		auto it = readers_.find(id);
		if (it != readers_.end()) {
			readers_.erase(id);	
		}
	}

	void add_reader(pk_t id) {
		readers_.emplace(id);
	}

	template<typename Container>
	void add_readers(Container& reader_container) {
		for (auto&& reader : reader_container) {
			add_reader(reader);
		}
	}

	std::size_t readers_count() {
		return readers_.size();
	}

	void sign_article_hash_newspaper(rsa_private_optional key);
	bool verify_news_signature(rsa_public_optional key);
	std::string& get_signature();
	void set_signature(std::string signature);
	void lazy_remove_readers(user_container& disconnected_users);
	void update_metadata(Article& other_article);
	void set_read() {
		read_ = true;
	}
	bool get_read() {
		return read_;
	}

	std::size_t get_version() {
		return version_;
	}

	void set_version(std::size_t ver) {
		version_ = ver;
	}

	std::size_t get_ancestor() {
		return ancestor_;
	}

	void set_ancestor(std::size_t ancestor) {
		ancestor_ = ancestor;
	}

private:
	my_string _author_name; //network, local
	pk_t _author_id; //network, local
	my_string _news_name; //network, local
	pk_t _news_id; //network, local
	hash_t _main_hash; //network, local
	my_string _heading; //network, local
	hashes_container _hashes; //map of hashes, UNUSED
	std::uint64_t _length; //UNUSED
	article_format _format; //network, local, UNUSED
	std::set<my_string> _categories; //set of categories //network, local
	my_string _path_to_article_file; //local
	margin_container _margins; //unordered map of public identifiers and margins //network, local
	my_string _notes; //local
	std::string crypto_hash_; //network, local
	bool article_present_; //local
	std::uint64_t creation_time_; //network, local
	std::uint64_t modification_time_; //network, local
	my_timepoint creation_time_timepoint_; //network, local
	user_container readers_; //local, TODO: implement
	std::string newspaper_signature_;
	bool read_ = false;
	std::size_t version_;
	hash_t ancestor_;
};

using article_ptr = Article*;
using article_optional = std::optional<article_ptr>;
using article_container = std::set<article_ptr>;
using article_database_container = std::map<hash_t, Article>;

/**
 * @brief Struct for encapsulation of all the readers of one specific article.
 * 
 * Contains all the readers of given article and article header.
 */
struct ArticleReaders {
	Article article;
	std::unordered_map<pk_t, PeerInfo*> readers;
	ArticleReaders() = default;
	ArticleReaders(Article a, pk_t user, PeerInfo* reader) 
	: article(std::move(a))
	{
		readers.insert({user, reader});
	}

	/**
	 * @brief Adds reader into the map.
	 * 
	 * @param user Public identifier of reader.
	 * @param reader Pointer to PeerInfo object of the reader.
	 */
	void add_reader(pk_t user, PeerInfo* reader) {
		readers.insert({user, reader});
	}

	
};

/*
 * Category -> Article(hash) >-ArticleReaders-> Article(actual), Readers (those, who downloaded those articles)
 */
using category_t = std::string;
using user_multimap_container = std::unordered_map<hash_t, ArticleReaders>;
using category_multimap_container = std::unordered_multimap<my_string, ArticleReaders*>;
using optional_author_peers = std::optional<ArticleReaders*>;

#endif //PROGRAM_ARTICLE_H
