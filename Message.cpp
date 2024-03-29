#include "Message.h"

using MFW = MessageFactoryWrapper;

shared_ptr_message MFW::ArticleDataChangeFactory(pk_t from, pk_t to, hash_t article_hash, bool download) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::ARTICLE_DATA_UPDATE);
	msg->mutable_article_data_update()->set_article_pk(article_hash);

	if (download) {
		msg->mutable_article_data_update()->set_article_action(np2ps::DOWNLOAD);
	}
	else {
		msg->mutable_article_data_update()->set_article_action(np2ps::REMOVAL);
	}

	return std::move(msg);
}

shared_ptr_message MFW::ArticleDownloadFactory(pk_t from, pk_t to, article_ptr article) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::ARTICLE_ALL);

	auto article_gpb = msg->mutable_article_all()->mutable_header();
	article->network_serialize_article(article_gpb);

	msg->mutable_article_all()->set_article_hash(article->main_hash());

	return std::move(msg);
}

shared_ptr_message MFW::ArticleHeaderFactory(pk_t from, pk_t to, hash_t article_hash) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::ARTICLE_HEADER);

	msg->mutable_article_header()->set_article_hash(article_hash);

	return std::move(msg);
}

shared_ptr_message MFW::ArticleListFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::ARTICLE_LIST);

	return std::move(msg);
}

shared_ptr_message MFW::UserIsMemberFactory(pk_t from, pk_t to, pk_t user_pk) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::USER_IS_MEMBER);

	msg->mutable_user_is_member()->set_user_pk(user_pk);

	return std::move(msg);
}

shared_ptr_message MFW::update_margin_factory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margins) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::UPDATE_MARGIN);

	msg->mutable_update_margin()->set_article_pk(article_hash);

	if (margins.size() > 0) {
		for (auto&& margin : margins) {
			auto marg_ptr = msg->mutable_update_margin()->mutable_margin()->add_margins();
			marg_ptr->set_type(margin.type);
			marg_ptr->set_content(margin.content);
		}
	}

	return std::move(msg);
}

shared_ptr_message MFW::UpdateMarginFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::ADD);

	return std::move(msg);
}

shared_ptr_message MFW::UpdateMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	return std::move(msg);
}

shared_ptr_message MFW::UpdateMarginRemoveFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::REMOVE);

	return std::move(msg);
}

shared_ptr_message MFW::UpdateMarginUpdateFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::UPDATE);

	return std::move(msg);
}

shared_ptr_message MFW::UpdateArticleFactory(pk_t from, pk_t to, hash_t article_hash) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::UPDATE_ARTICLE);

	msg->mutable_update_article()->set_article_pk(article_hash);

	return std::move(msg);
}

shared_ptr_message MFW::CredentialsFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::CREDENTIALS);

	return std::move(msg);
}

/* Requests: */
shared_ptr_message MFW::ReqUserIsMemberFactory(shared_ptr_message&& msg, level_t level) { 
	msg->mutable_user_is_member()->set_level(level);
	msg->set_msg_ctx(np2ps::REQUEST);
	return std::move(msg);
}

shared_ptr_message MFW::ReqCredentialsFactory(shared_ptr_message&& msg, 
	bool req_ip4, bool req_ip6,
	bool req_public_key, bool req_eax_key,
	std::size_t target,
	string_optional ip4, string_optional ip6,
	rsa_public_optional public_key, rsa_private_optional private_key,
	eax_optional eax_key)
{
	msg->set_msg_ctx(np2ps::REQUEST);
	msg->mutable_credentials()->set_target(target);

	msg->mutable_credentials()->set_req_ipv4(req_ip4);
	msg->mutable_credentials()->set_req_ipv6(req_ip6);
	msg->mutable_credentials()->set_req_rsa_public_key(req_public_key);
	msg->mutable_credentials()->set_req_eax_key(req_eax_key);

	if (ip4.has_value()) {
		msg->mutable_credentials()->set_ipv4(ip4.value());
	}

	if (ip6.has_value()) {
		msg->mutable_credentials()->set_ipv6(ip6.value());
	}

	if (public_key.has_value()) {
		msg->mutable_credentials()->mutable_rsa_public_key()->set_key(
			CryptoUtils::instance().rsa_to_hex(
				public_key.value()
			)
		);
	}

	if (eax_key.has_value() 
		&& public_key.has_value()
		&& private_key.has_value())
	{
		std::string signature = CryptoUtils::instance().sign_key(
			eax_key.value(),
			private_key
		);
		std::string encrypted_key = CryptoUtils::instance().encrypt_key(
			eax_key.value(),
			public_key
		);
		msg->mutable_credentials()->mutable_eax_key()->set_key(encrypted_key);
		msg->mutable_credentials()->mutable_eax_key()->set_signature(
			signature
		);
	}

	return std::move(msg);
}

