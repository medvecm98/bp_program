#ifndef PROGRAM_PEER_H
#define PROGRAM_PEER_H

#include "GlobalUsing.h"
#include "Networking.h"
#include "NewspaperEntry.h"
#include <unordered_map>
#include "Message.h"
#include <functional>
#include <QtNetwork/QHostAddress>
#include <QObject>


using MFW = MessageFactoryWrapper;
using reader_database = std::unordered_multimap<hash_t, PeerInfo*>;


/**
 * @brief Main class to describe the peer.
 * 
 * Class handles all important databases, public and private keys, networking, articles, message generation,
 * message handling, basic peer information.
 * 
 * Message needs to inherit `QObject`, because it's using slots and signals from Qt.
 */
class Peer : public QObject {

	Q_OBJECT

public:
	/**
	 * @brief Construct a new Peer object.
	 * 
	 * Networking is initialized here, random public identifier is generated and various
	 * QObject connections with `Networking` are established. Newspaper ID is not yet
	 * set, and same goes for name for both newspaper and peer. `CryptoPP` public and
	 * private keys are generated here as well.
	 * 
	 */
	Peer() : networking_(std::make_shared<Networking>())
	{
		std::cout << "Peer constructor" << std::endl;
		name_ = "";
		newspaper_name_ = "";
		networking_->init_sender_receiver(&news_);
		CryptoPP::AutoSeededRandomPool prng;
		std::random_device rd("/dev/urandom");
		public_key_ = rd();

		CryptoPP::RSA::PrivateKey private_key_new;
		private_key_new.GenerateRandomWithKeySize(prng, 2048);
		CryptoPP::RSA::PublicKey public_key_new(private_key_new);

		networking_->ip_map_.my_ip.add_rsa_key(std::move(public_key_new));
		networking_->ip_map_.private_rsa = {std::move(private_key_new)};

		newspaper_id_ = 0;

		/* slots and signals connections: */
		QObject::connect(this, &Peer::ip_credentials_arrived,
						 &(*networking_), &Networking::send_message_again_ip);

		QObject::connect(this, &Peer::new_symmetric_key,
						 &(*networking_), &Networking::decrypt_encrypted_messages);

		QObject::connect(&(*networking_), &Networking::new_message_received,
						 this, &Peer::handle_message);

		QObject::connect(this, &Peer::symmetric_key_exchanged, 
						 &(*networking_), &Networking::symmetric_exchanged);
	}

