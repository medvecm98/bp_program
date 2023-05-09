#ifndef PROGRAM_NEWSPAPERENTRY_H
#define PROGRAM_NEWSPAPERENTRY_H

#include "GlobalUsing.h"
#include "LazyWrappers.hpp"
#include "Article.h"
#include "CryptoUtils.hpp"
#include "IpMap.h"
#include <optional>

using database_iterator_t = article_database_container::iterator;
using article_data_vec = std::vector<database_iterator_t>;

struct ArticleListWrapper {
	std::set<my_string> categories;
	std::unordered_map<hash_t, Article> article_headers; 
};

class NewspaperEntry {
public:
	NewspaperEntry() = default;
	NewspaperEntry(pk_t first_key, pk_t id, const my_string& name, DisconnectedUsersLazy* disconnected_users_lazy);
	explicit NewspaperEntry(pk_t id, DisconnectedUsersLazy* disconnected_users_lazy);
	explicit NewspaperEntry(const np2ps::LocalSerializedNewspaperEntry& serialized_ne, DisconnectedUsersLazy* disconnected_users_lazy);
	explicit NewspaperEntry(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne, DisconnectedUsersLazy* disconnected_users_lazy);
	explicit NewspaperEntry(const std::string& path, DisconnectedUsersLazy* disconnected_users_lazy);
	void deserialize_config(const np2ps::NewspaperConfig& config);
	void add_article(hash_t article_hash, Article&& article);
	timed_article_map_pair get_newest_articles(int count);
	timed_article_map_pair get_newest_articles(int from, int to);
	void get_newest_articles(article_container& articles, int count, timestamp_t timestamp);
	void get_newest_articles(article_container& articles, int from, int to, timestamp_t timestamp);
	timed_article_map_pair get_newest_articles(QDate date, int count);
	bool remove_article(hash_t article_hash);
	std::optional<article_ptr> find_article_header(hash_t article_hash);
	database_iterator_t get_iterator_database();
	database_iterator_t get_iterator_database_end();
	article_data_vec get_articles_for_time_span(my_clock::time_point time_span_begin, my_clock::time_point time_span_end);
	void add_reader(pk_t id);
	const user_container& get_readers();
	std::size_t reader_count() const;
	void remove_reader(pk_t id);
	Article& get_article(hash_t id);
	std::size_t get_article_count();
	void deserialize(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne);

	user_container_citer get_first_authority() const;

	std::size_t get_authority_count() const;

	pk_t get_id() const;

	const my_string& get_name() const;

	level_t level();

	ArticleListWrapper& get_list_of_articles();

	void serialize_entry(np2ps::NewspaperEntry* entry) const;

	void network_serialize_entry(np2ps::NetworkSerializedNewspaperEntry* nserialized_ne, IpMap& news_wrapper, pk_t id) const;

	void local_serialize_entry(np2ps::LocalSerializedNewspaperEntry* lserialized_ne, bool serialize_articles = true) const;

	void serialize_entry_config(np2ps::NewspaperConfig* serialized_config) const;

	void fill_time_sorted_articles();

	bool has_newspaper_public_key() const ;

	void set_newspaper_public_key(CryptoPP::RSA::PublicKey pk) ;

	rsa_public_optional get_newspaper_public_key();

	CryptoPP::RSA::PublicKey get_newspaper_public_key_value() const;

	bool has_newspaper_private_key() const;

	void set_newspaper_private_key(CryptoPP::RSA::PrivateKey pk);

	rsa_private_optional get_newspaper_private_key() ;

	CryptoPP::RSA::PrivateKey get_newspaper_private_key_value() const;

	void sign_article(hash_t id);

	bool verify_article_signature(hash_t id);

	bool confirmation();

	void set_confirmation(bool c);


	article_database_container& get_all_articles();

	void remove_disconnected_readers(hash_t article);
	void remove_disconnected_readers(Article& article);
	void remove_disconnected_readers_all(bool ignore_treshold);

	bool await_confirmation = false;

	void confirmed_journalist(pk_t pid);
	const user_container& get_journalists();
	void remove_journalist(pk_t pid);

	void update();
	timestamp_t last_updated() {
		return last_updated_;
	}

	void set_article_limit_read(std::size_t limit);
	std::size_t get_article_limit_read();

	void set_article_limit_unread(std::size_t limit);
	std::size_t get_article_limit_unread();

	void emplace_journalist(pk_t pid) {
		if (journalists_.emplace(pid).second) {
			coworkers_.push_back(pid);
		}
	}

	pk_t get_next_coworker();
	std::list<pk_t>& get_coworkers();

	void update_metadata(NewspaperEntry& second_entry);

	void clear_abundant_articles();

	void set_config_read_articles_to_keep(int value);
	void set_config_unread_articles_to_keep(int value);

	int get_config_read_articles_to_keep();
	int get_config_unread_articles_to_keep();

private:
	void init_coworkers();

	pk_t news_id_;
	my_string news_name_;
	article_database_container _articles; //(downloaded) articles from these newspapers
	user_container _authorities; //authorities of these newspapers
	level_t level_ = 127; //level for given newspaper
	ArticleListWrapper article_list_wrapper_; //saving requested article list
	timed_article_map time_created_sorted_articles;
	timed_article_map time_modified_sorted_articles;
	user_container readers_; //friends, that can share the articles with you
	user_container journalists_;
	std::list<pk_t> coworkers_;
	rsa_public_optional newspaper_public_key_; //newspaper public key to check article legitimacy
	rsa_private_optional newspaper_private_key_;
	DisconnectedUsersLazy* disconnected_readers_lazy_remove;
	timestamp_t last_updated_ = 0;
	NewspaperGlobalConfig config;
	
	bool confirmed = false;
};

using news_database = std::unordered_map<pk_t, NewspaperEntry>;

/**
 * Functor, that covers one news company.
 */
struct TheSameNews {
	TheSameNews() = default;
	TheSameNews(news_database::iterator the_one, news_database::iterator end_i) :
		entry(the_one), end_iterator(end_i) {}
	news_database::iterator entry;
	news_database::iterator end_iterator;
	bool used = false;
	news_database::iterator operator() () {
		if (!used) {
			used = true;
			return entry;
		}
		else {
			return end_iterator;
		}
	}
};

/**
 * Functor, that covers all the various news companies.
 */
struct AllTheNews {
	explicit AllTheNews(news_database::iterator nd, news_database::iterator e) {
		data = nd;
		enddata = e;
	}
	news_database::iterator data;
	news_database::iterator enddata;
	news_database::iterator operator() () {
		std::cout << "AllTheNews operator()" << std::endl;
		if (data == enddata) {
			return enddata;
		}
		else {
			return data++;
		}
	}
};

#endif //PROGRAM_NEWSPAPERENTRY_H
