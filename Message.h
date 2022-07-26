#ifndef PROGRAM_MESSAGE_H
#define PROGRAM_MESSAGE_H

#include <random>

#include "GlobalUsing.h"
#include "Article.h"
#include "NewspaperEntry.h"
#include "IpWrapper.h"
//#include "Networking.h"
#include "protobuf_source/messages.pb.h"

using margin_ptr = std::unique_ptr<Margin>;
using msg_ctx_t = np2ps::MessageContext;

using proto_message = np2ps::Message;
using shared_ptr_message = std::shared_ptr< proto_message>;

using string_ptr_optional = std::optional<std::shared_ptr<std::string>>;
using rsa_public_ptr_optional = std::optional<std::shared_ptr<CryptoPP::RSA::PublicKey>>;
using eax_ptr_optional = std::optional<std::shared_ptr<CryptoPP::SecByteBlock>>;

class MessageFactoryWrapper {
public:

	/**
	 * @brief Sets the sender and receiver of the np2ps message.
	 * 
	 * Sets the random sequence number.
	 * 
	 * @param upm Message shared pointer.
	 * @param from Message sender.
	 * @param to Message receiver.
	 */
	static void set_from_to(shared_ptr_message& upm, pk_t from, pk_t to) {
		upm->set_from(from);
		upm->set_to(to);

		std::random_device rd("/dev/urandom");
		upm->set_seq(rd());
	}

	static shared_ptr_message upm_factory() {
		return std::make_shared<proto_message>();
	}
	/////////////////////////////
	//
	// REQUESTS:
	//
	/////////////////////////////

	/**
	 * @brief Factory for Article message with request context.
	 * 
	 * @tparam CategoryContainer Type of container containing categories.
	 * @param msg Barebones Article message, with only mandatory fields filled.
	 * @param categories Container of categories.
	 * @return shared_ptr_message 
	 */
	template <typename CategoryContainer>
	static shared_ptr_message ReqArticleListFactory(shared_ptr_message&& msg, const CategoryContainer& categories) {
		msg->mutable_article_list()->set_all_articles(true);

		msg->set_msg_ctx(np2ps::REQUEST);
		
		for (auto &&cat : categories) {
			msg->mutable_article_list()->set_all_articles(false);
			msg->mutable_article_list()->add_categories(cat);
		}
		return std::move(msg);
	}

	/**
	 * @brief Factory for Article message with request context.
	 * 
	 * @tparam CategoryContainer Type of container containing categories.
	 * @param msg Barebones Article message, with only mandatory fields filled.
	 * @param categories Container of categories.
	 * @return shared_ptr_message 
	 */
	template <typename CategoryContainer>
	static shared_ptr_message ReqArticleListFactory(shared_ptr_message&& msg, pk_t newspaper_id, int count, const CategoryContainer& categories) {
		msg->mutable_article_list()->set_all_articles(true);

		msg->set_msg_ctx(np2ps::REQUEST);
		
		for (auto &&cat : categories) {
			msg->mutable_article_list()->set_all_articles(false);
			msg->mutable_article_list()->add_categories(cat);
		}

		msg->mutable_article_list()->set_newspaper_id(newspaper_id);
		msg->mutable_article_list()->set_count(count);

		return std::move(msg);
	}
	
	static shared_ptr_message ReqUserIsMemberFactory(shared_ptr_message&& msg, level_t level);
	static shared_ptr_message ReqCredentialsFactory(shared_ptr_message&& msg, bool req_ip4, bool req_ip6, bool req_public_key, bool req_eax_key,
		string_ptr_optional ip4, string_ptr_optional ip6, rsa_public_ptr_optional public_key, eax_ptr_optional eax_key);