	void load_ip_authorities(pk_t newspaper_key); //to load the IPs of authorities
	void enroll_new_article(Article article); //add new article to list of category -> article
	void add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, const std::string& newspaper_ip);
	size_t list_all_articles_from_news(article_container& articles, const std::set<category_t>& categories);
	size_t list_all_articles_from_news(article_container& articles);
	size_t list_all_articles_by_me(article_container& articles, const std::set<category_t>& categories, pk_t news_id = 0);
	size_t list_all_articles_by_me(article_container& articles, pk_t news_id = 0);
	article_optional find_article(hash_t article_hash);
	optional_author_peers find_article_in_article_categories_db(hash_t article_hash);
	optional_author_peers find_article_in_article_categories_db(hash_t article_hash, category_container categories);
	void handle_requests(unique_ptr_message message);
	void handle_responses(unique_ptr_message message);
	void handle_one_way(unique_ptr_message message);
	void handle_error(unique_ptr_message message);
	void generate_article_all_message(pk_t destination, hash_t article_hash);
	void generate_article_header_message(pk_t destination, hash_t article_hash);

	/**
	 * @brief Set the name of the peer.
	 * 
	 * @param name Desired name, which will be set.
	 */
	void set_name(const my_string& name) {
		name_ = name;
	}

	/**
	 * @brief Get ID of my news.
	 * 
	 * @return pk_t ID of my news
	 */
	pk_t get_my_news_id() {
		return newspaper_id_;
	}

	/**
	 * @brief Set the ip of the peer.
	 * 
	 * @param ip Desired IP to be set.
	 */
	void set_my_ip(QString ip) {
		networking_->ip_map_.my_ip.ipv4 = QHostAddress(ip);
	}

	void init_newspaper(my_string name);

	/**
	 * @brief Generator for article list message.
	 * 
	 * @param destination ID of newspaper which article list we want.
	 */
	void generate_article_list(pk_t destination) {
		networking_->enroll_message_to_be_sent(
			MFW::ReqArticleListFactory(
				MFW::ArticleListFactory(
					public_key_,
					destination
				),
				std::vector<my_string>()
			)
		);
	}
	
	/**
	 * @brief Generator for article list message, with support for categories.
	 * 
	 * @tparam Container Category container type.
	 * @param destination ID of newspaper which article list we want.
	 * @param categories Categories we want.
	 */
	template<typename Container>
	void generate_article_list(pk_t destination, const Container& categories) {
		networking_->enroll_message_to_be_sent(
			MFW::ReqArticleListFactory<Container>(
				MFW::ArticleListFactory(
					public_key_,
					destination
				),
				categories
			)
		);
	}

	/**
	 * @brief Generator for add margin message.
	 * 
	 * For one margin.
	 * 
	 * @param article_hash Article hash of article, to which we want to add the margin.
	 * @param type Type of the margin.
	 * @param content Content of the margin.
	 */
	void add_margin(hash_t article_hash, my_string type, my_string content) {
		auto article = find_article(article_hash);
		if (article.has_value()) {
			auto author = article.value()->author_id();
			std::vector<Margin> vm = { Margin(type, content) };
			margins_added_.insert({article_hash, vm.back()});
			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
					MFW::UpdateMarginAddFactory(
						public_key_,
						author,
						article_hash,
						vm
					)
				)
			);
		}
		else {
			//TODO: log error
		}
	}

	/**
	 * @brief Generator for add margin message.
	 * 
	 * For multiple margins.
	 * 
	 * @param article_hash Article hash of article, to which we want to add the margins.
	 * @param vm Margin vector containing the margins.
	 */
	void add_margin(hash_t article_hash, margin_vector& vm) {
		auto article = find_article(article_hash);
		if (article.has_value()) {
			auto author = article.value()->author_id();
			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
					MFW::UpdateMarginAddFactory(
						public_key_,
						author,
						article_hash,
						vm
					)
				)
			);
		}
		else {
			//TODO: log error
		}
	}

	/**
	 * @brief Updates existing margin in given article.
	 * 
	 * For one margin only.
	 * 
	 * @param article_hash Article hash of article, where we want to update the margins.
	 * @param id ID of margin we want to update.
	 * @param type Type we want to set.
	 * @param content Content we want to set.
	 */
	void update_margin(hash_t article_hash, unsigned int id, my_string type, my_string content) {
		auto article = find_article(article_hash);
		if (article.has_value()) {
			auto author = article.value()->author_id();
			std::vector<Margin> vm = { Margin(type, content, id) };
			
			auto [marginb, margine] = margins_added_.equal_range(article_hash);

			for (; marginb != margine; marginb++) {
				//update our margin database first
				if (marginb->second.id == id) {
					marginb->second.type += ' ' + type;
					marginb->second.content += ' ' + content;
					break;
				}
			}

			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
					MFW::UpdateMarginUpdateFactory(
						public_key_,
						author,
						article_hash,
						vm
					)
				)
			);
		}
		else {
			//TODO: log error
		}
	}

	/**
	 * @brief Removes existing margin in given article.
	 * 
	 * @param article_hash Article hash of article, where we want to remove the margin.
	 * @param id ID of margin to remove.
	 */
	void remove_margin(hash_t article_hash, unsigned int id) {
		auto article = find_article(article_hash);
		if (article.has_value()) {
			auto author = article.value()->author_id();
			Margin m;
			m.id = id;
			
			auto [marginb, margine] = margins_added_.equal_range(article_hash);

			for (; marginb != margine; marginb++) {
				if (marginb->second.id == id) {
					m.type = marginb->second.type;
					m.content = marginb->second.content;
					margins_added_.erase(marginb);
					break;
				}
			}

			std::vector<Margin> vm = {m};

			networking_->enroll_message_to_be_sent(
				MFW::SetMessageContextRequest(
					MFW::UpdateMarginRemoveFactory(
						public_key_,
						author,
						article_hash,
						vm
					)
				)
			);
		}
		else {
			//TODO: log error
		}
	}

	/**
	 * @brief Public identifier getter.
	 * 
	 * @return pk_t Public identifier.
	 */
	pk_t get_public_key() {
		return public_key_;
	}

	/**
	 * @brief Name getter.
	 * 
	 * @return my_string Name
	 */
	my_string get_name() {
		return name_;
	}

	/**
	 * @brief Print Peer contents to standart output.
	 * 
	 * For debugging only.
	 * 
	 */
	void print_contents() {
		std::cout << "public_key_ " << public_key_ << std::endl;
		std::cout << "name_ " << name_ << std::endl;
		std::cout << "newspaper_id_ " << newspaper_id_ << std::endl;
		std::cout << "newspaper_name_ " << newspaper_name_ << std::endl;
		std::cout << "news_ count: " << news_.size() << std::endl;
	}

	/**
	 * @brief News database getter.
	 * 
	 * Returns non-const reference to newspaper database.
	 * Use with caution.
	 * 
	 * @return news_database& Reference to requested news.
	 */
	news_database& get_news_db() {
		return news_;
	}

	/**
	 * @brief Restarts `PeerReceiver` server in `Networking`.
	 * 
	 * For example, when IP is changed.
	 * 
	 */
	void restart_server(bool restart = true) {
		networking_->restart_server(restart);
	}

	/**
	 * @brief News name getter. 
	 * 
	 * @return std::string News name.
	 */
	std::string get_my_news_name() {
		return newspaper_name_;
	}

	/**
	 * @brief Serialize using boost archive.
	 * 
	 * @tparam Archive Archive type.
	 * @param ar Archive to use.
	 */
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & user_map;
		ar & news_;
		ar & public_key_;
		ar & name_;
		ar & networking_;
		ar & margins_added_;
		ar & article_headers_only;
		ar & readers_;
		ar & newspaper_all_readers;
		ar & articles_categories_;
		ar & newspaper_name_;
		ar & newspaper_id_;
		ar & authorities_;
		ar & journalists_;
	}

	void networking_init_sender_receiver() {
		networking_->init_sender_receiver(&news_);
	}

