#include "NewspaperEntry.h"

NewspaperEntry::NewspaperEntry(pk_t id, DisconnectedUsersLazy* disconnected_users_lazy) :
	news_id_(id),
	disconnected_readers_lazy_remove(disconnected_users_lazy)
{
	
}

NewspaperEntry::NewspaperEntry(pk_t first_key, pk_t id, const my_string& name, DisconnectedUsersLazy* disconnected_users_lazy) :
	disconnected_readers_lazy_remove(disconnected_users_lazy)
{
	_authorities.insert(first_key);
	news_id_ = id;
	news_name_= name;
}

NewspaperEntry::NewspaperEntry(const np2ps::LocalSerializedNewspaperEntry& serialized_ne, DisconnectedUsersLazy* disconnected_users_lazy) : 
	news_id_(serialized_ne.entry().news_id()),
	news_name_(serialized_ne.entry().news_name()),
	last_updated_(serialized_ne.last_updated()),
	disconnected_readers_lazy_remove(disconnected_users_lazy)
{
	for(const np2ps::SerializedArticle& gpb_articles : serialized_ne.articles()) {
		add_article(gpb_articles.article().main_hash(), Article(gpb_articles));
	}
	for (auto f : serialized_ne.friends()) {
		add_friend(f);
	}
	set_newspaper_public_key(
		CryptoUtils::instance().hex_to_rsa(
			serialized_ne.network_info().rsa_public_key()
		)
	);
	if (serialized_ne.has_private_key()) {
		set_newspaper_private_key(
			CryptoUtils::instance().hex_to_private(
				serialized_ne.private_key()
			)
		);
	}
}

NewspaperEntry::NewspaperEntry(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne, DisconnectedUsersLazy* disconnected_users_lazy) :
	news_id_(serialized_ne.entry().news_id()),
	news_name_(serialized_ne.entry().news_name()),
	last_updated_(0),
	disconnected_readers_lazy_remove(disconnected_users_lazy)
{
	set_newspaper_public_key(
		CryptoUtils::instance().hex_to_rsa(
			serialized_ne.network_info().rsa_public_key()
		)
	);
	// for(const np2ps::Article& gpb_articles : serialized_ne.articles()) {
	// 	add_article(gpb_articles.main_hash(), Article(gpb_articles));
	// }
}

NewspaperEntry::NewspaperEntry(const std::string& path, DisconnectedUsersLazy* disconnected_users_lazy) {
	
}

void NewspaperEntry::deserialize(const np2ps::NetworkSerializedNewspaperEntry& serialized_ne) {
	if (news_name_.empty()) {
		news_name_ = serialized_ne.entry().news_name();
	}
}

void NewspaperEntry::add_article(hash_t article_hash, Article&& article) {
	_articles.emplace(article_hash, article);
	time_sorted_articles.emplace(article.creation_time(), article_hash);
}

bool NewspaperEntry::remove_article(hash_t article_hash) {
	return _articles.erase(article_hash) > 0;
}

std::optional<article_ptr> NewspaperEntry::find_article_header(hash_t article_hash) {
	try {
		article_ptr article_ptr_ = &get_article(article_hash);
		return {article_ptr_};
	}
	catch (article_not_found_database& e) {
		return {};
	}
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
	timed_article_map_iter bit = time_sorted_articles.rbegin();
	timed_article_map_iter eit = time_sorted_articles.rend();

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

timed_article_map_pair NewspaperEntry::get_newest_articles(int from, int to) {
	timed_article_map_iter bit = time_sorted_articles.rbegin();
	timed_article_map_iter eit = time_sorted_articles.rend();

	if (to - from <= 0) {
		return { bit, eit };
	}

	std::size_t i_skip = 0;

	while (i_skip < from) {
		if (bit == eit) {
			return {bit, eit};
		}
		i_skip++;
		bit++;
	}

	timed_article_map_iter it = bit;
	std::size_t i_article = i_skip;

	while (it != eit && i_article < to) {
		i_article++;
		it++;
	}

	return { bit, it };
}

void NewspaperEntry::get_newest_articles(article_container& articles, int count, timestamp_t timestamp) {
	auto [bit, eit] = get_newest_articles(count);

	for(; bit != eit; bit++) {
		article_ptr article = &get_article(bit->second);
		if (timestamp != 0 && article->creation_time() < timestamp) {
			break;
		}
		articles.emplace(article);
	}
}

void NewspaperEntry::get_newest_articles(article_container& articles, int from, int to, timestamp_t timestamp) {
	auto [bit, eit] = get_newest_articles(from, to);

	for(; bit != eit; bit++) {
		article_ptr article = &get_article(bit->second);
		if (timestamp != 0 && article->creation_time() < timestamp) {
			break;
		}
		articles.emplace(article);
	}
}

/** Get `count` newest articles from start of given day.
*/
timed_article_map_pair NewspaperEntry::get_newest_articles(QDate date, int count) {
	QDateTime date_time = date.startOfDay(Qt::LocalTime);
	std::int64_t epoch = date_time.toMSecsSinceEpoch();

	timed_article_map_iter bit = time_sorted_articles.rbegin();
	timed_article_map_iter eit = time_sorted_articles.rend();
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
		auto& article = _articles[id];
		remove_disconnected_readers(article);
		return article;
	}
	throw article_not_found_database("Article not found in user database.");
}

std::size_t NewspaperEntry::friend_count() const {
	return friends_.size();
}

user_container_citer NewspaperEntry::get_first_authority() const {
	return _authorities.cbegin();
}
std::size_t NewspaperEntry::get_authority_count() const {
	return _authorities.size();
}

pk_t NewspaperEntry::get_id() const {
	return news_id_;
}

const my_string& NewspaperEntry::get_name() const {
	return news_name_;
}