shared_ptr_message MFW::OneWayCredentialsFactory(
	shared_ptr_message&& msg,
	rsa_public_optional public_key,
	rsa_private_optional private_key,
	eax_optional eax_key,
	int method
) {
	msg->set_msg_ctx(np2ps::ONE_WAY);

	if (public_key.has_value()) {
		msg->mutable_credentials()->mutable_rsa_public_key()->set_key(
			CryptoUtils::instance().rsa_to_hex(
				public_key.value()
			)
		);
	}

	if (eax_key.has_value() 
		&& public_key.has_value()
		&& private_key.has_value())
	{
		std::string signature = CryptoUtils::instance().sign_key(
			eax_key.value(),
			private_key
		);
		std::string encrypted_key = CryptoUtils::instance().encrypt_key(
			eax_key.value(),
			public_key
		);
		msg->mutable_credentials()->mutable_eax_key()->set_key(encrypted_key);
		msg->mutable_credentials()->mutable_eax_key()->set_signature(signature);

	}
	msg->mutable_credentials()->set_method(method);

	return std::move(msg);
}

shared_ptr_message MFW::RespArticleDownloadFactory(shared_ptr_message&& msg, article_ptr article_header, std::string&& article) { 
	auto header_ptr = msg->mutable_article_all()->mutable_header();
	msg->set_msg_ctx(np2ps::RESPONSE);
	article_header->network_serialize_article(header_ptr);

	msg->mutable_article_all()->set_article_actual(article);


	return std::move(msg);
}

shared_ptr_message MFW::RespArticleHeaderFactory(shared_ptr_message&& msg, article_ptr article_header) {
	auto header_ptr = msg->mutable_article_header()->mutable_article();
	article_header->network_serialize_article(header_ptr);
	msg->set_msg_ctx(np2ps::RESPONSE);

	return std::move(msg);
}

shared_ptr_message MFW::ReqArticleHeaderFactory(shared_ptr_message&& msg, Article* article_header) {
	auto header_ptr = msg->mutable_article_header()->mutable_article();
	article_header->network_serialize_article(header_ptr);
	msg->set_msg_ctx(np2ps::REQUEST);

	return std::move(msg);
}

shared_ptr_message MFW::RespArticleListFactory(
	shared_ptr_message&& msg,
	article_container& articles,
	pk_t news_id,
	article_container& articles_readers_only,
	bool first_in_batch)
{
	for(auto&& article : articles) {
		auto a = msg->mutable_article_list()->add_response();
		article->network_serialize_article(a);
	}
	for (auto&& article_only_readers : articles_readers_only) {
			auto a = msg->mutable_article_list()->add_response_readers();
			article_only_readers->network_serialize_readers(a);
	}
	msg->mutable_article_list()->set_first_in_batch(first_in_batch);
	msg->mutable_article_list()->set_newspaper_id(news_id);
	msg->set_msg_ctx(np2ps::RESPONSE);
	return std::move(msg);
}

shared_ptr_message MFW::RespUserIsMemberFactory(shared_ptr_message&& msg, bool is_member, level_t req_level) { 
	msg->mutable_user_is_member()->set_is_member(is_member);
	msg->mutable_user_is_member()->set_level(req_level);
	msg->set_msg_ctx(np2ps::RESPONSE);
	return std::move(msg);
}

shared_ptr_message MFW::RespCredentialsFactory(
	shared_ptr_message&& msg,
	QString ip4,
	QString ip6, 
	rsa_public_optional public_key,
	eax_optional eax_key,
	CredentialsPayload payload
) {
		msg->set_msg_ctx(np2ps::RESPONSE);
		if (!ip4.isEmpty()) {
			msg->mutable_credentials()->set_ipv4(ip4.toStdString());
			msg->mutable_credentials()->set_req_ipv4(true);
		}
		else
			msg->mutable_credentials()->set_req_ipv4(false);

		if (!ip6.isEmpty()) {
			msg->mutable_credentials()->set_req_ipv6(true);
			msg->mutable_credentials()->set_ipv6(ip6.toStdString());
		}
		else 
			msg->mutable_credentials()->set_req_ipv6(false);

		if (public_key.has_value()) {
			msg->mutable_credentials()->mutable_rsa_public_key()->set_key(
				CryptoUtils::instance().rsa_to_hex(
					public_key.value()
				)
			);
			msg->mutable_credentials()->set_req_rsa_public_key(true);
		}
		else
			msg->mutable_credentials()->set_req_rsa_public_key(false);

		if (eax_key.has_value()) {
			msg->mutable_credentials()->mutable_eax_key()->set_signature(
				CryptoUtils::instance().sign_key(
					eax_key.value(),
					payload.my_private_key_for_signing
				)
			);
			msg->mutable_credentials()->mutable_eax_key()->set_key(
				CryptoUtils::instance().encrypt_key(
					eax_key.value(),
					payload.their_public_key_for_encrypting
				)
			);
			msg->mutable_credentials()->set_req_eax_key(true);
		}
		else
			msg->mutable_credentials()->set_req_eax_key(false);

		return std::move(msg);
	}

