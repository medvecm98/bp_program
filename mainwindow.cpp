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
	ui->treeWidget_newspaper->addTopLevelItem(
				new QTreeWidgetItem(QStringList({
								QString(ctx->p.get_news_db()[news_id].get_name().c_str()),
								"Newspaper",
								QString::number(ctx->p.get_news_db()[news_id].get_id())
							}))
	);
}

void MainWindow::on_pushButton_add_news_released()
{
	subWindows["add_news"]->show();
}

void MainWindow::article_list_received(pk_t newspaper_id) {
	std::cout << "about to print article_list" << std::endl;
	auto news_the_one  = ctx->p.get_news_db().at(newspaper_id);
	auto& articles = news_the_one.get_list_of_articles();
	QString news_name = QString::fromStdString(news_the_one.get_name());
	QString id_in_string = QString::number(news_the_one.get_id());
	
	//auto items = ui->treeWidget_newspaper->findItems(news_name, Qt::MatchContains);
	QTreeWidgetItem* requseted_newspaper = nullptr;

	for (int i = 0; i < ui->treeWidget_newspaper->topLevelItemCount() && !requseted_newspaper; i++) {
		if (ui->treeWidget_newspaper->topLevelItem(i)->text(2) == id_in_string) {
			requseted_newspaper = ui->treeWidget_newspaper->topLevelItem(i);
		}
	}

	bool category_found = false;
	if (requseted_newspaper) {
		for (auto&& category : articles.categories) {
			for (int i = 0; i < requseted_newspaper->childCount(); i++) {
				if (requseted_newspaper->child(i)->text(0) == category.c_str()) { //found category we are looking for
					category_found = true;
					for (auto&& article : articles.article_headers) {
						bool article_found = false;
						for (int j = 0; j < requseted_newspaper->child(i)->childCount(); j++) {
							if (requseted_newspaper->child(i)->child(j)->text(2) == QString::number(article.second.main_hash())) {
								article_found = true;
							}
						}
						if (!article_found) {
							requseted_newspaper->child(i)->addChild( new QTreeWidgetItem(QStringList({article.second.heading().c_str(), "Article", QString::number(article.second.main_hash())})));
						}
						article_found = false;
					}
				}
			}
			if (!category_found) {
				auto new_cat_tree = new QTreeWidgetItem(QStringList({category.c_str(), "Category", category.c_str()}));
				requseted_newspaper->addChild( new_cat_tree);
				for (auto&& article : articles.article_headers) {
					new_cat_tree->addChild( new QTreeWidgetItem(QStringList({article.second.heading().c_str(), "Article", QString::number(article.second.main_hash())})));
				}
			}
			category_found = false;
		}
	}
}
void MainWindow::on_pushButton_add_article_released()
{
	Article a;
	if (std::filesystem::is_regular_file(ui->lineEdit_article_path->text().trimmed().toStdString())) {
		pk_t news_id = ui->comboBox_newspapers->currentText().split(QLatin1Char(':')).last().toULongLong();
		a.initialize_article(std::vector<my_string>(), ui->lineEdit_article_path->text().trimmed().toStdString(), ctx->p, ctx->p.get_news_db().at(news_id));

		ui->listWidget_articles->addItem(QString(a.heading().c_str()).append(':').append(QString::number(a.main_hash())));
		ctx->p.enroll_new_article(std::move(a));
	}
	else {
		ui->textEdit_article->setText(QString("Invalid article path"));
	}
}

void MainWindow::on_lineEdit_article_path_textEdited(const QString &arg1)
{
	if (ui->comboBox_newspapers->count() == 0) {
		auto news_db = ctx->p.get_news_db();
		for (auto&& news : news_db) {
			ui->comboBox_newspapers->addItem(QString(news.second.get_name().c_str()).append(':').append(QString::number(news.second.get_id())));
		}
		ui->comboBox_newspapers->setEnabled(true);
		ui->pushButton_add_article->setEnabled(true);
	}
}


void MainWindow::on_pushButton_preview_article_released()
{
	hash_t h = ui->listWidget_articles->selectedItems().begin().i->t()->text().split(':').last().toULongLong();
	auto a = ctx->p.find_article(h);
	if (a.has_value()) {
		auto article = a.value();
		QString path(article->get_path_to_file().c_str());
		QFile file;
		file.setFileName(path);
		file.open(QIODevice::ReadOnly);
		QTextStream text_stream(&file);
		QString line;
		ui->textEdit_article->clear();
		while (!text_stream.atEnd()) {
			line = text_stream.readLine();
			ui->textEdit_article->append(line);
		}
		file.close();
	}
	else {
		ui->textEdit_article->setText("Invalid article hash");
	}
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

	ctx->p.generate_article_list(ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong());
}


