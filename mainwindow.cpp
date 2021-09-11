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
	if (!ui->treeWidget_newspaper->topLevelItem(0)->parent())
		std::cout << "its nullptr" << std::endl;
}

void MainWindow::newspaper_added_to_db() {
	ui->treeWidget_newspaper->addTopLevelItem(
				new QTreeWidgetItem(QStringList({
								QString(ctx->p.recently_added->second.get_name().c_str()),
								"Newspaper",
								QString::number(ctx->p.recently_added->second.get_id())
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
	auto articles = news_the_one.get_list_of_articles();
	QString news_name = QString::fromStdString(news_the_one.get_name());
	QString id_in_string = QString::number(news_the_one.get_id());
	
	auto items = ui->treeWidget_newspaper->findItems(news_name, Qt::MatchContains);
	QTreeWidgetItem* requseted_newspaper = nullptr;

	for (int i = 0; i < ui->treeWidget_newspaper->topLevelItemCount() && !requseted_newspaper; i++) {
		std::cout << ui->treeWidget_newspaper->topLevelItem(i)->text(3).toStdString() << ' ' << id_in_string.toStdString() << '\n';
		if (ui->treeWidget_newspaper->topLevelItem(i)->text(2) == id_in_string) {
			requseted_newspaper = ui->treeWidget_newspaper->topLevelItem(i);
		}
	}

	bool category_found = false;
	if (requseted_newspaper) {
		for (auto&& category : articles.categories) {
			for (int i = 0; i < requseted_newspaper->childCount(); i++) {
				if (requseted_newspaper->child(i)->text(1) == category.c_str()) { //found category we are looking for
					category_found = true;
					for (auto&& article : articles.article_headers) {
						requseted_newspaper->child(i)->addChild( new QTreeWidgetItem(QStringList({article.heading().c_str(), "Article", QString::number(article.main_hash())})));
					}
				}
			}
			if (!category_found) {
				auto new_cat_tree = new QTreeWidgetItem(QStringList({category.c_str(), "Category", category.c_str()}));
				requseted_newspaper->addChild( new_cat_tree);
				for (auto&& article : articles.article_headers) {
					new_cat_tree->addChild( new QTreeWidgetItem(QStringList({article.heading().c_str(), "Article", QString::number(article.main_hash())})));
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
		pk_t news_id = ui->comboBox_newspapers->currentText().split(QLatin1Char(':'))[1].toULongLong();
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
	hash_t h = ui->listWidget_articles->selectedItems().begin().i->t()->text().split(':')[1].toULongLong();
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
	QErrorMessage qem;
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
	else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent()->parent() != nullptr) {
		std::cout << "Please, select article, thank you." << std::endl;
		return;
	}
	else {
		ctx->p.generate_article_all_message(ctx->p.get_news_db().at(ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent()->text(2).toULongLong()).get_id(), 
			ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong());
	}
}

