#ifndef PROGRAM_MESSAGE_H
#define PROGRAM_MESSAGE_H

#include "GlobalUsing.h"
#include "Article.h"
#include "IpWrapper.h"
#include "Networking.h"
#include "protobuf_source/messages.pb.h"

using margin_ptr = std::unique_ptr<Margin>;
using msg_ctx_t = np2ps::MessageContext;

class MessageFactoryWrapper {
public:
/*
	// Maybe one day...
	static unique_ptr_message ReqIpAddressFactory(pk_t from, pk_t to);
	static unique_ptr_message ReqMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, const Margin& margin);
	static unique_ptr_message ReqArticleDataChangeFactory(pk_t from, pk_t to, hash_t article_hash, bool download);
	static unique_ptr_message ReqArticleDownloadFactory(pk_t from, pk_t to, hash_t article_hash, level_t level);
	static unique_ptr_message ReqArticleListFactory(pk_t from, pk_t to, category_container_const_ref categories);
	static unique_ptr_message ReqArticleListFactory(pk_t from, pk_t to);
	static unique_ptr_message ReqUserIsMemberFactory(pk_t from, pk_t to, pk_t user_pk, level_t level);
	static unique_ptr_message ReqUpdateMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message ReqUpdateMarginRemoveFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message ReqUpdateMarginUpdateFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message ReqUpdateArticleFactory(pk_t from, pk_t to, hash_t article_hash);

	static unique_ptr_message RespIpAddressFactory(pk_t from, pk_t to);
	static unique_ptr_message RespMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, const Margin& margin);
	static unique_ptr_message RespArticleDataChangeFactory(pk_t from, pk_t to, hash_t article_hash, bool download);
	static unique_ptr_message RespArticleDownloadFactory(pk_t from, pk_t to, hash_t article_hash, level_t level);
	static unique_ptr_message RespArticleListFactory(pk_t from, pk_t to, category_container_const_ref categories);
	static unique_ptr_message RespArticleListFactory(pk_t from, pk_t to);
	static unique_ptr_message RespUserIsMemberFactory(pk_t from, pk_t to, pk_t user_pk, level_t level);
	static unique_ptr_message RespUpdateMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message RespUpdateMarginRemoveFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message RespUpdateMarginUpdateFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message RespUpdateArticleFactory(pk_t from, pk_t to, hash_t article_hash);
*/
	static unique_ptr_message IpAddressFactory(msg_ctx_t mt, pk_t from, pk_t to);
	static unique_ptr_message MarginAddFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, const Margin& margin);
	static unique_ptr_message ArticleDataChangeFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, bool download);
	static unique_ptr_message ArticleDownloadFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, level_t level);
	static unique_ptr_message ArticleListFactory(msg_ctx_t mt, pk_t from, pk_t to, category_container_const_ref categories);
	static unique_ptr_message ArticleListFactory(msg_ctx_t mt, pk_t from, pk_t to);
	static unique_ptr_message UserIsMemberFactory(msg_ctx_t mt, pk_t from, pk_t to, pk_t user_pk, level_t level);
	static unique_ptr_message UpdateMarginAddFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message UpdateMarginRemoveFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message UpdateMarginUpdateFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message UpdateArticleFactory(msg_ctx_t mt, pk_t from, pk_t to, hash_t article_hash);
};


#endif //PROGRAM_MESSAGE_H
