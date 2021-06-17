#include "Message.h"

np2ps::Message MessageFactoryWrapper::IpAddressFactory(pk_t from, pk_t to) {
	np2ps::Message msg;
	msg.set_from(from);
	msg.set_to(to);
	msg.set_msg_type(np2ps::IP_ADDRESS);
	msg.set_msg_ctx(np2ps::REQUEST);
	return std::move(msg);
}

np2ps::Message MessageFactoryWrapper::MarginAddFactory(pk_t from, pk_t to, hash_t article_hash, const Margin& margin) {
	np2ps::Message msg;
	msg.set_from(from);
	msg.set_to(to);
	msg.set_msg_type(np2ps::UPDATE_MARGIN);
	msg.set_msg_ctx(np2ps::REQUEST);

	msg.mutable_update_margin()->set_m_action(np2ps::ADD);
	msg.mutable_update_margin()->set_article_pk(article_hash);

	msg.mutable_update_margin()->mutable_margin()->set_type(margin.type);
	msg.mutable_update_margin()->mutable_margin()->set_content(margin.content);
	return std::move(msg);
}

