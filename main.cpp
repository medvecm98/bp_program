#include <iostream>
#include <filesystem>
#include <cstdlib>

#include "mainwindow.h"

#include <QApplication>
#include <QObject>
#include <QFile>
#include <filesystem>

#include "Article.h"
#include "Message.h"
#include "Peer.h"
#include "Networking.h"
#include "programcontext.h"
#include "form.h"
#include "add_newspaper.h"
#include "categoriesform.h"
#include "addmargin.h"

void load_articles_from_file(const std::string& path, std::shared_ptr<ProgramContext> ctx) {
	std::ifstream file(path.c_str());
	std::string line;
	ctx->p.peer_init("Test Peer", "Newspaper Test Peer");
	int counter = ctx->p.get_public_key() * 10;
	while (std::getline(file, line)) {
		std::string article_file_path(line);
		Article a;
		std::vector<std::string> v;
		a.initialize_article(v, article_file_path, ctx->p, ctx->p.get_news_db().at(ctx->p.get_my_news_id()), counter++);
		ctx->p.enroll_new_article(std::move(a), false);
	}
	file.close();
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w; //primary window

	/* initialize all the secondary windows */
	Form* f = new Form(); //New peer window
	add_newspaper* w_add_newspaper = new add_newspaper(); //Add newspaper window
	CategoriesForm* cf = new CategoriesForm(); //Add article window
	AddMargin* am = new AddMargin(); //Add margin window
	

	std::shared_ptr<ProgramContext> ctx; //program context that contains the Peer class

	bool loaded = false;
	if (QCoreApplication::arguments().size() > 1) {
		std::string path_string(QCoreApplication::arguments().at(1).toStdString());
		std::filesystem::path path(path_string);

		if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
			if (path_string[0] == '_') {
				pk_t custom_id = 123;
				if (QCoreApplication::arguments().size() > 2) {
					QString custom_id_string = QCoreApplication::arguments().at(2);
					custom_id = custom_id_string.toULongLong();
				}
				ctx = std::make_shared<ProgramContext>(custom_id);
				load_articles_from_file(path_string, ctx);
			}
			else {
				np2ps::Peer serialized_peer;
				std::ifstream ifile(path_string);
				serialized_peer.ParseFromIstream(&ifile);
				ctx = std::make_shared<ProgramContext>(serialized_peer);
			}
			loaded = true;
		}
	}
	else {
		ctx = std::make_shared<ProgramContext>();
	}
	
	w.setProgramContext(ctx.get()); //sets the program context for given window, for communication with peer
	
	/* Connect slots and signals in-between various widgets of the windows and windows and peer */
	QObject::connect(&ctx->p, &Peer::got_newspaper_confirmation, &w, &MainWindow::newspaper_added_to_db);
	QObject::connect(&ctx->p, &Peer::new_article_list, &w, &MainWindow::article_list_received);
	QObject::connect(ctx->p.get_networking(), &Networking::got_network_interfaces, &w, &MainWindow::got_network_interfaces);
	QObject::connect(&w, &MainWindow::start_server, ctx->p.get_networking()->get_peer_receiver(), &PeerReceiver::start_server);
	QObject::connect(&w, &MainWindow::start_server, ctx->p.get_networking()->get_stun_server(), &StunServer::start_server);
	QObject::connect(&ctx->p, &Peer::check_selected_item, &w, &MainWindow::check_selected_item);
	QObject::connect(&w, &MainWindow::signal_add_new_newspaper_from_file, &ctx->p, &Peer::slot_add_new_newspaper_from_file);
	QObject::connect(&w, &MainWindow::signal_add_new_newspaper_pk, &ctx->p, &Peer::slot_add_new_newspaper_pk);

	QObject::connect(f, &Form::enable_print_peer, &w, &MainWindow::enable_print_peer);
	QObject::connect(f, &Form::enable_add_article, &w, &MainWindow::enable_add_article);
	QObject::connect(f, &Form::enable_add_newspaper, &w, &MainWindow::enable_add_news);
	QObject::connect(f, &Form::disable_new_peer, &w, &MainWindow::disable_new_peer);
	QObject::connect(f, &Form::created_newspaper, &w, &MainWindow::newspaper_created);

	QObject::connect(&w, &MainWindow::add_margin, am, &AddMargin::show_this);
	QObject::connect(am, &AddMargin::new_margin, &w, &MainWindow::new_margin);
	QObject::connect(&w, &MainWindow::add_new_article, cf, &CategoriesForm::add_new_article);

	f->setProgramContext(ctx.get());
	w.addForm("new_peer", f);

	w_add_newspaper->setProgramContext(ctx.get());
	w.addForm("add_news", w_add_newspaper);

	cf->set_program_context(ctx.get());
	w.addForm("categories", cf);

	am->set_program_context(ctx.get());

	ctx->p.get_networking()->get_network_interfaces(); //gets networking interfaces for displaying them in the comboBox_networking

	if (loaded) {
		w.generate_article_list();
		w.enable_print_peer();
		w.enable_add_article();
		w.enable_add_news();
		w.disable_new_peer();
		w.newspaper_created();
	}

	w.show();

	return a.exec();
}
