#include "Message.h"

using MFW = MessageFactoryWrapper;

unique_ptr_message MFW::ArticleDataChangeFactory(pk_t from, pk_t to, hash_t article_hash, bool download) {
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

unique_ptr_message MFW::ArticleDownloadFactory(pk_t from, pk_t to, hash_t article_hash, level_t level) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::ARTICLE_ALL);

	msg->mutable_article_all()->set_article_hash(article_hash);
	msg->mutable_article_all()->set_level(level);

	return std::move(msg);
}

unique_ptr_message MFW::ArticleHeaderFactory(pk_t from, pk_t to, hash_t article_hash) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::ARTICLE_HEADER);

	msg->mutable_article_header()->set_article_hash(article_hash);

	return std::move(msg);
}

unique_ptr_message MFW::ArticleListFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::ARTICLE_LIST);

	return std::move(msg);
}

unique_ptr_message MFW::UserIsMemberFactory(pk_t from, pk_t to, pk_t user_pk) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::USER_IS_MEMBER);

	msg->mutable_user_is_member()->set_user_pk(user_pk);

	return std::move(msg);
}

unique_ptr_message MFW::update_margin_factory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margins) {
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

unique_ptr_message MFW::UpdateMarginFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::ADD);

	return std::move(msg);
}

unique_ptr_message MFW::UpdateMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	return std::move(msg);
}

unique_ptr_message MFW::UpdateMarginRemoveFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::REMOVE);

	return std::move(msg);
}

unique_ptr_message MFW::UpdateMarginUpdateFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::UPDATE);

	return std::move(msg);
}

unique_ptr_message MFW::UpdateArticleFactory(pk_t from, pk_t to, hash_t article_hash) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::UPDATE_ARTICLE);

	msg->mutable_update_article()->set_article_pk(article_hash);

	return std::move(msg);
}

unique_ptr_message MFW::CredentialsFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::CREDENTIALS);

	return std::move(msg);
}

/* Requests: */
unique_ptr_message MFW::ReqUserIsMemberFactory(unique_ptr_message&& msg, level_t level) { 
	msg->mutable_user_is_member()->set_level(level);
	msg->set_msg_ctx(np2ps::REQUEST);
	return std::move(msg);
}

unique_ptr_message MFW::ReqCredentialsFactory(unique_ptr_message&& msg, bool req_ip4, bool req_ip6, bool req_public_key, bool req_eax_key,
	string_ptr_optional ip4, string_ptr_optional ip6, rsa_public_ptr_optional public_key, eax_ptr_optional eax_key) {

		msg->set_msg_ctx(np2ps::REQUEST);

		msg->mutable_credentials()->set_req_ipv4(req_ip4);
		msg->mutable_credentials()->set_req_ipv6(req_ip6);
		msg->mutable_credentials()->set_req_rsa_public_key(req_public_key);
		msg->mutable_credentials()->set_req_eax_key(req_eax_key);

		if (ip4.has_value()) {
			msg->mutable_credentials()->set_ipv4(*ip4.value());
		}

		if (ip6.has_value()) {
			msg->mutable_credentials()->set_ipv6(*ip6.value());
		}

		if (public_key.has_value()) {
			std::string string_key;
			CryptoPP::StringSink rsa_pub_sink(string_key);
			public_key.value()->DEREncode(rsa_pub_sink);
			msg->mutable_credentials()->set_rsa_public_key(string_key);
		}

		if (eax_key.has_value()) {
			std::string string_key(reinterpret_cast<const char*>(&(*eax_key.value())[0]), eax_key.value()->size());
			msg->mutable_credentials()->set_eax_key(string_key);
		}

		return std::move(msg);
	}



unique_ptr_message MFW::RespArticleDownloadFactory(unique_ptr_message&& msg, article_ptr article_header, std::string&& article) { 
	auto header_ptr = msg->mutable_article_all()->mutable_header();
	msg->set_msg_ctx(np2ps::RESPONSE);
	article_header->network_serialize_article(header_ptr);

	msg->mutable_article_all()->set_article_actual(article);


	return std::move(msg);
}

unique_ptr_message MFW::RespArticleHeaderFactory(unique_ptr_message&& msg, article_ptr article_header) {
	auto header_ptr = msg->mutable_article_header()->mutable_article();
	article_header->network_serialize_article(header_ptr);
	msg->set_msg_ctx(np2ps::RESPONSE);

	return std::move(msg);
}

