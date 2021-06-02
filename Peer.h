#ifndef PROGRAM_PEER_H
#define PROGRAM_PEER_H

#include "GlobalUsing.h"
#include "Networking.h"
#include "NewspaperEntry.h"
#include <unordered_map>

using news_database = std::unordered_map<pk_t, NewspaperEntry>;

class Peer {
public:
	int load_ip_authorities(); //to load the IPs of authorities
	void enroll_new_article(Article article); //add new article to list of cat. -> article
	bool request_margin(hash_t article, Margin margin); //request margin addition
	void add_new_newspaper(pk_t newspaper_key, const std::string& newspaper_ip) {
		//add news ip to map
		networking_.ip_map_.add_to_map(newspaper_key, IpWrapper(newspaper_ip));
		//request public key of chief editor
		auto ip_msg = IpAddressMessage(public_key_, newspaper_key);
		networking_.enroll_message_to_be_sent(std::move(ip_msg));
	}
private:
	//reader part
	pk_t public_key_;
	my_string name_;
	Networking networking_;
	news_database news_; //all downloaded articles

	//reporter part
	std::unordered_multimap<hash_t, pk_t> readers_; //list of article readers

	//chief editor
	category_multimap_container articles_categories_;
	my_string newspaper_name_;
	pk_t newspaper_id_;
	user_container authorities_;
	user_container journalists_;

	//other
	std::map<hash_t, Margin> margins_added_; //map of Article <-> Margin, that this peer added, or requested to add
};

#endif //PROGRAM_PEER_H