public slots:
	void handle_message(unique_ptr_message message);

signals:
	/**
	 * @brief Confirmation, that requested newspaper exists and are communicating.
	 * 
	 * @param news_id Confirmed newspaper ID.
	 */
	void got_newspaper_confirmation(pk_t news_id);
	
	/**
	 * @brief New article list received.
	 * 
	 * @param newspaper_id Which newspaper article list was received.
	 */
	void new_article_list(pk_t newspaper_id);

	/**
	 * @brief IP credentials response arrived.
	 * 
	 * @param message_originally_to Whom we sent original message and whom IP we want to know.
	 */
	void ip_credentials_arrived(pk_t message_originally_to);

	/**
	 * @brief New symmetric key arrived.
	 * 
	 * @param key_sender With whom we will share the symmetric key.
	 */
	void new_symmetric_key(pk_t key_sender);

	/**
	 * @brief Verification, whether user is or is not member of certain group arrived.
	 * 
	 * @param message_seq_number Message sequence number for DB access.
	 * @param is_member If user was or wasn't a member.
	 */
	void user_is_member_verification(seq_t message_seq_number, bool is_member);

	/**
	 * @brief Successful exchange of symmetric keys.
	 * 
	 * @param other_peer Other peer that will share this symmetric key.
	 */
	void symmetric_key_exchanged(pk_t other_peer);

private:
	//reader part
	pk_t public_key_; //public identifier of my peer
	my_string name_; //name of my peer
	std::shared_ptr<Networking> networking_; //networking, for handling sending and receiving
	news_database news_; //list of all downloaded articles, mapped by their Newspapers

	std::unordered_multimap<hash_t, Margin> margins_added_; //multimap of Article -> Margins, that this peer added, or requested to add
	std::unordered_map<pk_t, Article> article_headers_only; //only for article headers, so it won't interfere with regular ones
	std::unordered_map<pk_t, NewspaperEntry> newspapers_awaiting_confirmation; //newspaper that we want to add, but that haven't yet confirmed their existence

	//journalist part
	reader_database readers_; //list of article readers

	//authorities
	user_level_map user_map; //users and their levels, contains `PeerInfo`

	//chief editor
	user_multimap_container newspaper_all_readers; //list of all readers of all articles
	category_multimap_container articles_categories_; //articles mapped by categories, maps to `newspaper_all_readers`
	my_string newspaper_name_; //my newspaper name
	pk_t newspaper_id_; //public identifier of my newspaper
	user_container authorities_; //list of authorities
	user_container journalists_; //list of journalists

	article_optional find_article_in_database(hash_t article_hash);
};

#endif //PROGRAM_PEER_H