unique_ptr_message MFW::ReqArticleHeaderFactory(unique_ptr_message&& msg, Article* article_header) {
	auto header_ptr = msg->mutable_article_header()->mutable_article();
	article_header->network_serialize_article(header_ptr);
	msg->set_msg_ctx(np2ps::REQUEST);

	return std::move(msg);
}

unique_ptr_message MFW::RespArticleListFactory(unique_ptr_message&& msg, article_container& articles) { 
	for(auto&& article : articles) {
		auto a = msg->mutable_article_list()->add_response();
		article->network_serialize_article(a);
	}
	msg->set_msg_ctx(np2ps::RESPONSE);
	return std::move(msg);
}

unique_ptr_message MFW::RespUserIsMemberFactory(unique_ptr_message&& msg, bool is_member, level_t req_level) { 
	msg->mutable_user_is_member()->set_is_member(is_member);
	msg->mutable_user_is_member()->set_level(req_level);
	msg->set_msg_ctx(np2ps::RESPONSE);
	return std::move(msg);
}

unique_ptr_message MFW::RespCredentialsFactory(unique_ptr_message&& msg, QString ip4, QString ip6, 
	std::shared_ptr<rsa_public_optional> public_key, std::shared_ptr<eax_optional> eax_key) {
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

		if (public_key->has_value()) {
			std::string string_key;
			CryptoPP::StringSink rsa_pub_sink(string_key);
			public_key->value().DEREncode(rsa_pub_sink);
			msg->mutable_credentials()->set_rsa_public_key(string_key);
			msg->mutable_credentials()->set_req_rsa_public_key(true);
		}
		else
			msg->mutable_credentials()->set_req_rsa_public_key(false);

		if (eax_key->has_value()) {
			std::string string_key(reinterpret_cast<const char*>(&(eax_key->value())[0]), eax_key->value().size());
			msg->mutable_credentials()->set_eax_key(string_key);
			msg->mutable_credentials()->set_req_eax_key(true);
		}
		else
			msg->mutable_credentials()->set_req_eax_key(false);

		return std::move(msg);
	}

unique_ptr_message MFW::PublicKeyFactory(pk_t from, pk_t to, CryptoPP::RSA::PublicKey& key) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::PUBLIC_KEY);

	std::string key_str;

	CryptoPP::StringSink rsa_pub_sink(key_str);
	key.DEREncode(rsa_pub_sink);

	msg->mutable_public_key()->set_key(key_str);
	return std::move(msg);
}

unique_ptr_message MFW::UpdateSeqNumber(unique_ptr_message&& msg, unsigned int seq) {
	msg->set_seq(seq);
	return std::move(msg);
}

unique_ptr_message MFW::ErrorArticleListFactory(unique_ptr_message&& msg, pk_t newspaper_id) {
	MFW::SetMessageContextError(msg);
	msg->mutable_article_list()->set_newspaper_id(newspaper_id);
	return std::move(msg);
}

unique_ptr_message MFW::SetMessageContextRequest(unique_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::REQUEST);
	return std::move(msg);
}

unique_ptr_message MFW::SetMessageContextResponse(unique_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::RESPONSE);
	return std::move(msg);
}

unique_ptr_message MFW::SetMessageContextOneWay(unique_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::ONE_WAY);
	return std::move(msg);
}

unique_ptr_message MFW::SetMessageContextError(unique_ptr_message&& msg) {
	msg->set_msg_ctx(np2ps::ERROR);
	return std::move(msg);
}

void MFW::SetMessageContextError(unique_ptr_message& msg) {
	msg->set_msg_ctx(np2ps::ERROR);
}

unique_ptr_message MFW::RespNewspaperEntryFactory(unique_ptr_message&& msg, NewspaperEntry& news) {
	news.network_serialize_entry(msg->mutable_newspaper_entry());
	msg->set_msg_ctx(np2ps::RESPONSE);
	return msg;
}

unique_ptr_message MFW::NewspaperEntryFactory(pk_t from, pk_t to, pk_t newspaper_id) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::NEWSPAPER_ENTRY);

	msg->mutable_newspaper_entry()->mutable_entry()->set_news_id(newspaper_id);

	return msg;
}