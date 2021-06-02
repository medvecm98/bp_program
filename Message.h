#ifndef PROGRAM_MESSAGE_H
#define PROGRAM_MESSAGE_H

#include "GlobalUsing.h"
#include "Article.h"
#include "IpWrapper.h"

struct MessageType {
	enum E {
		empty = 0,
		articleAll = 10,
		articleHeader = 20,
		userIsMember = 30,
		articleDownload = 40,
		articleRemoval = 50,
		handshake = 60,
		updateMarginAdd = 70,
		updateMarginRemove = 80,
		updateArticle = 90,
		ipAddress = 100
	};
};

struct MessageContext {
	enum E {
		empty = 0,
		request = 10,
		response = 20
	};
};

using my_vector = std::vector<uint8_t>;

class Message {
public:
	virtual ~Message() noexcept {}
	virtual my_vector Serialize();
	virtual bool Deserialize(my_vector);
protected:
	Message(pk_t from, pk_t to) {
		from_ = from;
		to_ = to;
		msg_type_ = MessageType::empty;
		msg_context_ = MessageContext::empty;
	}
	pk_t from_, to_;
	MessageType::E msg_type_;
	MessageContext::E msg_context_;
};

class ArticleHeaderMessage : public Message {
public:
	explicit ArticleHeaderMessage() : Message(0,0) {
		article_hash_ = 0;
		level_ = 0;
	}

	ArticleHeaderMessage(hash_t req, level_t lvl, pk_t from, pk_t to) : Message(from, to) {
		msg_context_ = MessageContext::request;
		msg_type_ = MessageType::articleHeader;

		from_ = from;
		to_ = to;

		article_hash_ = req;
		level_ = lvl;
	}

	ArticleHeaderMessage(const Article& article, level_t lvl, pk_t from, pk_t to) : Message(from, to) {
		msg_context_ = MessageContext::response;
		msg_type_ = MessageType::articleHeader;

		from_ = from;
		to_ = to;

		article_hash_ = article.get_main_hash();
		article_ = article;
		level_ = lvl;
	}

	my_vector Serialize() override;
	bool Deserialize(my_vector) override;

protected:
	hash_t article_hash_;
	level_t level_;
	Article article_;
};

class IpAddressMessage : public Message {
public:
	IpAddressMessage(pk_t from, pk_t to) : Message(from, to), ip_("") {
		msg_context_ = MessageContext::request;
		msg_type_ = MessageType::ipAddress;
	}

	IpAddressMessage(pk_t from, pk_t to, IpWrapper ip) : Message(from, to), ip_(std::move(ip)) {
		msg_context_ = MessageContext::response;
		msg_type_ = MessageType::ipAddress;
	}
protected:
	IpWrapper ip_;
};


#endif //PROGRAM_MESSAGE_H
