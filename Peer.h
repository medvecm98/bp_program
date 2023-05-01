#ifndef PROGRAM_PEER_H
#define PROGRAM_PEER_H

#include "CryptoUtils.hpp"
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
class Peer : public QObject
{

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
	Peer(pk_t public_id) : networking_(std::make_shared<Networking>(public_id))
	{
		CryptoPP::AutoSeededRandomPool prng;
		if (public_id == 0)
			public_identifier_ = (std::uint64_t)prng.GenerateWord32() << 32 | (std::uint64_t)prng.GenerateWord32();
		else
			public_identifier_ = public_id;
		std::cout << "Public ID: " << public_identifier_ << std::endl;

		auto key_pair = CryptoUtils::instance().generate_rsa_pair();

		networking_->ip_map_.my_ip().add_rsa_key(key_pair.first);
		networking_->ip_map_.private_rsa = {key_pair.second};
		qobject_connect_peer();
		peer_init();
	}

	explicit Peer(np2ps::Peer &peer_serialized) : public_identifier_(peer_serialized.public_identifier()),
												  name_(peer_serialized.name()),
												  networking_(std::make_shared<Networking>(peer_serialized.ip_map(), peer_serialized.public_identifier()))
	{
		for (const np2ps::LocalSerializedNewspaperEntry &ne : peer_serialized.news())
		{
			news_.emplace(ne.entry().news_id(), NewspaperEntry(ne, &networking_->disconnected_readers_lazy_remove));
			if (ne.entry().news_id() == public_identifier_)
			{
				newspaper_id_ = ne.entry().news_id();
				newspaper_name_ = ne.entry().news_name();
			}
		}
		qobject_connect_peer();
		peer_init();
	}

	void qobject_connect_peer()
	{
		QObject::connect(this, &Peer::new_symmetric_key,
						 &(*networking_), &Networking::decrypt_encrypted_messages);

		QObject::connect(&(*networking_), &Networking::new_message_received,
						 this, &Peer::handle_message);

		QObject::connect(this, &Peer::symmetric_key_exchanged,
						 &(*networking_), &Networking::symmetric_exchanged);

		QObject::connect(networking_->get_stun_client().get(), &StunClient::confirmed_newspaper,
						 this, &Peer::newspaper_confirm);

		QObject::connect(networking_->get_stun_client().get(), &StunClient::confirmed_newspaper_pk,
						 this, &Peer::newspaper_confirm_public_key);

		QObject::connect(networking_.get(), &Networking::newspaper_identified,
						 this, &Peer::newspaper_identified);
	}

	/**
	 * @brief To initialize all the fields of Peer class that
	 * are common to both constructors. QObject::connect has
	 * its own method like this one.
	 *
	 */
	void peer_init()
	{
		networking_->init_sender_receiver(&news_);
		networking_->set_peer_public_id(public_identifier_);
		networking_->set_maps(NULL, &news_, NULL, &journalists_);
		networking_->start_servers_with_first_ip();
	}

	void peer_init(const std::string &peer_name)
	{
		set_name(peer_name);
		peer_init();
	}

	void peer_init(const std::string &peer_name, const std::string &peer_newspaper_name)
	{
		init_newspaper(peer_newspaper_name);
		peer_init(peer_name);
	}

	void load_ip_authorities(pk_t newspaper_key);				// to load the IPs of authorities
	void enroll_new_article(Article article, bool header_only); // add new article to list of category -> article
	void add_new_newspaper(pk_t newspaper_key, const my_string &newspaper_name, const std::string &newspaper_ip, bool allocate_now = true);
	void add_new_newspaper(pk_t newspaper_key, const my_string &newspaper_name, pk_t sender);
	void add_new_newspaper(pk_t destination, pk_t news_id, my_string news_name);
	NewspaperEntry& add_new_newspaper(pk_t newspaper_key, const my_string &newspaper_name, QHostAddress &&newspaper_ip_domain, bool allocate_now = false);
	NewspaperEntry& add_new_newspaper(NewspaperEntry&& newspaper_entry, QHostAddress&& address, bool allocate_now = false);
	void add_new_newspaper_from_file(const std::string &path);
	void add_new_newspaper_pk(pk_t pid);
	size_t list_all_articles_from_news(article_container &articles, const std::set<category_t> &categories);
	size_t list_all_articles_from_news(article_container &articles);
	size_t list_all_articles_from_news(article_container &articles, pk_t newspaper_id, int count);
	size_t list_all_articles_from_news(article_container &articles, pk_t newspaper_id, int count, QDate date);
	size_t list_all_articles_by_me(article_container &articles, const std::set<category_t> &categories, pk_t news_id = 0);
	size_t list_all_articles_by_me(article_container &articles, pk_t news_id = 0);
	article_optional find_article(hash_t article_hash);
	optional_author_peers find_article_in_article_categories_db(hash_t article_hash);
	optional_author_peers find_article_in_article_categories_db(hash_t article_hash, category_container categories);

