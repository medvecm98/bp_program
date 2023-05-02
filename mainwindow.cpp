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

void MainWindow::newspaper_added_to_db_noarg() {
	all_newspaper_updated();
	ctx->p.allocate_next_newspaper();
}

void MainWindow::newspaper_added_to_db(pk_t news_id) {
	all_newspaper_updated();
	ctx->p.allocate_next_newspaper();
}

void MainWindow::on_pushButton_add_news_released()
{
	subWindows["add_news"]->show();
}

void MainWindow::generate_article_list() {
	ui->treeWidget_newspaper->clear();
	ui->comboBox_news_select->clear();
	for (auto&& news : ctx->p.get_news()) { //for all newspapers in database
		if (news.second.await_confirmation && !news.second.confirmation()) { // skip news that require confirmation, but aren't confirmed
			continue;
		}
		QTreeWidgetItem* newspaper_tree_entry =
			new QTreeWidgetItem(
				QStringList({
					QString::fromStdString(news.second.get_name()),
					"Newspaper",
					QString::number(news.second.get_id())
				})
			);
		ui->comboBox_news_select->addItem(
			QString::fromStdString(news.second.get_name()),
			QVariant(QString::number(news.second.get_id()))
		);
		ui->treeWidget_newspaper->addTopLevelItem( //adds newspaper into Newspaper tree
				newspaper_tree_entry
		);
		article_list_received(news.second.get_id()); //generate the article list for given newspaper
	}
	
}

void MainWindow::article_list_received(pk_t newspaper_id) {
	auto& news_the_one  = ctx->p.get_news(newspaper_id); //find requested new in database
	
	std::multimap<my_string, Article&> categories;
	std::set<my_string> category_names;

	auto [bit, eit] = news_the_one.get_newest_articles(0);

	for (; bit != eit; bit++) {
		Article& article = news_the_one.get_article(bit->second);
		for (auto&& cat : article.categories_ref()) {
			categories.emplace(cat, article);
			category_names.emplace(cat);
		}
	}

	

	QString news_name = QString::fromStdString(news_the_one.get_name());
	QString id_in_string = QString::number(news_the_one.get_id());
	
	QTreeWidgetItem* requested_newspaper = nullptr;

	for (int i = 0; i < ui->treeWidget_newspaper->topLevelItemCount() && !requested_newspaper; i++) { //find newspaper in the Newspaper tree
		if (ui->treeWidget_newspaper->topLevelItem(i)->text(2) == id_in_string) {
			requested_newspaper = ui->treeWidget_newspaper->topLevelItem(i);
			break;
		}
	}

	if (requested_newspaper) {
		requested_newspaper->takeChildren();
		for (auto&& category : category_names) {
			auto category_tree = new QTreeWidgetItem(
				QStringList({
					QString::fromStdString(category),
					"Category",
					""
				})
			);

			auto [it, eit] = categories.equal_range(category);	
			for (; it != eit; it++) {
				Article& article = it->second;
				category_tree->addChild(
					new QTreeWidgetItem(
						QStringList(
							{
								QString::fromStdString(article.heading()),
								"Article",
								QString::number(article.main_hash())
							}
						)
					)
				);
			}

			requested_newspaper->addChild(category_tree);
		}
	}

	auto [it2, eit2] = categories.equal_range(ui->comboBox_categories->currentText().toStdString());	
	for (; it2 != eit2; it2++) {
		Article& article = it2->second;

		QListWidgetItem* list_item =
			new QListWidgetItem(
				QString::fromStdString(
					article.heading()
				)
			);

		list_item->setData(Qt::UserRole, QVariant((qulonglong)article.main_hash()));
		ui->listWidget_articles->addItem(list_item);
	}
}

void MainWindow::all_newspaper_updated() {
	ui->comboBox_news_select->clear();
	for (auto&& news : ctx->p.get_news()) { //for all newspapers in database
		if (news.second.await_confirmation && !news.second.confirmation()) { // skip news that require confirmation, but aren't confirmed
			continue;
		}
		ui->comboBox_news_select->addItem(
			QString::fromStdString(news.second.get_name()),
			QVariant((qulonglong)news.second.get_id())
		);
	}
	int current_index = ui->comboBox_news_select->currentIndex();
	if (ui->comboBox_news_select->currentIndex() != -1) {
		if (ui->comboBox_news_select->count() <= current_index) {
			current_index -= 1;
		}
		ui->comboBox_news_select->setCurrentIndex(current_index);
		newspaper_updated(ui->comboBox_news_select->currentData().toUInt()); //generate the article list for given newspaper
	}
}