shared_ptr_message MFW::PublicKeyFactory(pk_t from, pk_t to, CryptoPP::RSA::PublicKey& key) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::PUBLIC_KEY);

	std::string key_str;

	CryptoPP::StringSink rsa_pub_sink(key_str);
	key.DEREncode(rsa_pub_sink);

	msg->mutable_public_key()->set_key(key_str);
	return std::move(msg);
}

shared_ptr_message MFW::UpdateSeqNumber(shared_ptr_message&& msg, unsigned int seq) {
	msg->set_seq(seq);
	return std::move(msg);
}

shared_ptr_message MFW::ErrorArticleListFactory(shared_ptr_message&& msg, pk_t newspaper_id) {
	MFW::SetMessageContextError(msg);
	msg->mutable_article_list()->set_newspaper_id(newspaper_id);
	return std::move(msg);
}

shared_ptr_message MFW::SetMessageContextRequest(shared_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::REQUEST);
	return std::move(msg);
}

shared_ptr_message MFW::SetMessageContextResponse(shared_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::RESPONSE);
	return std::move(msg);
}

shared_ptr_message MFW::SetMessageContextOneWay(shared_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::ONE_WAY);
	return std::move(msg);
}

shared_ptr_message MFW::SetMessageContextError(shared_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::ERROR);
	return std::move(msg);
}

void MFW::SetMessageContextError(shared_ptr_message& msg) {
	msg->set_msg_ctx(np2ps::ERROR);
}

shared_ptr_message MFW::RespNewspaperEntryFactory(shared_ptr_message&& msg, NewspaperEntry& news, IpMap& news_map) {
	news.network_serialize_entry(msg->mutable_newspaper_entry(), news_map, news.get_id());
	msg->set_msg_ctx(np2ps::RESPONSE);
	return msg;
}

shared_ptr_message MFW::NewspaperEntryFactory(
	pk_t from, pk_t to, pk_t newspaper_id, const std::string& name
) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::NEWSPAPER_ENTRY);

	msg->mutable_newspaper_entry()->mutable_entry()->set_news_id(newspaper_id);
	return msg;
}

shared_ptr_message MFW::NewspaperListFactory(pk_t from, pk_t to, std::int16_t article_count) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::NEWSPAPER_LIST);
	msg->mutable_newspaper_list()->set_article_count(article_count);

	return msg;
}

shared_ptr_message MFW::RespNewspaperListFactory(shared_ptr_message&& msg, const news_database& news_db, IpMap& networking_map, std::int16_t article_count_) {
	msg->set_msg_ctx(np2ps::RESPONSE);

	for (auto&& [nid, news] : news_db) {
		np2ps::NetworkSerializedNewspaperEntry* gpb_news = msg->mutable_newspaper_list()->add_news();
		news.network_serialize_entry(gpb_news, networking_map, nid, article_count_);
	}

	return msg;
}

shared_ptr_message MFW::ErrorArticleDownloadFactory(shared_ptr_message&& msg, article_ptr article_header) {
	auto header_ptr = msg->mutable_article_all()->mutable_header();
	msg->set_msg_ctx(np2ps::ERROR);
	article_header->network_serialize_article(header_ptr);

	return std::move(msg);
}

shared_ptr_message MFW::ErrorArticleDownloadFactory(shared_ptr_message&& msg, hash_t article_hash) {
	msg->set_msg_ctx(np2ps::ERROR);
	msg->mutable_article_all()->set_article_hash(article_hash);

	return std::move(msg);
}

shared_ptr_message MFW::JournalistFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::JOURNALIST);

	return msg;
}

shared_ptr_message MFW::RespJournalistFactory(shared_ptr_message&& msg, rsa_private_optional newspaper_private_key, NewspaperEntry& news, IpMap& map) {
	msg->set_msg_ctx(np2ps::RESPONSE);

	if (newspaper_private_key.has_value()) {
		auto hex_private = CryptoUtils::instance().private_to_hex(newspaper_private_key.value());
		msg->mutable_journalist()->set_private_key(hex_private);
	}

	news.network_serialize_entry(
		msg->mutable_journalist()->mutable_entry(),
		map,
		news.get_id()
	);

	for (auto&& journalist : news.get_journalists()) {
		std::cout << "Sending journalist: " << journalist << std::endl;
		auto journalist_wrapper = msg->mutable_journalist()->mutable_entry()->add_journalists();
		auto& wrapper = map.get_wrapper_ref(journalist);
		wrapper.serialize_wrapper(journalist_wrapper);
		journalist_wrapper->set_publicid(journalist);
	}

	return msg;
}

