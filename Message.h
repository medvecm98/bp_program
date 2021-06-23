#ifndef PROGRAM_MESSAGE_H
#define PROGRAM_MESSAGE_H

#include "GlobalUsing.h"
#include "Article.h"
#include "IpWrapper.h"
#include "Networking.h"
#include "protobuf_source/messages.pb.h"

using margin_ptr = std::unique_ptr<Margin>;

class MessageFactoryWrapper {
public:
	static unique_ptr_message IpAddressFactory(pk_t from, pk_t to);
	static unique_ptr_message MarginAddFactory(pk_t from, pk_t to, hash_t article_hash, const Margin& margin);
	static unique_ptr_message ArticleDataChangeFactory(pk_t from, pk_t to, hash_t article_hash, bool download);
	static unique_ptr_message ArticleDownloadFactory(pk_t from, pk_t to, hash_t article_hash, level_t level);
	static unique_ptr_message ArticleListFactory(pk_t from, pk_t to, category_container_const_ref categories);
	static unique_ptr_message ArticleListFactory(pk_t from, pk_t to);
	static unique_ptr_message UserIsMemberFactory(pk_t from, pk_t to, pk_t user_pk, level_t level);
	static unique_ptr_message UpdateMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message UpdateMarginRemoveFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message UpdateMarginUpdateFactory(pk_t from, pk_t to, hash_t article_hash, margin_ptr margin);
	static unique_ptr_message UpdateArticleFactory(pk_t from, pk_t to, hash_t article_hash);
};


#endif //PROGRAM_MESSAGE_H