void MainWindow::newspaper_updated(pk_t nid) {
	auto& news_the_one  = ctx->p.get_news(nid); //find requested news in database
	
	std::multimap<my_string, Article&> categories;
	std::set<my_string> category_names;

	auto [bit, eit] = news_the_one.get_newest_articles(0);

	for (; bit != eit; bit++) {
		Article& article = news_the_one.get_article(bit->second);
		for (auto&& cat : article.categories_ref()) {
			categories.emplace(cat, article);
			category_names.emplace(cat);
		}
	}

	ui->comboBox_categories->clear();
	ui->comboBox_categories->addItem(tr("All categories"));
	for (auto&& cn : category_names) {
		ui->comboBox_categories->addItem(QString::fromStdString(cn));
	}
	ui->comboBox_categories->setCurrentIndex(0);
	article_list_create(nid);
}

void MainWindow::article_list_create(pk_t nid) {
	auto& news = ctx->p.get_news(nid); //find requested new in database
	
	std::multimap<my_string, Article&> categories;
	std::set<my_string> category_names;

	auto [bit, eit] = news.get_newest_articles(0);

	ui->listWidget_articles->clear();
	for (; bit != eit; bit++) {
		Article& article = news.get_article(bit->second);

		QListWidgetItem* list_item =
			new QListWidgetItem(
				QString::fromStdString(
					article.heading()
				)
			);

		list_item->setData(Qt::UserRole, QVariant((qulonglong)article.main_hash()));
		ui->listWidget_articles->addItem(list_item);
	}
}

void MainWindow::article_list_create_category(pk_t nid, std::string category) {
	auto& news = ctx->p.get_news(nid); //find requested new in database
	std::vector<std::reference_wrapper<Article>> category_articles;
	auto [bit, eit] = news.get_newest_articles(0);
	for (; bit != eit; bit++) {
		Article& article = news.get_article(bit->second);
		for (auto&& cat : article.categories_ref()) {
			if (cat == category) {
				category_articles.push_back(article);
			}
		}
	}
	ui->listWidget_articles->clear();
	for (auto&& article : category_articles) {
		QListWidgetItem* list_item =
			new QListWidgetItem(
				QString::fromStdString(
					article.get().heading()
				)
			);

		list_item->setData(Qt::UserRole, QVariant((qulonglong)article.get().main_hash()));
		ui->listWidget_articles->addItem(list_item);
	}
}

void MainWindow::article_list_regenerate(pk_t nid) {
	if (ui->comboBox_news_select->currentData().toUInt() == nid) {
		if (ui->comboBox_categories->currentText() == "All categories") {
			article_list_create(nid);
		}
		else {
			article_list_create_category(nid, ui->comboBox_categories->currentText().toStdString());
		}
	}
}

void MainWindow::on_pushButton_add_article_released()
{
	const char* env_p = std::getenv("HOME");
	auto fileName = QFileDialog::getOpenFileName(this, "Select files", env_p, "Markdown (*.md);;Text Files (*.txt)"); //open file selection dialog for plain text files and markdown
	if (!fileName.isNull()) {
		if (std::filesystem::is_regular_file(fileName.toStdString())) {
			emit add_new_article(fileName); //we selected a regular file and new article now may be added
		}
	}
}

void MainWindow::on_pushButton_article_list_released()
{
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

	ctx->p.generate_article_list_message(ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong()); //request the article list of the newspaper
}


void MainWindow::on_pushButton_set_ip_released()
{
	QHostAddress address(ui->comboBox_interfacs->currentText().split(':').last().trimmed()); //sets the IP from the comboBox
	emit start_server(address); //starts the STUN and NP2PS servers
}

