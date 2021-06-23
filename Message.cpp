#include "Message.h"

using MFW = MessageFactoryWrapper;

void set_from_to(unique_ptr_message& upm, pk_t from, pk_t to) {
	upm->set_from(from);
	upm->set_to(to);
}

unique_ptr_message upm_factory() {
	return std::make_unique<proto_message>();
}

unique_ptr_message MFW::IpAddressFactory(msg_ctx_t mt, pk_t from, pk_t to) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::IP_ADDRESS);
	msg->set_msg_ctx(mt);
	return std::move(msg);
}

unique_ptr_message MFW::MarginAddFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, const Margin& margin) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::UPDATE_MARGIN);
	msg->set_msg_ctx(mt);

	msg->mutable_update_margin()->set_m_action(np2ps::ADD);
	msg->mutable_update_margin()->set_article_pk(article_hash);

	msg->mutable_update_margin()->mutable_margin()->set_type(margin.type);
	msg->mutable_update_margin()->mutable_margin()->set_content(margin.content);
	return std::move(msg);
}

unique_ptr_message MFW::ArticleDataChangeFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, bool download) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::ARTICLE_DATA_UPDATE);
	msg->set_msg_ctx(mt);
	msg->mutable_article_data_update()->set_article_pk(article_hash);

	if (download) {
		msg->mutable_article_data_update()->set_article_action(np2ps::DOWNLOAD);
	}
	else {
		msg->mutable_article_data_update()->set_article_action(np2ps::REMOVAL);
	}

	return std::move(msg);
}

unique_ptr_message MFW::ArticleDownloadFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, level_t level) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_ctx(mt);
	msg->set_msg_type(np2ps::ARTICLE_ALL);

	msg->mutable_article_all()->set_article_hash(article_hash);
	msg->mutable_article_all()->set_level(level);

	return std::move(msg);
}

unique_ptr_message MFW::ArticleListFactory(msg_ctx_t mt, pk_t from, pk_t to, category_container_const_ref categories) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_ctx(mt);
	msg->set_msg_type(np2ps::ARTICLE_LIST);

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

unique_ptr_message MessageFactoryWrapper::ArticleListFactory(msg_ctx_t mt, pk_t from, pk_t to) {
	return ArticleListFactory(mt, from, to, category_container());
}

unique_ptr_message MFW::UserIsMemberFactory(msg_ctx_t mt, pk_t from, pk_t to, pk_t user_pk, level_t level) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::USER_IS_MEMBER);
	msg->set_msg_ctx(mt);

	msg->mutable_user_is_member()->set_user_pk(user_pk);
	msg->mutable_user_is_member()->set_level(level);

	return std::move(msg);
}

unique_ptr_message update_margin_factory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, margin_ptr& margin) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_type(np2ps::UPDATE_MARGIN);
	msg->set_msg_ctx(mt);

	msg->mutable_update_margin()->set_article_pk(article_hash);

	msg->mutable_update_margin()->mutable_margin()->set_content(margin->content);
	msg->mutable_update_margin()->mutable_margin()->set_type(margin->type);

	return std::move(msg);
}

unique_ptr_message MFW::UpdateMarginAddFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, margin_ptr margin) {
	auto msg = update_margin_factory(mt, from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::ADD);

	return std::move(msg);
}

unique_ptr_message MFW::UpdateMarginRemoveFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, margin_ptr margin) {
	auto msg = update_margin_factory(mt, from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::REMOVE);

	return std::move(msg);
}

unique_ptr_message MFW::UpdateMarginUpdateFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, margin_ptr margin) {
	auto msg = update_margin_factory(mt, from, to, article_hash, margin);

	msg->mutable_update_margin()->set_m_action(np2ps::UPDATE);

	return std::move(msg);
}

unique_ptr_message MFW::UpdateArticleFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash) {
	auto msg = upm_factory();
	set_from_to(msg, from, to);

	msg->set_msg_ctx(mt);
	msg->set_msg_type(np2ps::UPDATE_ARTICLE);

	msg->mutable_update_article()->set_article_pk(article_hash);

	return std::move(msg);
}