	/* Responses: */
	//static shared_ptr_message RespIpAddressFactory(shared_ptr_message&& msg, const std::string& ip4);
	//static shared_ptr_message RespIpAddressFactory(shared_ptr_message&& msg, const std::string& ip4, const std::string& ip6);
	static shared_ptr_message RespArticleDownloadFactory(shared_ptr_message&& msg, article_ptr article_header, std::string&& article);
	static shared_ptr_message RespArticleHeaderFactory(shared_ptr_message&& msg, article_ptr article_header);
	static shared_ptr_message RespArticleListFactory(shared_ptr_message&& msg, article_container& articles);
	static shared_ptr_message RespUserIsMemberFactory(shared_ptr_message&& msg, bool is_member, level_t req_level);
	//static shared_ptr_message RespNewPublicKeyFactory(shared_ptr_message&& msg, const CryptoPP::RSA::PublicKey& public_key);
	static shared_ptr_message RespCredentialsFactory(shared_ptr_message&& msg, QString ip4, QString ip6, 
		std::shared_ptr<rsa_public_optional> public_key, std::shared_ptr<eax_optional> eax_key);
	static shared_ptr_message RespNewspaperEntryFactory(shared_ptr_message&& msg, NewspaperEntry& news);

	static shared_ptr_message ReqArticleHeaderFactory(shared_ptr_message&& msg, Article* article_header);

	static shared_ptr_message ErrorArticleListFactory(shared_ptr_message&& msg, pk_t newspaper_id);

	/* Basic factories: */
	
	static shared_ptr_message ArticleDataChangeFactory(pk_t from, pk_t to, hash_t article_hash, bool download);
	static shared_ptr_message ArticleDownloadFactory(pk_t from, pk_t to, hash_t article_hash, level_t level);
	static shared_ptr_message ArticleHeaderFactory(pk_t from, pk_t to, hash_t article_hash);
	static shared_ptr_message ArticleListFactory(pk_t from, pk_t to);
	static shared_ptr_message UserIsMemberFactory(pk_t from, pk_t to, pk_t user_pk);
	static shared_ptr_message UpdateMarginAddFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin);
	static shared_ptr_message UpdateMarginRemoveFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin);
	static shared_ptr_message UpdateMarginUpdateFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin);
	static shared_ptr_message UpdateMarginFactory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margin);
	static shared_ptr_message UpdateArticleFactory(pk_t from, pk_t to, hash_t article_hash);
	static shared_ptr_message CredentialsFactory(pk_t from, pk_t to);
	static shared_ptr_message PublicKeyFactory(pk_t from, pk_t to, CryptoPP::RSA::PublicKey& key);
	static shared_ptr_message NewspaperEntryFactory(pk_t from, pk_t to, pk_t newspaper_id);

	template<typename PeerContainer>
	static shared_ptr_message ArticleSolicitationFactory(pk_t from, pk_t to, hash_t article_hash, const PeerContainer& peers, std::int32_t level = 127) {
		shared_ptr_message msg = upm_factory();
		set_from_to(msg, from, to);

		msg->set_msg_type(np2ps::ARTICLE_SOLICITATION);

		msg->mutable_article_sol()->set_article_hash(article_hash);
		for (auto&& peer : peers) {
			msg->mutable_article_sol()->add_possible_owners(peer);
		}
		msg->mutable_article_sol()->set_level(level);
		return std::move(msg);
	}
	
	static shared_ptr_message UpdateSeqNumber(shared_ptr_message&& msg, unsigned int seq);

	static shared_ptr_message update_margin_factory(pk_t from, pk_t to, hash_t article_hash, margin_vector& margins);

	/* For those messages that don't have special factories for request and/or response. */
	static shared_ptr_message SetMessageContextRequest(shared_ptr_message&& msg);
	static shared_ptr_message SetMessageContextResponse(shared_ptr_message&& msg);
	static shared_ptr_message SetMessageContextOneWay(shared_ptr_message&& msg);
	static shared_ptr_message SetMessageContextError(shared_ptr_message&& msg);
	static void SetMessageContextError(shared_ptr_message& msg);
};


#endif //PROGRAM_MESSAGE_H