void MainWindow::check_on_article(
	NewspaperEntry& news, hash_t article_selected_hash, article_optional& article_header
) {
	if (!article_header.has_value()) { //if article wasn't found, not even his header
		ctx->p.generate_article_all_message(news.get_id(), article_selected_hash); //request an article
	}
	else {
		if (!article_header.value()->article_present()) { //check if article contains its contents
			ctx->p.generate_article_all_message(
				ctx->p.check_destination_valid(
					article_header.value()->author_id(),
					news.get_id()),
				article_selected_hash); //no, and so it needs to be requested
		}
		else {
			ui->textEdit_article->clear();

			//article content is present and we may print it

			ui->textEdit_article->clear();
			QString contents = QString::fromStdString(article_header.value()->read_contents());

			switch (article_header.value()->get_format()) //sets the correct format for Article field
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
        NewspaperEntry& news_db = ctx->p.get_news_db().at(
			ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent()->text(2).toULongLong()
		);
        hash_t article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
        article_optional present_article = ctx->p.get_news_db().at(news_db.get_id()).find_article_header(article_selected_hash);
        check_on_article(news_db, article_selected_hash, present_article);
	}
}

void MainWindow::on_treeWidget_newspaper_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (item->parent() != nullptr &&
		item->parent()->parent() != nullptr
	) {
		/* We have selected an article, it will be requested now. */
		NewspaperEntry& news_db = ctx->p.get_news(item->parent()->parent()->text(2).toULongLong());
		hash_t selected_article_hash = item->text(2).toULongLong();
		article_optional article_header = news_db.find_article_header(selected_article_hash);
		check_on_article(news_db, selected_article_hash, article_header);
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
	if (ui->treeWidget_newspaper && ui->treeWidget_newspaper->selectedItems().size() > 0) {
		check_item(ui->treeWidget_newspaper->selectedItems()[0]);
	}
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
			else {
				ctx->p.remove_reader(h, ctx->p.get_public_key());

			}
			ui->treeWidget_newspaper->clear();
			all_newspaper_updated(); //article is deleted from Newspaper tree when the tree is regenerated
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
	all_newspaper_updated();
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

void MainWindow::on_pushButton_testPeer1_clicked()
{
	ctx->p.set_name("OnePeer"); //set the name of the peer

	ctx->p.init_newspaper("OneNews"); //initializes new newspaper
	
	/* enables or disables all the buttons */
	enable_add_article();
	disable_new_peer();
	newspaper_created();
	
	enable_add_news();
	enable_print_peer();

	std::set<std::string> categories;
	categories.insert("politics");

	QString path("../data/heger.md");
	Article a;
	a.initialize_article(categories, path.toStdString(), ctx->p, ctx->p.get_news_db().at(ctx->p.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->p.enroll_new_article(std::move(a), false); //insert new article into the map of my newspaper, including the article's contents

	path = "../data/ecb.md";
	categories.insert("economy");
	Article a2;
	a2.initialize_article(categories, path.toStdString(), ctx->p, ctx->p.get_news_db().at(ctx->p.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->p.enroll_new_article(std::move(a2), false); //insert new article into the map of my newspaper, including the article's contents

	path = "../data/lorem.txt";
	categories.clear();
	categories.insert("txt");
	Article a3;
	a3.initialize_article(categories, path.toStdString(), ctx->p, ctx->p.get_news_db().at(ctx->p.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->p.enroll_new_article(std::move(a3), false); //insert new article into the map of my newspaper, including the article's contents

	ui->pushButton_testPeer1->setEnabled(false);
	ui->pushButton_testPeer2->setEnabled(false);
}


void MainWindow::on_pushButton_testPeer2_clicked()
{
	ctx->p.set_name("TwoPeer"); //set the name of the peer

	ctx->p.init_newspaper("TwoNews"); //initializes new newspaper
	
	/* enables or disables all the buttons */
	enable_add_article();
	disable_new_peer();
	newspaper_created();
	
	enable_add_news();
	enable_print_peer();

	std::set<std::string> categories;
	categories.insert("sport");
	QString path("../data/ferencvaros.md");
	Article a;
	a.initialize_article(categories, path.toStdString(), ctx->p, ctx->p.get_news_db().at(ctx->p.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->p.enroll_new_article(std::move(a), false); //insert new article into the map of my newspaper, including the article's contents

	categories.clear();
	categories.insert("economy");
	path = "../data/kurenie.md";
	Article a2;
	a2.initialize_article(categories, path.toStdString(), ctx->p, ctx->p.get_news_db().at(ctx->p.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->p.enroll_new_article(std::move(a2), false); //insert new article into the map of my newspaper, including the article's contents

	categories.insert("politics");
	path = "../data/taliani.md";
	Article a3;
	a3.initialize_article(categories, path.toStdString(), ctx->p, ctx->p.get_news_db().at(ctx->p.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->p.enroll_new_article(std::move(a3), false); //insert new article into the map of my newspaper, including the article's contents

	ui->pushButton_testPeer1->setEnabled(false);
	ui->pushButton_testPeer2->setEnabled(false);
}


void MainWindow::on_pushButton_save_clicked()
{
    np2ps::Peer serialized_peer;
    ctx->p.serialize(&serialized_peer);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(), tr("NP2PS archive (*.nppsa)"));

    std::ofstream file(fileName.toStdString());
    serialized_peer.SerializeToOstream(&file);
}

void MainWindow::on_pushButton_load_clicked()
{
    std::filesystem::remove("./archive.txt");
}

void MainWindow::on_pushButton_loadFromFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Newspaper File"), QDir::homePath(), tr("NP2PS newspaper (*.nppsn)"));
    emit signal_add_new_newspaper_from_file(fileName);
}

void MainWindow::on_pushButtonFriends_clicked()
{
	QString ip_string = ui->lineEditFriends->text();
	QString pid_string = ui->lineEditFriendID->text();
	bool pid_cast_ok;
	pk_t pid = pid_string.toULongLong(&pid_cast_ok);
	if (!ctx->p.add_friend(pid, ip_string) || !pid_cast_ok) {
		throw other_error("Adding friend failed for some reason.");
	}
	ctx->p.generate_newspaper_list_request(pid);
}

void MainWindow::on_comboBox_categories_currentIndexChanged(int index)
{

}

void MainWindow::on_comboBox_news_select_currentIndexChanged(int index)
{

}

void MainWindow::on_comboBox_categories_activated(int index)
{
    if (index != -1) {
        QVariant data = ui->comboBox_news_select->itemData(
            ui->comboBox_news_select->currentIndex()
        );
		if (ui->comboBox_categories->currentText() == "All categories") {
        	article_list_create(data.toUInt());
		}
		else {
			article_list_create_category(
				data.toUInt(),
				ui->comboBox_categories->currentText().toStdString()
			);
		}
    }
}


void MainWindow::on_comboBox_news_select_activated(int index)
{
    QVariant data = ui->comboBox_news_select->currentData();
	NewspaperEntry& news = ctx->p.get_news(data.toUInt());
    if (index != -1) {
        newspaper_updated(data.toUInt());
    }
	if (data.toUInt() != ctx->p.get_public_key()
		// && (news.last_updated() + 300000) > GlobalMethods::get_time_now()
	) {
		ctx->p.generate_article_list_message(news.get_id());
	}
}

void MainWindow::checked_display_article(pk_t news_id, hash_t article)
{
	if (ui->comboBox_news_select->currentData().toUInt() == news_id
		&& ui->listWidget_articles->currentItem()->data(Qt::UserRole).toUInt() == article
	) {
		display_article(news_id, article);
	}
}


void MainWindow::display_article(pk_t news_id, hash_t article)
{
	NewspaperEntry& news = ctx->p.get_news(news_id);
	Article& article_header = news.get_article(article);

	if (!article_header.article_present()) { //check if article contains its contents
		ctx->p.generate_article_all_message(
			ctx->p.check_destination_valid(
				article_header.author_id(),
				news.get_id()
			),
			article_header.main_hash()
		); //no, and so it needs to be requested
	}
	else {
		ui->textBrowser->clear();

		//article content is present and we may print it

		ui->textBrowser->clear();
		QString contents = QString::fromStdString(article_header.read_contents());

		switch (article_header.get_format()) //sets the correct format for Article field
		{
		case article_format::Markdown:
			ui->textBrowser->setMarkdown(contents);
			break;
		case article_format::Html:
			ui->textBrowser->setHtml(contents);
			break;

		default:
			ui->textBrowser->setPlainText(contents);
			break;
		}
	}
}

void MainWindow::on_listWidget_articles_itemClicked(QListWidgetItem *item)
{
	display_article(
		ui->comboBox_news_select->currentData().toUInt(),
		item->data(Qt::UserRole).toUInt()
	);
}

void MainWindow::on_pushButton_add_news_clicked()
{

}


void MainWindow::on_toolButton_addNewspaper_clicked()
{
    subWindows["add_news"]->show();
}


void MainWindow::on_toolButton_removeNewspaper_clicked()
{
	// ctx->p.news
}

void MainWindow::on_pushButton_informCoworkers_clicked()
{
	ctx->p.inform_coworkers();
}

void MainWindow::on_pushButton_gossip_clicked()
{
	ctx->p.generate_gossip_one_way();
}
