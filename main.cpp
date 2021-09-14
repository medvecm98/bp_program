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

	QObject::connect(&ctx.p, &Peer::got_newspaper_confirmation,
					 &w, &MainWindow::newspaper_added_to_db);

	QObject::connect(&ctx.p, &Peer::new_article_list, &w, &MainWindow::article_list_received);

	QObject::connect(f, &Form::enable_print_peer, &w, &MainWindow::enable_print_peer);
	QObject::connect(f, &Form::enable_add_article, &w, &MainWindow::enable_add_article);
	QObject::connect(f, &Form::enable_add_newspaper, &w, &MainWindow::enable_add_news);
	QObject::connect(f, &Form::disable_new_peer, &w, &MainWindow::disable_new_peer);

	f->setProgramContext(&ctx);
	w.addForm("new_peer", f);
	w_add_newspaper->setProgramContext(&ctx);
	w.addForm("add_news", w_add_newspaper);

	w.show();

	std::cout << "B" << std::endl;

	return a.exec();
}