	news_database &get_news();
	NewspaperEntry &get_news(pk_t news_id);

	/* -------------------------- */
	/*  message context handlers  */
	/* -------------------------- */

	void handle_requests(shared_ptr_message message);
	void handle_responses(shared_ptr_message message);
	void handle_one_way(shared_ptr_message message);
	void handle_error(shared_ptr_message message);

	/* ----------------------- */
	/*  message type handlers  */
	/* ----------------------- */

	/* request */

	void handle_article_all_request(shared_ptr_message message);
	void handle_article_header_request(shared_ptr_message message);
	void handle_article_list_request(shared_ptr_message message);
	void handle_article_data_update_request(shared_ptr_message message);
	void handle_update_margin_request(shared_ptr_message message);
	void handle_credentials_request(shared_ptr_message message);
	void handle_newspaper_entry_request(shared_ptr_message message);
	void handle_newspaper_list_request(shared_ptr_message message);
	void handle_journalist_request(shared_ptr_message message);
	void handle_gossip_request(shared_ptr_message message);

	/* response */

	void handle_article_all_response(shared_ptr_message message);
	void handle_article_list_response(shared_ptr_message message);
	void handle_credentials_response(shared_ptr_message message);
	void handle_public_key_response(shared_ptr_message message);
	void handle_symmetric_key_response(shared_ptr_message message);
	void handle_update_margin_response(shared_ptr_message message);
	void handle_newspaper_entry_response(shared_ptr_message message);
	void handle_newspaper_list_response(shared_ptr_message message);
	void handle_journalist_response(shared_ptr_message message);
	void handle_gossip_response(shared_ptr_message message);

	/* one way */

	void handle_article_solicitation_one_way(shared_ptr_message message);
	void handle_symmetric_key_one_way(shared_ptr_message message);
	void handle_public_key_one_way(shared_ptr_message message);
	void handle_article_data_update_one_way(shared_ptr_message message);
	void handle_credentials_one_way(shared_ptr_message message);
	void handle_user_info_message_one_way(shared_ptr_message message);
	void handle_user_info_message_one_way_advert(shared_ptr_message message);
	void handle_user_info_message_one_way_request(shared_ptr_message message);
	void handle_user_info_message_one_way_response(shared_ptr_message message);
	void handle_new_journalist_one_way(shared_ptr_message message);
	void handle_gossip_one_way(shared_ptr_message message);

	/* error */

	void handle_article_solicitation_error(shared_ptr_message message);
	void handle_article_header_error(shared_ptr_message message);
	void handle_article_list_error(shared_ptr_message message);
	void handle_article_all_error(shared_ptr_message message);
	void handle_newspaper_entry_error(shared_ptr_message message);
	void handle_journalist_error(shared_ptr_message message);

	/* ------------ */
	/*  generators  */
	/* ------------ */

	void generate_article_all_message(pk_t destination, hash_t article_hash);
	void generate_article_header_message(pk_t destination, hash_t article_hash);
	void generate_article_list_message(pk_t newspaper_id);
	void generate_newspaper_entry_request(pk_t destination, pk_t newspaper_id);
	void generate_newspaper_list_request();
	void generate_newspaper_list_request(pk_t destination);
	void generate_successful_download_message(pk_t reader, pk_t recv_article_id);
	void generate_successful_download_message_all_readers(const user_container& readers, pk_t from, pk_t recv_article_id);
	void generate_new_journalist(pk_t pid);
	void generate_user_info_message(pk_t to);
	void generate_gossip_request(pk_t to);
	void generate_gossip_one_way(pk_t to);
	void generate_gossip_one_way();

	void inform_coworkers();

