#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

void MainWindow::setProgramContext(ProgramContext* c) {
	ctx = c;
}

void MainWindow::addForm(QString name, QWidget* form) {
	subWindows.insert(name, form);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButton_new_peer_released()
{
	subWindows["new_peer"]->show();
}


void MainWindow::on_pushButton_print_peer_released()
{
	ctx->p.print_contents();
}

void MainWindow::newspaper_added_to_db(pk_t news_id) {
	auto name = ctx->p.get_news_db()[news_id].get_name().c_str();
	auto id = ctx->p.get_news_db()[news_id].get_id();
	generate_article_list();
	/*ui->treeWidget_newspaper->addTopLevelItem(
				new QTreeWidgetItem(QStringList({
								QString(name),
								"Newspaper",
								QString::number(id)
							}))
	);*/
}

void MainWindow::on_pushButton_add_news_released()
{
	subWindows["add_news"]->show();
}

void MainWindow::generate_article_list() {
	ui->treeWidget_newspaper->clear();
	for (auto&& news : ctx->p.get_news_db()) {
		ui->treeWidget_newspaper->addTopLevelItem(
				new QTreeWidgetItem(QStringList({
								QString::fromStdString(news.second.get_name()),
								"Newspaper",
								QString::number(news.second.get_id())
							}))
		);
		article_list_received(news.second.get_id());
	}
	
}

void MainWindow::article_list_received(pk_t newspaper_id) {
	std::cout << "about to print article_list" << std::endl;
	auto news_the_one  = ctx->p.get_news_db().at(newspaper_id);

	auto news_articles_it = news_the_one.get_iterator_database();
	auto news_atricles_it_end = news_the_one.get_iterator_database_end();
	
	std::set<my_string> categories;

	for (database_iterator_t it = news_articles_it; it != news_atricles_it_end; it++) {
		auto[cit, cite] = it->second.categories();
		for (; cit != cite; cit++) {
			categories.insert(*cit);
		}
	}

	QString news_name = QString::fromStdString(news_the_one.get_name());
	QString id_in_string = QString::number(news_the_one.get_id());
	
	//auto items = ui->treeWidget_newspaper->findItems(news_name, Qt::MatchContains);
	QTreeWidgetItem* requseted_newspaper = nullptr;

	for (int i = 0; i < ui->treeWidget_newspaper->topLevelItemCount() && !requseted_newspaper; i++) {
		if (ui->treeWidget_newspaper->topLevelItem(i)->text(2) == id_in_string) {
			requseted_newspaper = ui->treeWidget_newspaper->topLevelItem(i);
		}
	}

	/*for (auto&& news : ctx->p.get_news_db()) {
		ui->treeWidget_newspaper->addTopLevelItem(
				new QTreeWidgetItem(QStringList({
								QString::fromStdString(news.second.get_name()),
								"Newspaper",
								QString::number(news.second.get_id())
							}))
		);
	}*/


	bool category_found = false;
	if (requseted_newspaper) {
		for (auto&& category : categories) {
			for (int i = 0; i < requseted_newspaper->childCount(); i++) {
				if (requseted_newspaper->child(i)->text(0) == category.c_str()) { //found category we are looking for
					category_found = true;
					auto article = news_articles_it;
					for (; article != news_atricles_it_end; article++) {
						bool article_found = false;
						for (int j = 0; j < requseted_newspaper->child(i)->childCount(); j++) {
							if (requseted_newspaper->child(i)->child(j)->text(2) == QString::number(article->second.main_hash())) {
								article_found = true;
							}
						}
						if (!article_found && article->second.is_in_category(category)) {
							requseted_newspaper->child(i)->addChild( new QTreeWidgetItem(QStringList({article->second.heading().c_str(), "Article", QString::number(article->second.main_hash())})));
						}
						article_found = false;
					}
				}
			}
			if (!category_found) {
				auto new_cat_tree = new QTreeWidgetItem(QStringList({category.c_str(), "Category", category.c_str()}));
				requseted_newspaper->addChild( new_cat_tree);
				auto article = news_articles_it;
					for (; article != news_atricles_it_end; article++) {
						if (article->second.is_in_category(category))
							new_cat_tree->addChild( new QTreeWidgetItem(QStringList({article->second.heading().c_str(), "Article", QString::number(article->second.main_hash())})));
				}
			}
			category_found = false;
		}
	}
}

void MainWindow::on_pushButton_add_article_released()
{
	const char* env_p = std::getenv("HOME");
	auto fileName = QFileDialog::getOpenFileName(this, "Select files", env_p, "Text Files (*.txt);;Markdown (*.md)");
	if (!fileName.isNull()) {
		if (std::filesystem::is_regular_file(fileName.toStdString())) {
			emit add_new_article(fileName);
		}
	}

	//article_list_received(ctx->p.get_my_news_id());
}

void MainWindow::on_lineEdit_article_path_textEdited(const QString &arg1)
{

}


void MainWindow::on_pushButton_preview_article_released()
{

}


void MainWindow::on_pushButton_article_list_released()
{
	std::cout << "Article list requested" << std::endl;
	QErrorMessage qem(this);
	if (ui->treeWidget_newspaper->selectedItems().size() == 0) {
		std::cout << "Please, select one item, thank you." << std::endl;
		qem.showMessage("Please, select one item, thank you.");
		return;
	}
	if (ui->treeWidget_newspaper->selectedItems().size() > 1) {
		std::cout << "Please, select only one item, thank you." << std::endl;
		qem.showMessage("Please, select only one item, thank you.");
		return;
	}
	if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() != nullptr) {
		std::cout << "Non-newspaper item selected. Please select only top-level newspaper item from tree." << std::endl;
		qem.showMessage("Non-newspaper item selected. Please select only top-level newspaper item from tree.");
		return;
	}

	if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong() == ctx->p.get_public_key()) {
		return;
	}

	ctx->p.generate_article_list(ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong());
}