void MainWindow::on_pushButton_set_ip_released()
{
	ctx->p.set_my_ip(ui->lineEdit_set_ip->text());
	ctx->p.restart_server();
}


void MainWindow::on_pushButton_released()
{

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
		article_optional present_article = ctx->p.get_news_db().at(news_db_id).find_article_header(article_selected_hash);
		if (!present_article.has_value()) {
			ctx->p.generate_article_all_message(news_db_id, article_selected_hash);
		}
		else {
			ui->textEdit_article->clear();

			QString path(present_article.value()->get_path_to_file().c_str());
			QFile file;
			file.setFileName(path);
			file.open(QIODevice::ReadOnly);
			QTextStream text_stream(&file);
			QString line;
			ui->textEdit_article->clear();
			while (!text_stream.atEnd()) {
				line = text_stream.readLine();
				ui->textEdit_article->append(line);
			}
			file.close();
		}
	}
}

void MainWindow::enable_add_news(){
	ui->pushButton_add_news->setEnabled(true);
}

void MainWindow::enable_add_article() {
	ui->pushButton_add_article->setEnabled(true);
	ui->lineEdit_article_path->setEnabled(true);
	ui->pushButton_select_files->setEnabled(true);
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
		ui->lineEdit_article_path->clear();
		ui->lineEdit_article_path->setText(fileName);
		//on_pushButton_add_article_released();
	}
}

void MainWindow::on_lineEdit_article_path_textChanged(const QString &arg1)
{
	if (ui->comboBox_newspapers->count() == 0) {
		auto news_db = ctx->p.get_news_db();
		for (auto&& news : news_db) {
			ui->comboBox_newspapers->addItem(QString(news.second.get_name().c_str()).append(':').append(QString::number(news.second.get_id())));
		}
		ui->comboBox_newspapers->setEnabled(true);
		ui->pushButton_add_article->setEnabled(true);
	}
}

void MainWindow::on_treeWidget_newspaper_itemClicked(QTreeWidgetItem *item, int column)
{

}

void MainWindow::on_listWidget_articles_itemClicked(QListWidgetItem *item)
{
	ui->pushButton_preview_article->setEnabled(true);
}

void MainWindow::on_pushButton_save_released()
{
	std::ofstream ofs(GU::get_program_home().append("/settings.txt"));
	boost::archive::text_oarchive oa(ofs);
	oa << (*ctx);
}

void MainWindow::on_pushButton_load_released()
{
	//enable/disable buttons
	std::ifstream ifs(GU::get_program_home().append("/settings.txt"));
	boost::archive::text_iarchive ia(ifs);
	ia >> (*ctx);
	if (!ctx->p.get_name().empty()) {
		if (!ctx->p.get_my_news_name().empty()) {
			ui->pushButton_new_peer->setEnabled(false);
			ui->pushButton_add_article->setEnabled(true);
			ui->pushButton_select_files->setEnabled(true);
			ui->lineEdit_article_path->setEnabled(true);
		}
		ui->pushButton_add_news->setEnabled(true);
		ui->pushButton_print_peer->setEnabled(true);
	}

	//tree
	ui->treeWidget_newspaper->clear();
	for (auto&& news : ctx->p.get_news_db()) {
		if (news.second.get_id() != ctx->p.get_my_news_id()) {
			QTreeWidgetItem* new_item = new QTreeWidgetItem(QStringList({QString::fromStdString(news.second.get_name()), "Newspaper", QString::number(news.second.get_id())}));
			ui->treeWidget_newspaper->addTopLevelItem(new_item);
			for (auto&& category : news.second.get_list_of_articles().categories) {
				QTreeWidgetItem* new_category = new QTreeWidgetItem(
					QStringList(
						{
							QString::fromStdString(category), "Category", QString::fromStdString(category)
						}
					)
				);
				new_item->addChild(new_category);
				for (auto&& article : news.second.get_list_of_articles().article_headers) {
					if (article.second.is_in_category(category)) {
						new_category->addChild(new QTreeWidgetItem(
							QStringList(
								{
									QString::fromStdString(article.second.heading()), "Article", QString::number(article.second.main_hash()) 
								}
							)
						));
					}
				}
			}
		}
	}

	//list of my articles
	if (!ctx->p.get_my_news_name().empty()) {
		auto& news_mine = ctx->p.get_news_db().at(ctx->p.get_my_news_id());
		for (auto ab = news_mine.get_const_iterator_database(); ab != news_mine.get_const_iterator_database_end(); ab++) {
			ui->listWidget_articles->addItem(
				new QListWidgetItem(
					QString::fromStdString(ab->second.heading()).append(':').append(QString::number(ab->second.main_hash()))
				)
			);
		}
	}


}

