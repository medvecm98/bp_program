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
}

void MainWindow::on_pushButton_add_news_released()
{
	subWindows["add_news"]->show();
}

void MainWindow::generate_article_list() {
	ui->treeWidget_newspaper->clear();
	for (auto&& news : ctx->p.get_news_db()) { //for all newspapers in database
		ui->treeWidget_newspaper->addTopLevelItem( //adds newspaper into Newspaper tree
				new QTreeWidgetItem(QStringList({
								QString::fromStdString(news.second.get_name()),
								"Newspaper",
								QString::number(news.second.get_id())
							}))
		);
		article_list_received(news.second.get_id()); //generate the article list
	}
	
}

void MainWindow::article_list_received(pk_t newspaper_id) {
	std::cout << "about to print article_list" << std::endl;
	auto news_the_one  = ctx->p.get_news_db().at(newspaper_id); //find requested new in database

	auto news_articles_it = news_the_one.get_iterator_database();
	auto news_atricles_it_end = news_the_one.get_iterator_database_end();
	
	std::set<my_string> categories;

	for (database_iterator_t it = news_articles_it; it != news_atricles_it_end; it++) { //create the union of all categories using std::set
		auto[cit, cite] = it->second.categories();
		for (; cit != cite; cit++) {
			categories.insert(*cit);
		}
	}

	QString news_name = QString::fromStdString(news_the_one.get_name());
	QString id_in_string = QString::number(news_the_one.get_id());
	
	QTreeWidgetItem* requseted_newspaper = nullptr;

	for (int i = 0; i < ui->treeWidget_newspaper->topLevelItemCount() && !requseted_newspaper; i++) { //find newspaper in the Newspaper tree
		if (ui->treeWidget_newspaper->topLevelItem(i)->text(2) == id_in_string) {
			requseted_newspaper = ui->treeWidget_newspaper->topLevelItem(i);
		}
	}

	bool category_found = false;
	if (requseted_newspaper) { //if we found newspaper in the newspaper tree
		for (auto&& category : categories) { //iterate thorugh all the categories
			for (int i = 0; i < requseted_newspaper->childCount(); i++) { //traverse thorugh all the children of the newspaper (those are categories)
				if (requseted_newspaper->child(i)->text(0) == category.c_str()) { //is the currently iterated category `category` already in the Newspaper tree?
					category_found = true; //found category we are looking for in Newspaper tree
					auto article = news_articles_it;
					for (; article != news_atricles_it_end; article++) { //traverse thorugh all articles
						bool article_found = false;
						for (int j = 0; j < requseted_newspaper->child(i)->childCount(); j++) { //traverse thorugh all the children of given category (those are articles)
							if (requseted_newspaper->child(i)->child(j)->text(2) == QString::number(article->second.main_hash())) { //is the currently iterated article `article` in the Newspaper tree already?
								article_found = true; //we found the article in the Newspaper tree
							}
						}
						if (!article_found && article->second.is_in_category(category)) { //article wasn't found in the Newspaper tree, but does it belong to the category?
							//yes, and so we will add it
							requseted_newspaper->child(i)->addChild( new QTreeWidgetItem(QStringList({article->second.heading().c_str(), "Article", QString::number(article->second.main_hash())})));
						}
						article_found = false; //resets the flag
					}
				}
			}
			if (!category_found) { //category wasn't found in the newspaper tree
				auto new_cat_tree = new QTreeWidgetItem(QStringList({category.c_str(), "Category", category.c_str()})); //we will add the category to the tree
				requseted_newspaper->addChild( new_cat_tree);
				auto article = news_articles_it;
					for (; article != news_atricles_it_end; article++) { //check all the articles...
						if (article->second.is_in_category(category)) //...if they belong in this newly inserted category
							new_cat_tree->addChild( new QTreeWidgetItem(QStringList({article->second.heading().c_str(), "Article", QString::number(article->second.main_hash())}))); //and if yes, add them
				}
			}
			category_found = false; //resets the flag
		}
	}
}

void MainWindow::on_pushButton_add_article_released()
{
	const char* env_p = std::getenv("HOME");
	auto fileName = QFileDialog::getOpenFileName(this, "Select files", env_p, "Text Files (*.txt);;Markdown (*.md)"); //open file selection dialog for plain text files and markdown
	if (!fileName.isNull()) {
		if (std::filesystem::is_regular_file(fileName.toStdString())) {
			emit add_new_article(fileName); //we selected a regular file and new article now may be added
		}
	}
}

