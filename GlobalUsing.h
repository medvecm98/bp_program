#ifndef PROGRAM_GLOBALUSING_H
#define PROGRAM_GLOBALUSING_H

#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>

using pk_t = std::size_t;

struct AuthorPeers {
	pk_t author;
	std::set<pk_t> peers;
	explicit AuthorPeers(pk_t a) {
		author = a;
	}
};

using my_string = std::string;
using hash_t = std::size_t;
using level_t = std::uint32_t;
using user_container = std::unordered_set<pk_t>;

/*
 * Category -> Article -> Author, Readers (those, who downloaded those articles)
 */
using user_multimap_container = std::unordered_map<hash_t, AuthorPeers>;
using category_multimap_container = std::unordered_multimap<my_string, user_multimap_container>;

#endif //PROGRAM_GLOBALUSING_H
