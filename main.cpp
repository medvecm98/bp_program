#include <iostream>
#include <filesystem>
#include <cstdlib>

#include "mainwindow.h"

#include <QApplication>
#include <QObject>
#include <QFile>
#include <filesystem>

#include <chrono>
#include <thread>
#include <random>

#include "Article.h"
#include "Message.h"
#include "Peer.h"
#include "Networking.h"
#include "programcontext.h"
#include "form.h"
#include "add_newspaper.h"
#include "categoriesform.h"
#include "addmargin.h"
#include "editarticle.h"
#include "viewmargin.h"
#include "newspaperlistview.h"

void load_articles_from_file(const std::string& path, std::shared_ptr<ProgramContext> ctx) {
	std::ifstream file(path.c_str());
	std::string line;
	ctx->peer.peer_init("Test Peer", "Newspaper Test Peer");
	int counter = ctx->peer.get_public_id() * 10;
	while (std::getline(file, line)) { // loads articles from file, line after line
		std::stringstream input_sstream(line);
		std::string token;
		std::string article_file_path;

		if (std::getline(input_sstream, token, ' ')) {
			article_file_path = token;
		}

		if (!std::filesystem::is_regular_file(std::filesystem::path(article_file_path))) {
			std::cout << "Invalid file: " << article_file_path << std::endl;
			continue;
		}

		std::vector<std::string> v;
		while (std::getline(input_sstream, token, ' ')) {
			v.push_back(token);
		}

		std::random_device dev;
		std::mt19937 rng(dev());
    	std::uniform_int_distribution<std::mt19937::result_type> dist(1,1000000);

		Article a;
		a.initialize_article(v, article_file_path, ctx->peer, ctx->peer.get_news_db().at(ctx->peer.get_my_news_id()), 1, (counter++ + dist(rng)));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		ctx->peer.enroll_new_article(std::move(a), false);
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
	EditArticle* ea = new EditArticle();
	ViewMargin* vm = new ViewMargin();
	NewspaperListView* nlv = new NewspaperListView();

	std::shared_ptr<ProgramContext> ctx; //program context that contains the Peer class

	bool loaded = false;
	if (QCoreApplication::arguments().size() > 1) {
		std::string path_string(QCoreApplication::arguments().at(1).toStdString());
		std::filesystem::path path(path_string);

		std::cout << "FOUND PATH in arguments: " << path_string << std::endl;

		if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
			if (path.filename().string()[0] == '_') { // create new peer, for testing purposes
				pk_t custom_id = 123;
				if (QCoreApplication::arguments().size() > 2) {
					QString custom_id_string = QCoreApplication::arguments().at(2);
					custom_id = custom_id_string.toULongLong();
				}
				ctx = std::make_shared<ProgramContext>(custom_id);
				load_articles_from_file(path_string, ctx);
			}
			else { // load peer from save file
				std::cout << "Loading peer from save file" << std::endl;
				np2ps::Peer serialized_peer;
				std::ifstream ifile(path_string);
				serialized_peer.ParseFromIstream(&ifile);
				ctx = std::make_shared<ProgramContext>(serialized_peer);
				ctx->peer.ping_direct_peers();
			}
			loaded = true;
		}
	}
	else {
		ctx = std::make_shared<ProgramContext>();
	}
	
	w.setProgramContext(ctx.get()); //sets the program context for given window, for communication with peer
	
	/* Connect slots and signals in-between various widgets of the windows and windows and peer */
	QObject::connect(&ctx->peer, &Peer::got_newspaper_confirmation, &w, &MainWindow::newspaper_added_to_db);
	QObject::connect(&ctx->peer, &Peer::signal_newspaper_updated, &w, &MainWindow::newspaper_added_to_db_noarg);
	QObject::connect(&ctx->peer, &Peer::newspaper_list_received, nlv, &NewspaperListView::show_this);
	QObject::connect(&ctx->peer, &Peer::new_article_list, &w, &MainWindow::article_list_regenerate);
	QObject::connect(&ctx->peer, &Peer::checked_display_article, &w, &MainWindow::checked_display_article);
	QObject::connect(nlv, &NewspaperListView::signal_new_news, &ctx->peer, &Peer::slot_newspaper_from_list_added);
	QObject::connect(ctx->peer.get_networking(), &Networking::got_network_interfaces, &w, &MainWindow::got_network_interfaces);
	QObject::connect(&w, &MainWindow::start_server_np2ps, ctx->peer.get_networking()->get_peer_receiver(), &PeerReceiver::start_server);
	QObject::connect(&w, &MainWindow::start_server_stun, ctx->peer.get_networking()->get_stun_server(), &StunServer::start_server);
	QObject::connect(&ctx->peer, &Peer::check_selected_item, &w, &MainWindow::check_selected_item);
	QObject::connect(&w, &MainWindow::signal_add_new_newspaper_from_file, &ctx->peer, &Peer::slot_add_new_newspaper_from_file);
	QObject::connect(&w, &MainWindow::signal_add_new_newspaper_pk, &ctx->peer, &Peer::slot_add_new_newspaper_pk);

	QObject::connect(f, &Form::enable_print_peer, &w, &MainWindow::enable_print_peer);
	QObject::connect(f, &Form::enable_add_article, &w, &MainWindow::enable_add_article);
	QObject::connect(f, &Form::enable_add_newspaper, &w, &MainWindow::enable_add_news);
	QObject::connect(f, &Form::disable_new_peer, &w, &MainWindow::disable_new_peer);
	QObject::connect(f, &Form::created_newspaper, &w, &MainWindow::newspaper_created);
	QObject::connect(f, &Form::creation_cancelled, &w, &MainWindow::new_peer_creation_cancelled);

	QObject::connect(&w, &MainWindow::add_margin, am, &AddMargin::show_this);
	QObject::connect(&w, &MainWindow::view_margin, vm, &ViewMargin::display_margins);
	QObject::connect(am, &AddMargin::new_margin, &w, &MainWindow::new_margin);
	QObject::connect(&w, &MainWindow::add_new_article, cf, &CategoriesForm::add_new_article);

	QObject::connect(&w, &MainWindow::signal_edit_article, ea, &EditArticle::showa);
	QObject::connect(ea, &EditArticle::signal_article_updated, &ctx->peer, &Peer::update_article);
	QObject::connect(&ctx->peer, &Peer::signal_article_updated, &w, &MainWindow::slot_article_updated);
	QObject::connect(&a, &QApplication::lastWindowClosed, &w, &MainWindow::save_peer);

	QObject::connect(&ctx->peer, &Peer::signal_new_journalist_request, &w, &MainWindow::slot_new_journalist_request);
	QObject::connect(&ctx->peer, &Peer::signal_journalism_approved, &w, &MainWindow::slot_journalism_approved);

	f->setProgramContext(ctx.get());
	w.addForm("new_peer", f);

	w_add_newspaper->setProgramContext(ctx.get());
	w.addForm("add_news", w_add_newspaper);

	cf->set_program_context(ctx.get());
	w.addForm("categories", cf);

	am->set_program_context(ctx.get());
	ea->set_program_context(ctx.get());
	vm->set_context(ctx.get());
	nlv->set_context(ctx.get());

	ctx->peer.get_networking()->get_network_interfaces(); //gets networking interfaces for displaying them in the comboBox_networking

	if (loaded) {
		w.all_newspaper_updated();
		w.enable_print_peer();
		w.enable_add_article();
		w.enable_add_news();
		w.disable_new_peer();
		w.newspaper_created();
	}

	w.show();

	if (!loaded) {
		f->setWindowFlags(Qt::WindowStaysOnTopHint);
		f->setWindowModality(Qt::ApplicationModal);
		f->show();
	}

	return a.exec();
}