void MainWindow::on_pushButton_article_list_released()
{
	std::cout << "Article list requested" << std::endl;
	QErrorMessage qem(this);
	if (ui->treeWidget_newspaper->selectedItems().size() == 0) { //no item was selected from Newspaper tree
		std::cout << "Please, select one item, thank you." << std::endl;
		qem.showMessage("Please, select one item, thank you.");
		return;
	}
	if (ui->treeWidget_newspaper->selectedItems().size() > 1) { //more than one item was selected from Newspaper tree
		std::cout << "Please, select only one item, thank you." << std::endl;
		qem.showMessage("Please, select only one item, thank you.");
		return;
	}
	if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() != nullptr) { //non-newspaper item was selected from Newspaper tree
		std::cout << "Non-newspaper item selected. Please select only top-level newspaper item from tree." << std::endl;
		qem.showMessage("Non-newspaper item selected. Please select only top-level newspaper item from tree.");
		return;
	}

	if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong() == ctx->p.get_public_key()) { //article list for our own newspaper doesn't make sense
		return;
	}

	ctx->p.generate_article_list(ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong()); //request the article list of the newspaper
}


void MainWindow::on_pushButton_set_ip_released()
{
	QHostAddress address(ui->comboBox_interfacs->currentText().split(':').last().trimmed()); //sets the IP from the comboBox
	emit start_server(address); //starts the STUN and NP2PS servers
}

