#ifndef PROGRAM_PEER_H
#define PROGRAM_PEER_H

#include "CryptoUtils.hpp"
#include "Networking.h"
#include "NewspaperEntry.h"
#include <unordered_map>
#include "Message.h"
#include <functional>
#include <QHostInfo>
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
	Peer(pk_t public_id)
	{
		CryptoPP::AutoSeededRandomPool prng;
		if (public_id == 0)
			public_identifier_ = (std::uint64_t)prng.GenerateWord32() << 32 | (std::uint64_t)prng.GenerateWord32();
		else
			public_identifier_ = public_id;
		networking_ = std::make_shared<Networking>(public_identifier_);
		std::cout << "Public ID: " << public_identifier_ << std::endl;

		auto key_pair = CryptoUtils::instance().generate_rsa_pair();

		networking_->ip_map_.my_ip().add_rsa_key(key_pair.first);
		networking_->ip_map_.private_rsa = {key_pair.second};

		init_timers();
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

		deserialize_config(peer_serialized.config());
		init_timers();
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

		QObject::connect(&(*networking_), &Networking::got_article_all_rejection,
						 this, &Peer::article_all_send_more_message);

		QObject::connect(networking_.get(), &Networking::newspaper_identified,
						 this, &Peer::newspaper_identified);
		QObject::connect(
			auto_update_timer, &QTimer::timeout,
			this, &Peer::generate_article_list_message_all_news);
		QObject::connect(
			waiting_messages_timer, &QTimer::timeout,
			networking_.get(), &Networking::resend_np2ps_messages_waiting_for_peer_online);
		QObject::connect(
			gossip_timer, &QTimer::timeout,
			this, &Peer::generate_gossip_one_way_all);
		QObject::connect(
			gossip_timer, &QTimer::timeout,
			this, &Peer::inform_coworkers);
		QObject::connect(
			clear_articles_timer, &QTimer::timeout,
			this, &Peer::clear_abundant_articles);
		QObject::connect(
			clear_articles_timer, &QTimer::timeout,
			networking_.get(), &Networking::clean_long_term_np2ps_messages);
		QObject::connect(
			stun_allocate_timer, &QTimer::timeout,
			this, &Peer::check_stun_servers
		);
		auto_update_timer->start(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::minutes(20)));
		waiting_messages_timer->start(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::minutes(10)));
		gossip_timer->start(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::minutes(5)));
		clear_articles_timer->start(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::hours(1)));
		stun_allocate_timer->start(
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::minutes(5)));
	}

	void init_timers()
	{
		if (!auto_update_timer)
		{
			auto_update_timer = new QTimer(this);
		}
		if (!waiting_messages_timer)
		{
			waiting_messages_timer = new QTimer(this);
		}
		if (!gossip_timer)
		{
			gossip_timer = new QTimer(this);
		}
		if (!clear_articles_timer)
		{
			clear_articles_timer = new QTimer(this);
		}
		if (!stun_allocate_timer)
		{
			stun_allocate_timer = new QTimer(this);
		}
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

	void enroll_new_article(Article article, bool header_only); // add new article to list of category -> article
	void add_new_newspaper(pk_t newspaper_pid, const my_string &newspaper_name, const std::string &newspaper_ip, bool allocate_now = true);
	void add_new_newspaper(pk_t newspaper_pid, const my_string &newspaper_name, pk_t sender);
	void add_new_newspaper(pk_t destination, pk_t news_id, my_string news_name);
	NewspaperEntry &add_new_newspaper(pk_t newspaper_pid, const my_string &newspaper_name, QHostAddress &&newspaper_ip_domain, port_t np2ps_port, port_t stun_port, bool allocate_now = false);
	NewspaperEntry &add_new_newspaper(NewspaperEntry &&newspaper_entry, QHostAddress &&address, port_t np2ps_port, port_t stun_port, bool allocate_now = false);
	void add_new_newspaper_from_file(const std::string &path);
	void add_new_newspaper_pk(pk_t pid);
	size_t list_all_articles_by_me(article_container &articles, const std::set<category_t> &categories, pk_t news_id = 0);
	size_t list_all_articles_by_me(article_container &articles, pk_t news_id = 0);
	article_optional find_article(hash_t article_hash);

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

	void handle_article_all_one_way(shared_ptr_message message);
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
	void handle_ping_one_way(shared_ptr_message message);

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
	void generate_article_list_message(pk_t newspaper_id, bool ignore_timestamp = false);
	void generate_article_list_message_all_news();
	void generate_newspaper_entry_request(pk_t destination, pk_t newspaper_id);
	void generate_newspaper_list_request();
	void generate_newspaper_list_request_connected();
	void generate_newspaper_list_request(pk_t destination);
	void generate_successful_download_message(pk_t reader, pk_t recv_article_id);
	void generate_successful_download_message_all_readers(const user_container &readers, pk_t from, pk_t recv_article_id);
	void generate_new_journalist(pk_t pid);
	void generate_journalist_request(pk_t news_id);
	void generate_user_info_message(pk_t to);
	void generate_gossip_request(pk_t to);
	void generate_gossip_one_way(pk_t to);
	void generate_gossip_one_way_all();
	void generate_news_refresh();
	void generate_ping_one_way(pk_t to);
	void generate_article_removed_message(pk_t news, pk_t author, hash_t article_id);

	void article_all_send(Article &article);
	void article_all_send_more(Article &article);

	void inform_coworkers();
	void ping_direct_peers();

	void send_stun_binding_request();
	void removed_external_article(hash_t article, pk_t to);

	pk_t check_destination_valid(pk_t destination, pk_t newspaper);

	void set_name(const my_string &name);
	pk_t get_my_news_id();

	void set_my_ip(QString ip);

	void init_newspaper(my_string name);

	void generate_article_list_message(pk_t destination, const std::vector<std::string> &categories);

	void add_margin(hash_t article_hash, my_string type, my_string content);
	void add_margin(hash_t article_hash, margin_vector &vm);
	void update_margin(hash_t article_hash, unsigned int id, my_string type, my_string content);
	void remove_margin(hash_t article_hash, unsigned int id);

	void create_margin_request(pk_t, hash_t);

	pk_t get_public_id();
	my_string get_name();

	my_string &name();

	void print_contents();

	news_database &get_news_db();
	std::string get_my_news_name();

	void networking_init_sender_receiver();

	void stun_allocate();
	bool remove_article(hash_t hash);
	bool remove_article(hash_t hash, pk_t &newspaper_id);
	void identify_newspaper(QString address, const std::string &newspaper_name);
	void identify_newspaper(QHostAddress address, port_t np2ps_port, port_t stun_port, const std::string &newspaper_name);
	void upload_external_article(Article a, Article* ancestor = NULL);
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
	NewspaperEntry &get_my_newspaper();
	void remove_news(pk_t to_remove);

	void serialize_config(np2ps::PeerConfig *serialized_peer);
	void deserialize_config(const np2ps::PeerConfig &serialized_peer)
	{
		config.gossip_randoms = serialized_peer.gossip_randoms();
		config.list_size_default = serialized_peer.list_size_default();
		config.list_size_first = serialized_peer.list_size_first();
	}

	user_container &get_journalist_of()
	{
		return journalist_of_;
	}

	std::map<std::string, pk_t> &get_pending_journalists();

	void load_news_from_file(std::string path);
	void save_news_to_file(std::string path, pk_t news_id, QHostAddress address);
	void clear_abundant_articles();

	ArticleListSort get_article_list_sort_config()
	{
		return config.sort_type;
	}

	void set_article_list_sort_config(ArticleListSort sort_type)
	{
		config.sort_type = sort_type;
	}

	void store_newspaper_potential(pk_t news_id, news_potential_tuple entry);

	void clear_newspaper_potential();

	news_potential_db& get_newspaper_potential();

	void check_stun_servers();

public slots:
	void article_all_send_more_message(shared_ptr_message message);

	void update_article(pk_t news_id, hash_t article_hash, std::string new_text);

	void handle_message(shared_ptr_message message);

	void allocate_on_stun_server(pk_t target);

	void newspaper_confirm(pk_t pid);

	void newspaper_confirm_public_key(pk_t pid, rsa_public_optional public_key);

	void newspaper_identified(pk_t newspaper_pid, my_string newspaper_name, std::string newspaper_ip_domain, port_t np2ps_port, port_t stun_port);

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

	/* slots for config setters */

	void slot_set_config_peer_gossips(int value);
	void slot_set_config_peer_article_list_first(int value);
	void slot_set_config_peer_article_list_default(int value);
	void slot_set_config_peer_article_list_first_percent(int value);
	void slot_set_config_peer_article_list_default_percent(int value);

	void slot_set_config_news_no_read_articles(pk_t news_id, int value);
	void slot_set_config_news_no_unread_articles(pk_t news_id, int value);

	/* slots for config getters */

	int slot_get_config_peer_gossips();
	int slot_get_config_peer_article_list_first();
	int slot_get_config_peer_article_list_default();
	int slot_get_config_peer_article_list_first_percent();
	int slot_get_config_peer_article_list_default_percent();
	int slot_get_config_peer_article_list_first_total();
	int slot_get_config_peer_article_list_default_total();

	int slot_get_config_news_no_read_articles(pk_t news_id);
	int slot_get_config_news_no_unread_articles(pk_t news_id);

	void slot_dns_result(const QHostInfo &host);

	void slot_newspaper_from_list_added(pk_t news_id);

signals:
	void news_from_file_added();

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

	void signal_article_updated();

	void signal_new_journalist_request(pk_t pid, std::string name);
	
	void signal_journalism_approved(pk_t pid);

	void signal_newspaper_updated();

private:
	PeerConfig config;

	// reader part
	pk_t public_identifier_;	// public identifier of my peer
	my_string name_;			// name of my peer
	networking_ptr networking_; // networking, for handling sending and receiving
	news_database news_;		// list of all downloaded articles, mapped by their Newspapers
	user_container friends_;	// friends, sharing their newspaper entries with you

	std::unordered_multimap<hash_t, Margin> margins_added_;						// multimap of Article -> Margins, that this peer added, or requested to add
	std::unordered_map<pk_t, NewspaperEntry> newspapers_awaiting_confirmation_; // newspaper that we want to add, but that haven't yet confirmed their existence

	// journalist part
	//  reader_database readers_; //list of article readers
	user_container journalist_of_;

	// chief editor
	my_string newspaper_name_;	 // my newspaper name
	pk_t newspaper_id_;			 // public identifier of my newspaper
	user_container journalists_; // list of journalists
	std::unordered_set<hash_t> downloading_articles;
	std::unordered_set<pk_t> getting_article_list;
	std::map<std::string, pk_t> pending_journalist_requests;
	std::map<QString, std::string> waiting_dns_newspapers;
	news_potential_db news_potential;

	QTimer *auto_update_timer = NULL;
	QTimer *waiting_messages_timer = NULL;
	QTimer *gossip_timer = NULL;
	QTimer *clear_articles_timer = NULL;
	QTimer *stun_allocate_timer = NULL;

	article_optional find_article_in_database(hash_t article_hash);
};

#endif // PROGRAM_PEER_H
