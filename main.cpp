#include <iostream>
#include <filesystem>
#include <cstdlib>

#include "mainwindow.h"

#include <QApplication>
#include <QObject>

#include "Article.h"
#include "ArticleDatabase.h"
#include "Message.h"
#include "Peer.h"
#include "Networking.h"
#include "openssl/rsa.h"
#include "programcontext.h"
#include "form.h"
#include "add_newspaper.h"


#define ARTICLE_DIR "./articles"
#define PEER_INFO "./peer_info.nfs"
#define FILE_HEADER "./file_header.nfs"

void print_help() {
	std::cout << "h for help" << std::endl;
}

/**
 * Finds all the missing files, that are necessary for correct behaviour.
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
			case 'c':
				
			case 'h':
			default:
				print_help();
				break;
		}
	}
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	Form* f = new Form();
	add_newspaper* w_add_newspaper = new add_newspaper();

	ProgramContext ctx;
	w.setProgramContext(&ctx);

	QObject::connect(w_add_newspaper, &add_newspaper::new_newspaper_in_db,
					 &w, &MainWindow::newspaper_added_to_db);

	f->setProgramContext(&ctx);
	w.addForm("new_peer", f);
	w_add_newspaper->setProgramContext(&ctx);
	w.addForm("add_news", w_add_newspaper);

	w.show();

	std::cout << "B" << std::endl;

	return a.exec();
}