void MainWindow::on_pushButton_external_article_released()
{
	if (ui->treeWidget_newspaper->selectedItems().size() == 0) { //no item was selected from Newspaper tree
		std::cout << "Please, select one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().size() > 1) { //more than one item was selected from Newspaper tree
		std::cout << "Please, select only one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr ||
			 ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr) {  //non-article item was selected from Newspaper tree
		std::cout << "Please, select an article, thank you." << std::endl;
		return;
	}
	else {
		/* gets an article from newspaper database */
		auto news_db_id = ctx->p.get_news_db().at(ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent()->text(2).toULongLong()).get_id();
		auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
		auto present_article = ctx->p.get_news_db().at(news_db_id).find_article_header(article_selected_hash);
		if (!present_article.has_value()) { //if article wasn't found, not even his header
			ctx->p.generate_article_all_message(news_db_id, article_selected_hash); //request an article
		}
		else {
			if (!present_article.value()->article_present()) { //check if article contains its contents
				ctx->p.generate_article_all_message(present_article.value()->author_id(), article_selected_hash); //no, and so it needs to be requested
			}
			else {
				ui->textEdit_article->clear();

				//article content is present and we may print it

				ui->textEdit_article->clear();
				QString contents = QString::fromStdString(present_article.value()->read_contents());

				switch (present_article.value()->get_format()) //sets the correct format for Article field
				{
				case article_format::Markdown:
					ui->textEdit_article->setMarkdown(contents);
					break;
				case article_format::Html:
					ui->textEdit_article->setHtml(contents);
					break;

				default:
					ui->textEdit_article->setPlainText(contents);
					break;
				}
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

void MainWindow::set_article_related_buttons(bool state) {
	ui->pushButton_add_margin->setEnabled(state);
	ui->pushButton_view_margin->setEnabled(state);
	ui->pushButton_external_article->setEnabled(state);
	ui->pushButton_delete_article->setEnabled(state);
	ui->pushButton->setEnabled(state);
}

void MainWindow::set_newspaper_related_buttons(bool state) {
	ui->pushButton_article_list->setEnabled(state);
}

void MainWindow::check_selected_item() {
	check_item(ui->treeWidget_newspaper->selectedItems()[0]);
}

void MainWindow::check_item(QTreeWidgetItem* item) {
	if (item->text(1) == "Article") { //is article selected?
		if (ctx->p.get_downloading_articles().find(item->text(2).toULongLong()) != ctx->p.get_downloading_articles().end()) { //check if there isn't already an article request pending
			set_article_related_buttons(false);
		}
		else {
			set_article_related_buttons(true);
		}
		set_newspaper_related_buttons(false);
	}
	else if (item->text(1) == "Newspaper") { //is newspaper selected?
		if (ctx->p.get_getting_article_list().find(item->text(2).toULongLong()) != ctx->p.get_getting_article_list().end()) { //check if there isn't already an article list request pending
			set_newspaper_related_buttons(false); //if yes, disable newspaper related buttons
		}
		else {
			set_newspaper_related_buttons(true); //if no, enable newspaper related buttons
		}
		set_article_related_buttons(false);
	}
	else { //if neither article nor newspaper is selected, all the buttons for either articles or newspaper will be disabled
		set_article_related_buttons(false);
		set_newspaper_related_buttons(false);
	}
}

void MainWindow::on_treeWidget_newspaper_itemClicked(QTreeWidgetItem *item, int column)
{
	check_item(item);
}

void MainWindow::on_pushButton_delete_article_clicked()
{
	if (ui->treeWidget_newspaper->selectedItems().size() > 0 && 
			 !(ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr ||
			   ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr)) {
		auto h = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
		pk_t news_id;
		if (ctx->p.remove_article(h, news_id)) {
			if (ctx->p.get_public_key() != news_id) {
				ctx->p.removed_external_article(h, news_id);
			}
			ui->treeWidget_newspaper->clear();
			generate_article_list(); //article is deleted from Newspaper tree when the tree is regenerated
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

void MainWindow::newspaper_created() {
	generate_article_list();
}

void MainWindow::got_network_interfaces(address_vec_ptr addresses_and_interfaces) {
	for (auto&& ai : *addresses_and_interfaces) { //append all interfaces to the comboBox_interfaces
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
		std::uint64_t margin_autor = ui->lineEdit_margin->text().toULongLong(); //gets the author of the margin from lineEdit
		std::cout << "Requesting margin for article: " << article_selected_hash << " and peer " << margin_autor << std::endl;
		ctx->p.create_margin_request(margin_autor, article_selected_hash); //send request to the author
	}

}


void MainWindow::on_pushButton_view_margin_clicked()
{

	if (ui->treeWidget_newspaper->selectedItems().size() == 0) { //no item was selected
		std::cout << "Please, select one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().size() > 1) { //more than one item was selected
		std::cout << "Please, select only one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr || //its a newspaper
			 ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr) { //or its a category
		std::cout << "Please, select an article, thank you." << std::endl; //we want an article
		return;
	}
	else {
		auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
		std::cout << "Viewing margin for article: " << article_selected_hash << std::endl;

		auto article = ctx->p.find_article( article_selected_hash);

		auto [mb, me] = article.value()->margins();

		for (; mb != me; mb++) { //iterate thorugh all the margins for found article
			ui->plainTextEdit_margins->appendPlainText(QString::number(mb->first).append(':'));
			ui->plainTextEdit_margins->appendPlainText(QString::fromStdString(mb->second.type).append(':'));
			ui->plainTextEdit_margins->appendPlainText(QString::fromStdString(mb->second.content).append('\n'));
		}
	}
}

void MainWindow::on_pushButton_add_margin_clicked()
{

	if (ui->treeWidget_newspaper->selectedItems().size() == 0) { //no item was selected
		std::cout << "Please, select one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().size() > 1) { //more than one item was selected
		std::cout << "Please, select only one item, thank you." << std::endl;
		return;
	}
	else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr || //its a newspaper
			 ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr) { //or its a category
		std::cout << "Please, select an article, thank you." << std::endl; //we want an article
		return;
	}
	else {
		auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong(); //gets article hash from Newspaper tree
		std::cout << "Adding margin for article: " << article_selected_hash << std::endl;
		auto article = ctx->p.find_article( article_selected_hash); //article is found in newspaper database and margin is appended
		if (article.has_value()) {
			emit add_margin(article.value());
		}
	}
}

void MainWindow::new_margin(std::string type, std::string contents) {
	auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong(); //gets article hash from Newspaper tree
	std::cout << "Adding margin for article: " << article_selected_hash << std::endl;
	auto article = ctx->p.find_article( article_selected_hash); //find article in newspaper database
	if (article.has_value()) { //if article was found
		article.value()->add_margin(ctx->p.get_public_key(), Margin(type, contents, ctx->p.get_public_key())); //add the margin with provided type and contents
	}
}
