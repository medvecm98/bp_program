#ifndef PROGRAM_MESSAGE_H
#define PROGRAM_MESSAGE_H

#include "GlobalUsing.h"
#include "Article.h"
#include "IpWrapper.h"
#include "protobuf_source/messages.pb.h"

class MessageFactoryWrapper {
public:
	static np2ps::Message IpAddressFactory(pk_t from, pk_t to);
	static np2ps::Message MarginAddFactory(pk_t from, pk_t to, hash_t article_hash, const Margin& margin);
};


#endif //PROGRAM_MESSAGE_H
