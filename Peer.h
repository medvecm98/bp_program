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
	Peer(pk_t public_id) : 
		networking_(std::make_shared<Networking>()) 
	{
		CryptoPP::AutoSeededRandomPool prng;
		if (public_id == 0)
			public_identifier_ = (std::uint64_t)prng.GenerateWord32() << 32 | (std::uint64_t)prng.GenerateWord32();
		else
			public_identifier_ = 123;
		std::cout << "Public ID: " << public_identifier_ << std::endl;

		CryptoPP::RSA::PrivateKey private_key_new;
		private_key_new.GenerateRandomWithKeySize(prng, 4096);
		CryptoPP::RSA::PublicKey public_key_new(private_key_new);

		networking_->ip_map_.my_ip.add_rsa_key(std::move(public_key_new));
		networking_->ip_map_.private_rsa = {std::move(private_key_new)};
		qobject_connect_peer();
		peer_init();
	}

	explicit Peer(np2ps::Peer& peer_serialized) : 
		public_identifier_(peer_serialized.public_identifier()),
		name_(peer_serialized.name()),
		networking_(std::make_shared<Networking>(peer_serialized.ip_map()))
	{
		for (const np2ps::LocalSerializedNewspaperEntry& ne : peer_serialized.news()) {
			news_.emplace(ne.entry().news_id(), NewspaperEntry(ne));
			if (ne.entry().news_id() == public_identifier_) {
				newspaper_id_ = ne.entry().news_id();
				newspaper_name_ = ne.entry().news_name();
			}
		}
		qobject_connect_peer();
		peer_init();
	}

	void qobject_connect_peer() {
		QObject::connect(this, &Peer::new_symmetric_key,
						 &(*networking_), &Networking::decrypt_encrypted_messages);

		QObject::connect(&(*networking_), &Networking::new_message_received,
						 this, &Peer::handle_message);

		QObject::connect(this, &Peer::symmetric_key_exchanged, 
						 &(*networking_), &Networking::symmetric_exchanged);

		QObject::connect(networking_->get_stun_client().get(), &StunClient::confirmed_newspaper,
							this, &Peer::newspaper_confirm);

		QObject::connect(networking_.get(), &Networking::newspaper_identified, this, &Peer::newspaper_identified);
	}

	/**
	 * @brief To initialize all the fields of Peer class that
	 * are common to both constructors. QObject::connect has
	 * its own method like this one.
	 * 
	 */
	void peer_init() {
		user_map.emplace(public_identifier_, PeerInfo(public_identifier_, 255));
		networking_->init_sender_receiver(&news_);
		networking_->set_peer_public_id(public_identifier_);
		networking_->set_maps(&user_map, &news_, &readers_, &journalists_);
	}

	void peer_init(const std::string& peer_name) {
		set_name(peer_name);
		peer_init();
	}

	void peer_init(const std::string& peer_name, const std::string& peer_newspaper_name) {
		init_newspaper(peer_newspaper_name);
		peer_init(peer_name);
	}

	void load_ip_authorities(pk_t newspaper_key); //to load the IPs of authorities
	void enroll_new_article(Article article, bool header_only); //add new article to list of category -> article
	void add_new_newspaper(pk_t newspaper_key, const my_string& newspaper_name, const std::string& newspaper_ip);
	size_t list_all_articles_from_news(article_container& articles, const std::set<category_t>& categories);
	size_t list_all_articles_from_news(article_container& articles);
	size_t list_all_articles_from_news(article_container& articles, pk_t newspaper_id);
	size_t list_all_articles_by_me(article_container& articles, const std::set<category_t>& categories, pk_t news_id = 0);
	size_t list_all_articles_by_me(article_container& articles, pk_t news_id = 0);
	article_optional find_article(hash_t article_hash);
	optional_author_peers find_article_in_article_categories_db(hash_t article_hash);
	optional_author_peers find_article_in_article_categories_db(hash_t article_hash, category_container categories);

	NewspaperEntry& get_news(pk_t news_id);

	void handle_requests(unique_ptr_message message);
	void handle_responses(unique_ptr_message message);
	void handle_one_way(unique_ptr_message message);
	void handle_error(unique_ptr_message message);

	void handle_article_all_request(unique_ptr_message message);
	void handle_article_header_request(unique_ptr_message message);
	void handle_article_list_request(unique_ptr_message message);
	void handle_article_data_update_request(unique_ptr_message message);
	void handle_update_margin_request(unique_ptr_message message);
	void handle_credentials_request(unique_ptr_message message);

	void handle_article_all_response(unique_ptr_message message);
	void handle_article_list_response(unique_ptr_message message);
	void handle_credentials_response(unique_ptr_message message);
	void handle_public_key_response(unique_ptr_message message);
	void handle_symmetric_key_response(unique_ptr_message message);
	void handle_update_margin_response(unique_ptr_message message);

	void handle_article_solicitation_one_way(unique_ptr_message message);
	void handle_symmetric_key_one_way(unique_ptr_message message);
	void handle_public_key_one_way(unique_ptr_message message);

	void handle_article_solicitation_error(unique_ptr_message message);
	void handle_article_header_error(unique_ptr_message message);
	void handle_article_list_error(unique_ptr_message message);

	void generate_article_all_message(pk_t destination, hash_t article_hash);
	void generate_article_header_message(pk_t destination, hash_t article_hash);
	void send_stun_binding_request();
	void removed_external_article(hash_t article, pk_t to);

	pk_t check_destination_valid(pk_t destination, pk_t newspaper);

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
	 * @param newspaper_id ID of newspaper which article list we want.
	 */
	void generate_article_list_message(pk_t newspaper_id) {
		getting_article_list.insert(newspaper_id);
		emit check_selected_item();

		auto& news = get_news_db();
		const user_container& news_friends = news[newspaper_id].get_friends();

		networking_->enroll_message_to_be_sent(
			MFW::ReqArticleListFactory(
				MFW::ArticleListFactory(
					public_identifier_,
					newspaper_id
				),
				newspaper_id,
				10,
				std::vector<my_string>()
			)
		);

		for (auto&& user : news_friends) {
			networking_->enroll_message_to_be_sent(
				MFW::ReqArticleListFactory(
					MFW::ArticleListFactory(
						public_identifier_,
						user
					),
					newspaper_id,
					10,
					std::vector<my_string>()
				)
			);	
		}
	}
	
	/**
	 * @brief Generator for article list message, with support for categories.
	 * 
	 * @tparam Container Category container type.
	 * @param destination ID of newspaper which article list we want.
	 * @param categories Categories we want.
	 */
	template<typename Container>
	void generate_article_list_message(pk_t destination, const Container& categories) {
		networking_->enroll_message_to_be_sent(
			MFW::ReqArticleListFactory<Container>(
				MFW::ArticleListFactory(
					public_identifier_,
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
		article_optional article = find_article(article_hash);
		article.value()->add_margin(public_identifier_, Margin(type, content, public_identifier_));
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
						public_identifier_,
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
						public_identifier_,
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
						public_identifier_,
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

	void create_margin_request(pk_t, hash_t);

	/**
	 * @brief Public identifier getter.
	 * 
	 * @return pk_t Public identifier.
	 */
	pk_t get_public_key() {
		return public_identifier_;
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
		std::cout << "public_identifier_ " << public_identifier_ << std::endl;
		std::cout << "name_ " << name_ << std::endl;
		std::cout << "newspaper_id_ " << newspaper_id_ << std::endl;
		std::cout << "newspaper_name_ " << newspaper_name_ << std::endl;
		std::cout << "news_ count: " << news_.size() << std::endl;
		std::cout << "Reader count: " << readers_.size() << std::endl;
		std::cout << "User count: " << user_map.size() << std::endl;
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
	 * @brief News name getter. 
	 * 
	 * @return std::string News name.
	 */
	std::string get_my_news_name() {
		return newspaper_name_;
	}

	void networking_init_sender_receiver() {
		networking_->init_sender_receiver(&news_);
	}

	void stun_allocate() {
		//networking_->get_stun_client()->allocate_request();
	}

	bool remove_article(hash_t hash);
	bool remove_article(hash_t hash, pk_t& newspaper_id);
	void identify_newspaper(QHostAddress address, const std::string& newspaper_name);
	void upload_external_article(Article a);
	void add_journalist(pk_t j) {
		journalists_.insert(j);
	}
	void remove_journalist(pk_t j) {
		journalists_.erase(j);
	}
	Networking* get_networking() {
		return networking_.get();
	}

	std::unordered_set<hash_t>& get_downloading_articles() {
		return downloading_articles;
	}

	std::unordered_set<hash_t>& get_getting_article_list() {
		return getting_article_list;
	}

	/**
	 * @brief Removes reader for given article.
	 */
	void remove_reader(hash_t article, pk_t reader) {
		auto [ait, eit] = readers_.equal_range(article);
		for (; ait != eit; ait++) {
			if (ait->second->peer_key == reader) {
				readers_.erase(ait);
				break;
			}
		}
	}

	void serialize(np2ps::Peer* p) {
		p->set_name(name_);
		p->set_public_identifier(public_identifier_);
		auto ip_map_gpb = p->mutable_ip_map();
		networking_->ip_map_.serialize_ip_map(ip_map_gpb);
		for (auto&& newspapers : news_) {
			auto ne_gpb = p->add_news();
			NewspaperEntry& ne = newspapers.second;
			ne.local_serialize_entry(ne_gpb);
		}
	}

public slots:
	void handle_message(unique_ptr_message message);

	void allocate_on_stun_server(pk_t target);

	void newspaper_confirm(pk_t pid);

	void newspaper_identified(pk_t newspaper_key, my_string newspaper_name, std::string newspaper_ip_domain);


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

	void check_selected_item();

	
private:
	//reader part
	pk_t public_identifier_; //public identifier of my peer
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
	user_multimap_container newspaper_all_readers; //TODO: delete //list of all readers of all articles
	category_multimap_container articles_categories_; //articles mapped by categories, maps to `newspaper_all_readers`
	my_string newspaper_name_; //my newspaper name
	pk_t newspaper_id_; //public identifier of my newspaper
	user_container authorities_; //list of authorities
	user_container journalists_; //list of journalists
	std::unordered_set<hash_t> downloading_articles;
	std::unordered_set<pk_t> getting_article_list;

	article_optional find_article_in_database(hash_t article_hash);
};

#endif //PROGRAM_PEER_H