shared_ptr_message MFW::ReqJournalistFactory(shared_ptr_message&& msg, std::string& name) {
	msg->set_msg_ctx(np2ps::REQUEST);
	msg->mutable_journalist()->set_peer_name(name);
	return msg;
}

shared_ptr_message MFW::ErrorJournalistFactory(shared_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::ERROR);
	return msg;
}

shared_ptr_message MFW::UserInfoFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::USER_INFO);

	return msg;
}

shared_ptr_message MFW::AdvertUserInfoFactory(shared_ptr_message&& msg, std::set<pk_t> users) {
	msg->set_msg_ctx(np2ps::ONE_WAY);
	msg->mutable_user_info()->set_method(np2ps::ADVERT_UI);
	for (auto&& user : users) {
		msg->mutable_user_info()->add_peers(user);
	}
	return msg;
}

shared_ptr_message MFW::ReqUserInfoFactory(shared_ptr_message&& msg, std::set<pk_t> users) {
	msg->set_msg_ctx(np2ps::ONE_WAY);
	msg->mutable_user_info()->set_method(np2ps::REQUEST_UI);
	for (auto&& user : users) {
		msg->mutable_user_info()->add_peers(user);
	}
	return msg;
}

shared_ptr_message MFW::RespUserInfoFactory(shared_ptr_message&& msg, std::list<std::pair<pk_t, IpWrapper>> users) {
	msg->set_msg_ctx(np2ps::ONE_WAY);
	msg->mutable_user_info()->set_method(np2ps::RESPONSE_UI);
	for (auto&& user : users) {
		IpWrapper ip = user.second;
		auto* gpb_ipw = msg->mutable_user_info()->add_peer_ip_wrappers();
		ip.serialize_wrapper(gpb_ipw, false);
		gpb_ipw->set_publicid(user.first);
	}
	return msg;
}

shared_ptr_message MFW::GossipFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::GOSSIP);

	return msg;
}

shared_ptr_message MFW::ReqGossipFactory(shared_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::REQUEST);
	return msg;	
}

shared_ptr_message MFW::OneWayGossipFactory(shared_ptr_message&& msg, std::list<std::pair<pk_t, IpWrapper>> users) {
	msg->set_msg_ctx(np2ps::ONE_WAY);
	for (auto&& user : users) {
		IpWrapper ip = user.second;
		auto* gpb_ipw = msg->mutable_gossip()->add_peer_ip_wrappers();
		ip.serialize_wrapper(gpb_ipw, false);
		gpb_ipw->set_publicid(user.first);
	}
	return msg;	
}

shared_ptr_message MFW::RespGossipFactory(shared_ptr_message&& msg, std::list<std::pair<pk_t, IpWrapper>> users) {
	msg->set_msg_ctx(np2ps::RESPONSE);
	for (auto&& user : users) {
		IpWrapper ip = user.second;
		auto* gpb_ipw = msg->mutable_gossip()->add_peer_ip_wrappers();
		ip.serialize_wrapper(gpb_ipw, false);
		gpb_ipw->set_publicid(user.first);
	}
	return msg;	
}

shared_ptr_message MFW::NewJournalistFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::NEW_JOURNALIST);

	return msg;
}

shared_ptr_message MFW::OneWayNewJournalistFactory(shared_ptr_message&& msg, pk_t journalist_id, IpWrapper journalist) {
	msg->set_msg_ctx(np2ps::ONE_WAY);
	auto* gpb_ipw = msg->mutable_new_journalist()->mutable_journalist_ip_wrapper();
	journalist.serialize_wrapper(gpb_ipw, false);
	gpb_ipw->set_publicid(journalist_id);
	return msg;
}

shared_ptr_message MFW::ReqNewspaperListFactory(shared_ptr_message&& msg, const news_database& news) {
	for (auto&& one_news : news) {
		msg->mutable_newspaper_list()->add_requested_ids(one_news.first);
	}
	return msg;
}

shared_ptr_message MFW::OneWayArticleAllFactory(shared_ptr_message&& msg, Article& article) {
	auto header_ptr = msg->mutable_article_all()->mutable_header();
	msg->set_msg_ctx(np2ps::ONE_WAY);
	article.network_serialize_article(header_ptr);

	msg->mutable_article_all()->set_article_actual(article.read_contents());


	return std::move(msg);
}

shared_ptr_message MFW::PingFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::PING);

	return msg;
}
