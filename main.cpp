#include <iostream>
#include <filesystem>
#include "Article.h"
#include "ArticleDatabase.h"
#include "Message.h"
#include "Peer.h"
#include "Networking.h"
#include "openssl/rsa.h"

#define ARTICLE_DIR "./articles"
#define PEER_INFO "./peer_info.nfs"
#define FILE_HEADER "./file_header.nfs"

class PeerInfoSingleton {
public:
	PeerInfoSingleton& get_instance() {
		static PeerInfoSingleton instance;
		return instance;
	}

	bool get_peer_created() {
		return peer_created_;
	}

	void set_peer_created(bool value) {
		peer_created_ = value;
	}
private:
	PeerInfoSingleton() {
		peer_created_ = false;
	}
	~PeerInfoSingleton() {

	}

	bool peer_created_;
};

void print_help() {
	std::cout << "h for help" << std::endl;
}

/**
 * Finds all the missing files, that are necessary for correct behaiour.
 */
void find_what_is_missing(std::unordered_set<std::string>& what_created) {
	if (!std::filesystem::exists(ARTICLE_DIR)) {
		what_created.insert(ARTICLE_DIR);
	}
	if (!std::filesystem::exists(PEER_INFO)) {
		what_created.insert(PEER_INFO);
	}
	if (!std::filesystem::exists(FILE_HEADER)) {
		what_created.insert(FILE_HEADER);
	}
}

void handle_input() {
	char c;
	std::cin.get(c);

	if (c) {
		switch (c) {

			case 'h':
			default:
				print_help();
				break;
		}
	}
}

int main() {
	std::cout << "Nothing in main so far..." << std::endl;

	char c;
	std::cin >> c;

	handle_input();

	return 0;
}
