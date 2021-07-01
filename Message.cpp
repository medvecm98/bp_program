#include "Message.h"

using MFW = MessageFactoryWrapper;

void set_from_to(unique_ptr_message& upm, pk_t from, pk_t to) {
	upm->set_from(from);
	upm->set_to(to);

	std::random_device rd("/dev/urandom");
	upm->set_seq(rd());
}

unique_ptr_message upm_factory() {
	return std::make_unique<proto_message>();
}

unique_ptr_message MFW::IpAddressFactory(pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::IP_ADDRESS);

	return std::move(msg);
}

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

unique_ptr_message update_margin_factory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margins) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::UPDATE_MARGIN);

	msg->mutable_update_margin()->set_article_pk(article_hash);

	for (auto&& margin : margins) {
		auto marg_ptr = msg->mutable_update_margin()->mutable_margin()->add_margins();
		marg_ptr->set_type(margin.type);
		marg_ptr->set_content(margin.content);
	}

	return std::move(msg);
}

unique_ptr_message MFW::UpdateMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin) {
	auto msg = update_margin_factory(from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::ADD);

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

/* Requests: */
unique_ptr_message MFW::ReqArticleListFactory(unique_ptr_message&& msg, category_container& categories) { 
	if (!categories.empty()) {
		msg->mutable_article_list()->set_all_articles(false);
		for (auto &&cat : categories) {
			msg->mutable_article_list()->add_categories(cat);
		}
	}
	else {
		msg->mutable_article_list()->set_all_articles(true);
	}
	return std::move(msg);
}

unique_ptr_message MFW::ReqUserIsMemberFactory(unique_ptr_message&& msg, level_t level) { 
	msg->mutable_user_is_member()->set_level(level);
	return std::move(msg);
}

/* Responses: */
unique_ptr_message MFW::RespIpAddressFactory(unique_ptr_message&& msg, const std::string& ip4) { 
	msg->mutable_ip_address()->set_ipv4(ip4);
	return std::move(msg);
}

unique_ptr_message MFW::RespIpAddressFactory(unique_ptr_message&& msg, const std::string& ip4, const std::string& ip6) { 
	msg->mutable_ip_address()->set_ipv6(ip6);
	msg->mutable_ip_address()->set_ipv4(ip4);
	return std::move(msg);
}

/**
 * @brief Converts article from internal representation to protocol buffer representation.
 */
void CreateArticle(np2ps::Article* art, article_ptr article) {
	art->set_author_id(article->author_id());
	art->set_author_name(article->author_name());
	art->set_news_id(article->news_id());
	art->set_news_name(article->news_name());
	art->set_main_hash(article->main_hash());
	art->set_heading(article->heading());
 
	auto [hi, hie] = article->hashes();
	for (; hi != hie; hi++) {
		np2ps::HashWrapper hw;
		hw.set_hash(hi->second.hash);
		hw.set_level(hi->second.paragraph_level);

		google::protobuf::MapPair<google::protobuf::int32, np2ps::HashWrapper> vt(hi->first, hw);
		art->mutable_paragraph_hashes()->insert(vt);
	}

	art->set_length(article->length());
	
	auto [ci, cie] = article->categories();
	for (; ci != cie; ci++) {
		art->add_categories(*ci);
	}
 
	auto [mi, mie] = article->margins();
	pk_t actual_pk = 0; //User PK currently being processed
	np2ps::Margins margin_group;
	for (; mi != mie; mi++) {
		if (actual_pk == 0) {
			actual_pk = mi->first;
		}

		if (actual_pk != 0 && actual_pk != mi->first) {
			google::protobuf::MapPair<google::protobuf::uint64, np2ps::Margins> vt(mi->first, margin_group);
			art->mutable_margins()->insert(vt);
			margin_group.clear_margins();
			actual_pk = mi->first;
		}

		if (actual_pk == mi->first) {
			auto mga = margin_group.add_margins();
			mga->set_type(mi->second.type);
			mga->set_content(mi->second.content);
			mga->set_id(mi->second.id);
		}
	}
}

unique_ptr_message MFW::RespArticleDownloadFactory(unique_ptr_message&& msg, article_ptr article_header, std::string&& article) { 
	auto header_ptr = msg->mutable_article_all()->mutable_header();
	CreateArticle(header_ptr, article_header);
	msg->mutable_article_all()->set_article_actual(article);

	return std::move(msg);
}

unique_ptr_message MFW::RespArticleHeaderFactory(unique_ptr_message&& msg, article_ptr article_header) {
	auto header_ptr = msg->mutable_article_header()->mutable_article();
	CreateArticle(header_ptr, article_header);

	return std::move(msg);
}

unique_ptr_message MFW::RespArticleListFactory(unique_ptr_message&& msg, article_container& articles) { 
	for(auto&& article : articles) {
		auto a = msg->mutable_article_list()->add_response();
		CreateArticle(a, article);
	}
	return std::move(msg);
}

unique_ptr_message MFW::RespUserIsMemberFactory(unique_ptr_message&& msg, bool is_member) { 
	msg->mutable_user_is_member()->set_is_member(true);
	return std::move(msg);
}

unique_ptr_message MFW::UpdateSeqNumber(unique_ptr_message&& msg, unsigned int seq) {
	msg->set_seq(seq);
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