void MainWindow::on_pushButton_set_ip_released()
{
	QHostAddress address(ui->comboBox_interfacs->currentText().split(':').last().trimmed());
	emit start_server(address);
}


void MainWindow::on_pushButton_released()
{
	ui->textEdit_article->setMarkdown("# Hello");
}


void MainWindow::on_pushButton_external_article_released()
{
	if (ui->treeWidget_newspaper->selectedItems().size() == 0) {
		std::cout << "Please, select one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().size() > 1) {
		std::cout << "Please, select only one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr ||
			 ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr) {
		std::cout << "Please, select an article, thank you." << std::endl;
		return;
	}
	else {
		auto news_db_id = ctx->p.get_news_db().at(ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent()->text(2).toULongLong()).get_id();
		auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
		auto present_article = ctx->p.get_news_db().at(news_db_id).find_article_header(article_selected_hash);
		if (!present_article.has_value()) {
			ctx->p.generate_article_all_message(news_db_id, article_selected_hash);
		}
		else {
			if (!present_article.value()->article_present()) {
				ctx->p.generate_article_all_message(present_article.value()->author_id(), article_selected_hash);
			}
			else {
				ui->textEdit_article->clear();

				QString path(present_article.value()->get_path_to_file().c_str());
				QFile file;
				file.setFileName(path);
				file.open(QIODevice::ReadOnly);
				QTextStream text_stream(&file);
				QString line, contents;
				ui->textEdit_article->clear();
				while (!text_stream.atEnd()) {
					line = text_stream.readLine();
					contents.append(line);
				}
				auto file_extension = path.split('.').last();
				if (file_extension.toLower() == "md")
					ui->textEdit_article->setMarkdown(contents);
				else if (file_extension.toLower() == "html")
					ui->textEdit_article->setHtml(contents);
				else 
					ui->textEdit_article->setPlainText(contents);
				file.close();
			}
		}
	}
}

void MainWindow::enable_add_news(){
	ui->pushButton_add_news->setEnabled(true);
}

void MainWindow::enable_add_article() {
	ui->pushButton_add_article->setEnabled(true);
}

void MainWindow::enable_print_peer() {
	ui->pushButton_print_peer->setEnabled(true);
}

void MainWindow::disable_new_peer(){
	ui->pushButton_new_peer->setDisabled(true);
}

void MainWindow::on_treeWidget_newspaper_itemActivated(QTreeWidgetItem *item, int column)
{

}

void MainWindow::on_listWidget_articles_itemActivated(QListWidgetItem *item)
{

}

void MainWindow::on_pushButton_select_files_released()
{
	const char* env_p = std::getenv("HOME");
	auto fileName = QFileDialog::getOpenFileName(this, "Select files", env_p, "Text Files (*.txt);;Markdown (*.md)");
	if (!fileName.isNull()) {
		//on_pushButton_add_article_released();
	}
}

void MainWindow::on_lineEdit_article_path_textChanged(const QString &arg1)
{
		ui->pushButton_add_article->setEnabled(true);
}

void MainWindow::on_treeWidget_newspaper_itemClicked(QTreeWidgetItem *item, int column)
{

}

void MainWindow::on_listWidget_articles_itemClicked(QListWidgetItem *item)
{

}

void MainWindow::on_pushButton_save_released()
{
	std::ofstream ofs(GU::get_program_home().append("/settings.txt"));
	boost::archive::text_oarchive oa(ofs);
	oa << (*ctx);
}

void MainWindow::on_pushButton_load_released()
{



}


void MainWindow::on_pushButton_2_released()
{

}


void MainWindow::on_textEdit_article_cursorPositionChanged()
{
	QPoint qp;
	const QTextCursor tc = ui->textEdit_article->textCursor();
	const QTextBlock block = tc.block();
	std::cout << tc.blockNumber() << std::endl;
}


void MainWindow::on_stunButton_clicked()
{
	ctx->p.send_stun_binding_request();
}