level_t NewspaperEntry::level() {
	return level_;
}

ArticleListWrapper& NewspaperEntry::get_list_of_articles() {
	return article_list_wrapper_;
};

void NewspaperEntry::serialize_entry(np2ps::NewspaperEntry* entry) const {
	std::cout << "serializing newspaper\n";
	entry->set_news_name(news_name_);
	entry->set_news_id(news_id_);
}

void NewspaperEntry::network_serialize_entry(np2ps::NetworkSerializedNewspaperEntry* nserialized_ne, IpWrapper& news_wrapper) const {
	serialize_entry(nserialized_ne->mutable_entry());
	for (auto& [hash, art] : _articles) {
		np2ps::Article* pa = nserialized_ne->add_articles();
		art.network_serialize_article(pa);
	}
	nserialized_ne->mutable_network_info()->set_ipv4(news_wrapper.ipv4.toIPv4Address());
	nserialized_ne->mutable_network_info()->set_port(news_wrapper.port);
	nserialized_ne->mutable_network_info()->set_publicid(news_id_);
	nserialized_ne->mutable_network_info()->set_rsa_public_key(
		CryptoUtils::instance().rsa_to_hex(
			get_newspaper_public_key_value()
		)
	);
}

void NewspaperEntry::local_serialize_entry(np2ps::LocalSerializedNewspaperEntry* lserialized_ne) const {
	serialize_entry(lserialized_ne->mutable_entry());
	for (auto& [hash, art] : _articles) {
		np2ps::SerializedArticle* pa = lserialized_ne->add_articles();
		art.local_serialize_article(pa);
	}
	for (auto&& f : friends_) {
		lserialized_ne->add_friends(f);
	}
	if (has_newspaper_public_key()) {
		lserialized_ne->mutable_network_info()->set_rsa_public_key(
			CryptoUtils::instance().rsa_to_hex(
				get_newspaper_public_key_value()
			)
		);
	}
	if (has_newspaper_private_key()) {
		lserialized_ne->set_private_key(
			CryptoUtils::instance().private_to_hex(
				get_newspaper_private_key_value()
			)
		);
	}
	lserialized_ne->set_last_updated(last_updated_);
}

void NewspaperEntry::fill_time_sorted_articles() {
	if (time_sorted_articles.empty()) {
		for (auto&& article : _articles) {
			time_sorted_articles.emplace(article.first, article.second.creation_time());
		}
	}
}

bool NewspaperEntry::has_newspaper_public_key() const {
	return newspaper_public_key_.has_value();
}

void NewspaperEntry::set_newspaper_public_key(CryptoPP::RSA::PublicKey pk) {
	newspaper_public_key_ = {pk};
}

rsa_public_optional NewspaperEntry::get_newspaper_public_key() {
	return newspaper_public_key_;
}

CryptoPP::RSA::PublicKey NewspaperEntry::get_newspaper_public_key_value() const {
	if (newspaper_public_key_.has_value())
		return newspaper_public_key_.value();

	throw other_error("No key for given newspaper found.");
}

bool NewspaperEntry::has_newspaper_private_key() const {
	return newspaper_private_key_.has_value();
}

void NewspaperEntry::set_newspaper_private_key(CryptoPP::RSA::PrivateKey pk) {
	newspaper_private_key_ = {pk};
}

rsa_private_optional NewspaperEntry::get_newspaper_private_key() {
	return newspaper_private_key_;
}

CryptoPP::RSA::PrivateKey NewspaperEntry::get_newspaper_private_key_value() const {
	if (newspaper_private_key_.has_value())
		return newspaper_private_key_.value();

	throw other_error("No key for given newspaper found.");
}

void NewspaperEntry::sign_article(hash_t id) {
	Article& article = get_article(id);
	if (has_newspaper_private_key()) {
		auto signature = CryptoUtils::instance().sign_with_keys(
			get_newspaper_private_key(), article.get_crypto_hash()
		);
		article.set_signature(signature);
	}
	else {
		throw other_error("Private key for given newspaper not found.");
	}
}

bool NewspaperEntry::verify_article_signature(hash_t id) {
	auto& article = get_article(id);
	return CryptoUtils::instance().verify_signature_with_keys(
		get_newspaper_public_key(),
		article.get_crypto_hash(),
		article.get_signature()
	);
}

bool NewspaperEntry::confirmation() {
	return confirmed;
}

void NewspaperEntry::set_confirmation(bool c) {
	confirmed = c;
}

article_database_container& NewspaperEntry::get_all_articles() {
	remove_disconnected_readers_all(false);
	return _articles;
}

void NewspaperEntry::remove_disconnected_readers(hash_t article) {
	Article& article_ref = _articles.at(article);
	remove_disconnected_readers(article_ref);
}

void NewspaperEntry::remove_disconnected_readers(Article& article) {
	for (auto&& disconnected_reader : disconnected_readers_lazy_remove->users) {
		article.readers().erase(disconnected_reader);
	}
}

void NewspaperEntry::remove_disconnected_readers_all(bool ignore_treshold) {
	if (disconnected_readers_lazy_remove->users.size() >= 20 || ignore_treshold) {
		for (auto&& disconnected_reader : disconnected_readers_lazy_remove->users) {
			for (auto&& article : _articles) {
				remove_disconnected_readers(article.second);
			}
		}
		disconnected_readers_lazy_remove->users.clear();
	}
}

void NewspaperEntry::confirmed_journalist(pk_t pid) {
	journalists_.emplace(pid);
}

const user_container& NewspaperEntry::get_journalists() {
	return journalists_;
}

void NewspaperEntry::remove_journalist(pk_t pid) {
	journalists_.erase(pid);
}

void NewspaperEntry::update() {
	last_updated_ = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count();
}
