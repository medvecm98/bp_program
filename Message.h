#ifndef PROGRAM_MESSAGE_H
#define PROGRAM_MESSAGE_H

#include <random>

#include "GlobalUsing.h"
#include "Article.h"
#include "IpWrapper.h"
#include "Networking.h"
#include "protobuf_source/messages.pb.h"

using margin_ptr = std::unique_ptr<Margin>;
using msg_ctx_t = np2ps::MessageContext;

class MessageFactoryWrapper {
public:

	/* Requests: */
	static unique_ptr_message ReqArticleListFactory(unique_ptr_message&& msg, category_container& categories);
	static unique_ptr_message ReqUserIsMemberFactory(unique_ptr_message&& msg, level_t level);

	/* Responses: */
	static unique_ptr_message RespIpAddressFactory(unique_ptr_message&& msg, const std::string& ip4);
	static unique_ptr_message RespIpAddressFactory(unique_ptr_message&& msg, const std::string& ip4, const std::string& ip6);
	static unique_ptr_message RespArticleDownloadFactory(unique_ptr_message&& msg, article_ptr article_header, std::string&& article);
	static unique_ptr_message RespArticleHeaderFactory(unique_ptr_message&& msg, article_ptr article_header);
	static unique_ptr_message RespArticleListFactory(unique_ptr_message&& msg, article_container& articles);
	static unique_ptr_message RespUserIsMemberFactory(unique_ptr_message&& msg, bool is_member);

	/* Basic factories: */
	static unique_ptr_message IpAddressFactory(pk_t from, pk_t to);
	static unique_ptr_message ArticleDataChangeFactory(pk_t from, pk_t to, hash_t article_hash, bool download);
	static unique_ptr_message ArticleDownloadFactory(pk_t from, pk_t to, hash_t article_hash, level_t level);
	static unique_ptr_message ArticleHeaderFactory(pk_t from, pk_t to, hash_t article_hash);
	static unique_ptr_message ArticleListFactory(pk_t from, pk_t to);
	static unique_ptr_message UserIsMemberFactory(pk_t from, pk_t to, pk_t user_pk);
	static unique_ptr_message UpdateMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin);
	static unique_ptr_message UpdateMarginRemoveFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin);
	static unique_ptr_message UpdateMarginUpdateFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin);
	static unique_ptr_message UpdateArticleFactory(pk_t from, pk_t to, hash_t article_hash);
	
	static unique_ptr_message UpdateSeqNumber(unique_ptr_message&& msg, unsigned int seq);

	/* For those messages that don't have special factories for request or response, or both. */
	static unique_ptr_message SetMessageContextRequest(unique_ptr_message&& msg);
	static unique_ptr_message SetMessageContextResponse(unique_ptr_message&& msg);
	static unique_ptr_message SetMessageContextOneWay(unique_ptr_message&& msg);
	static unique_ptr_message SetMessageContextError(unique_ptr_message&& msg);
};


#endif //PROGRAM_MESSAGE_H
