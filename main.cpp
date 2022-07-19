#include <iostream>
#include <filesystem>
#include <cstdlib>

#include "mainwindow.h"

#include <QApplication>
#include <QObject>

#include "Article.h"
#include "Message.h"
#include "Peer.h"
#include "Networking.h"
#include "programcontext.h"
#include "form.h"
#include "add_newspaper.h"
#include "categoriesform.h"
#include "addmargin.h"

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w; //primary window

	/* initialize all the secondary windows */
	Form* f = new Form(); //New peer window
	add_newspaper* w_add_newspaper = new add_newspaper(); //Add newspaper window
	CategoriesForm* cf = new CategoriesForm(); //Add article window
	AddMargin* am = new AddMargin(); //Add margin window
	

	ProgramContext ctx; //program context that contains the Peer class
	w.setProgramContext(&ctx); //sets the program context for given window, for communication with peer

	/* Connect slots and signals in-between various widgets of the windows and windows and peer */
	QObject::connect(&ctx.p, &Peer::got_newspaper_confirmation, &w, &MainWindow::newspaper_added_to_db);
	QObject::connect(&ctx.p, &Peer::new_article_list, &w, &MainWindow::article_list_received);
	QObject::connect(ctx.p.get_networking(), &Networking::got_network_interfaces, &w, &MainWindow::got_network_interfaces);
	QObject::connect(&w, &MainWindow::start_server, ctx.p.get_networking()->get_peer_receiver(), &PeerReceiver::start_server);
	QObject::connect(&w, &MainWindow::start_server, ctx.p.get_networking()->get_stun_server(), &StunServer::start_server);
	QObject::connect(&ctx.p, &Peer::check_selected_item, &w, &MainWindow::check_selected_item);
	QObject::connect(f, &Form::enable_print_peer, &w, &MainWindow::enable_print_peer);
	QObject::connect(f, &Form::enable_add_article, &w, &MainWindow::enable_add_article);
	QObject::connect(f, &Form::enable_add_newspaper, &w, &MainWindow::enable_add_news);
	QObject::connect(f, &Form::disable_new_peer, &w, &MainWindow::disable_new_peer);
	QObject::connect(f, &Form::created_newspaper, &w, &MainWindow::newspaper_created);
	QObject::connect(&w, &MainWindow::add_margin, am, &AddMargin::show_this);
	QObject::connect(am, &AddMargin::new_margin, &w, &MainWindow::new_margin);
	QObject::connect(&w, &MainWindow::add_new_article, cf, &CategoriesForm::add_new_article);

	f->setProgramContext(&ctx);
	w.addForm("new_peer", f);

	w_add_newspaper->setProgramContext(&ctx);
	w.addForm("add_news", w_add_newspaper);

	cf->set_program_context(&ctx);
	w.addForm("categories", cf);

	am->set_program_context(&ctx);

	ctx.p.get_networking()->get_network_interfaces(); //gets networking interfaces for displaying them in the comboBox_networking

	w.show();

	return a.exec();
}