	void send_stun_binding_request();
	void removed_external_article(hash_t article, pk_t to);

	pk_t check_destination_valid(pk_t destination, pk_t newspaper);

	void set_name(const my_string &name);
	pk_t get_my_news_id();

	void set_my_ip(QString ip);

	void init_newspaper(my_string name);

	template <typename Container>
	void generate_article_list_message(pk_t destination, const Container &categories);

	void add_margin(hash_t article_hash, my_string type, my_string content);
	void add_margin(hash_t article_hash, margin_vector &vm);
	void update_margin(hash_t article_hash, unsigned int id, my_string type, my_string content);
	void remove_margin(hash_t article_hash, unsigned int id);

	void create_margin_request(pk_t, hash_t);

	pk_t get_public_key();
	my_string get_name();

	my_string name();

	void print_contents();

	news_database &get_news_db();
	std::string get_my_news_name();

	void networking_init_sender_receiver();

	void stun_allocate();
	bool remove_article(hash_t hash);
	bool remove_article(hash_t hash, pk_t &newspaper_id);
	void identify_newspaper(QHostAddress address, const std::string &newspaper_name);
	void upload_external_article(Article a);
	void add_journalist(pk_t j);
	void remove_journalist(pk_t j);
	Networking *get_networking();

	std::unordered_set<hash_t> &get_downloading_articles();

	std::unordered_set<hash_t> &get_getting_article_list();

	void remove_reader(hash_t article, pk_t reader);
	void serialize(np2ps::Peer *p);
	bool find_news(pk_t news_id);
	user_container &get_friends();
	bool add_friend(pk_t id, const std::string &ip);
	bool add_friend(pk_t id, const QString &ip);
	bool add_friend(pk_t id, QHostAddress ip);

	void allocate_next_newspaper();
	NewspaperEntry& get_my_newspaper();
	void remove_news(pk_t to_remove);

public slots:
	void handle_message(shared_ptr_message message);

	void allocate_on_stun_server(pk_t target);

	void newspaper_confirm(pk_t pid);

	void newspaper_confirm_public_key(pk_t pid, rsa_public_optional public_key);

	void newspaper_identified(pk_t newspaper_key, my_string newspaper_name, std::string newspaper_ip_domain);

	void slot_add_new_newspaper_from_file(QString path)
	{
		add_new_newspaper_from_file(path.toStdString());
	}

	void slot_add_new_newspaper_pk(QString id_qstring)
	{
		bool ok;
		pk_t id = id_qstring.toULongLong(&ok);
		if (ok)
		{
			add_new_newspaper_pk(id);
		}
		else
		{
			throw new other_error("Invalid form od ID inserted.");
		}
	}

signals:
	void newspaper_list_received();

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

	void checked_display_article(pk_t news_id, hash_t article);

private:
	PeerConfig config;

	// reader part
	pk_t public_identifier_;				 // public identifier of my peer
	my_string name_;						 // name of my peer
	std::shared_ptr<Networking> networking_; // networking, for handling sending and receiving
	news_database news_;					 // list of all downloaded articles, mapped by their Newspapers
	user_container friends_;				 // friends, sharing their newspaper entries with you

	std::unordered_multimap<hash_t, Margin> margins_added_;					   // multimap of Article -> Margins, that this peer added, or requested to add
	std::unordered_map<pk_t, Article> article_headers_only;					   // only for article headers, so it won't interfere with regular ones
	std::unordered_map<pk_t, NewspaperEntry> newspapers_awaiting_confirmation; // newspaper that we want to add, but that haven't yet confirmed their existence

	// journalist part
	//  reader_database readers_; //list of article readers
	user_container journalist_of;

	// chief editor
	user_multimap_container newspaper_all_readers;	  // TODO: delete //list of all readers of all articles
	category_multimap_container articles_categories_; // articles mapped by categories, maps to `newspaper_all_readers`
	my_string newspaper_name_;						  // my newspaper name
	pk_t newspaper_id_;								  // public identifier of my newspaper
	user_container authorities_;					  // list of authorities
	user_container journalists_;					  // list of journalists
	std::unordered_set<hash_t> downloading_articles;
	std::unordered_set<pk_t> getting_article_list;

	article_optional find_article_in_database(hash_t article_hash);
};

#endif // PROGRAM_PEER_H