void MainWindow::on_pushButton_3_clicked()
{
	ctx->p.stun_allocate();
}


void MainWindow::on_pushButton_delete_article_clicked()
{
	if (ui->treeWidget_newspaper->selectedItems().size() > 0 && 
			 !(ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr ||
			   ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr)) {
		auto h = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
		pk_t news_id;
		if (ctx->p.remove_article(h, news_id)) {
			//qDeleteAll(ui->treeWidget_newspaper->selectedItems());
			if (ctx->p.get_public_key() != news_id) {
				ctx->p.removed_external_article(h, news_id);
			}
			ui->treeWidget_newspaper->clear();
			generate_article_list();
		}
		else {
			ui->textEdit_article->setText("Invalid article hash or article was not found in database.");
		}
	}
	else {
		ui->textEdit_article->setText("Please, select an article.");
	}

	
}


void MainWindow::on_pushButton_addJournalist_clicked()
{
	std::size_t pk = ui->lineEdit_addJournalist->text().toULongLong();
	ctx->p.add_journalist(pk);
}

void MainWindow::newspaper_identified(pk_t id, my_string newspaper_name, std::string newspaper_ip_domain) {
	//ui->comboBox_newspapers->addItem(QString::fromStdString(newspaper_name).append(':').append(QString::number(id)));
	//ui->comboBox_newspapers->setEnabled(true);
}

void MainWindow::newspaper_created() {
	//ui->comboBox_newspapers->addItem(QString::fromStdString(ctx->p.get_my_news_name()).append(':').append(QString::number(ctx->p.get_my_news_id())));
	//ui->comboBox_newspapers->setEnabled(true);

	/*ui->treeWidget_newspaper->addTopLevelItem(
				new QTreeWidgetItem(QStringList({
								QString::fromStdString(ctx->p.get_my_news_name()),
								"Newspaper",
								QString::number(ctx->p.get_my_news_id())
							}))
	);*/
	generate_article_list();
}

void MainWindow::got_network_interfaces(address_vec_ptr addresses_and_interfaces) {
	std::cout << "AAAAAAAAAAAAAAAAA" << std::endl;
	for (auto&& ai : *addresses_and_interfaces) {
		ui->comboBox_interfacs->addItem(QString("Interface: ").append(ai.first).append(", address: ").append(ai.second.toString()));
	}
}

void MainWindow::on_pushButton_clicked()
{


	if (ui->treeWidget_newspaper->selectedItems().size() == 0) {
		std::cout << "Please, select one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().size() > 1) {
		std::cout << "Please, select only one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr ||
			 ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr) {
		std::cout << "Please, select an article, thank you." << std::endl;
		return;
	}
	else {
		auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
		std::uint64_t margin_autor = ui->lineEdit_margin->text().toULongLong();
		std::cout << "Requesting margin for article: " << article_selected_hash << " and peer " << margin_autor << std::endl;
		ctx->p.create_margin_request(margin_autor, article_selected_hash);
	}

}


void MainWindow::on_pushButton_2_clicked()
{

}


void MainWindow::on_pushButton_view_margin_clicked()
{

	if (ui->treeWidget_newspaper->selectedItems().size() == 0) {
		std::cout << "Please, select one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().size() > 1) {
		std::cout << "Please, select only one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr ||
			 ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr) {
		std::cout << "Please, select an article, thank you." << std::endl;
		return;
	}
	else {
		auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
		std::cout << "Viewing margin for article: " << article_selected_hash << std::endl;

		auto article = ctx->p.find_article( article_selected_hash);

		auto [mb, me] = article.value()->margins();

		for (; mb != me; mb++) {
			ui->plainTextEdit_margins->appendPlainText(QString::number(mb->first).append(':'));
			ui->plainTextEdit_margins->appendPlainText(QString::fromStdString(mb->second.type).append(':'));
			ui->plainTextEdit_margins->appendPlainText(QString::fromStdString(mb->second.content).append('\n'));
		}
	}
}


void MainWindow::on_pushButton_remove_margin_clicked()
{

}


void MainWindow::on_pushButton_add_margin_clicked()
{

	if (ui->treeWidget_newspaper->selectedItems().size() == 0) {
		std::cout << "Please, select one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().size() > 1) {
		std::cout << "Please, select only one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr ||
			 ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr) {
		std::cout << "Please, select an article, thank you." << std::endl;
		return;
	}
	else {
		auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
		std::cout << "Adding margin for article: " << article_selected_hash << std::endl;
		auto article = ctx->p.find_article( article_selected_hash);
		if (article.has_value()) {
			emit add_margin(article.value());
		}
	}
}

void MainWindow::new_margin(std::string type, std::string contents) {
	auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
	std::cout << "Adding margin for article: " << article_selected_hash << std::endl;
	auto article = ctx->p.find_article( article_selected_hash);
	if (article.has_value()) {
		article.value()->add_margin(ctx->p.get_public_key(), Margin(type, contents, ctx->p.get_public_key()));
	}
}