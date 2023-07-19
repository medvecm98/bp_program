#include "mainwindow.h"
#include "./ui_mainwindow.h"

#define TAB_BROWSER 0
#define TAB_CHIEF_EDITOR 1
#define TAB_JOURNALIST 2
#define TAB_SETTINGS 3

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
	ctx->peer.print_contents();
}

void MainWindow::newspaper_added_to_db_noarg() {
	all_newspaper_updated();
	ctx->peer.allocate_next_newspaper();
}

void MainWindow::newspaper_added_to_db(pk_t news_id) {
	all_newspaper_updated();
	ctx->peer.generate_article_list_message(news_id);
	ctx->peer.allocate_next_newspaper();
}

void MainWindow::on_pushButton_add_news_released()
{
	subWindows["add_news"]->show();
}

void MainWindow::all_newspaper_updated() {
	ui->comboBox_news_select->clear();
	for (auto&& news : ctx->peer.get_news()) { //for all newspapers in database
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
		newspaper_updated(ui->comboBox_news_select->currentData().toULongLong()); //generate the article list for given newspaper
	}
}

void MainWindow::newspaper_updated(pk_t nid) {
	auto& news_the_one  = ctx->peer.get_news(nid); //find requested news in database

	std::multimap<my_string, Article&> categories;
	std::set<my_string> category_names;

	auto [bit, eit] = news_the_one.get_newest_articles(0, ctx->peer.get_article_list_sort_config());

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
	auto& news = ctx->peer.get_news(nid); //find requested new in database

	std::multimap<my_string, Article&> categories;
	std::set<my_string> category_names;

	auto [bit, eit] = news.get_newest_articles(0, ctx->peer.get_article_list_sort_config());

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

void MainWindow::article_list_create_category(pk_t nid, std::string category, QString pattern) {
	auto& news = ctx->peer.get_news(nid); //find requested new in database

	QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionMatch match;

	std::vector<std::reference_wrapper<Article>> category_articles;
	auto [bit, eit] = news.get_newest_articles(0, ctx->peer.get_article_list_sort_config());
	for (; bit != eit; bit++) {
		Article& article = news.get_article(bit->second);
		for (auto&& cat : article.categories_ref()) {
			if (category == "All categories" || cat == category) {
				if (pattern.trimmed().isEmpty() || regex.match(QString::fromStdString(article.heading())).hasMatch()) {
					category_articles.push_back(article);
					break;
				}
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
	if (ui->comboBox_news_select->currentData().toULongLong() == nid) {
		if (ui->comboBox_categories->currentText() == "All categories" && ui->lineEdit_search_articles->text().trimmed().isEmpty()) {
			article_list_create(nid);
		}
		else {
			article_list_create_category(
				nid,
				ui->comboBox_categories->currentText().toStdString(),
				ui->lineEdit_search_articles->text()
			);
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

void MainWindow::on_pushButton_set_ip_released()
{
	QString text_np2ps_port = ui->lineEdit_np2ps_port->text();
	QString text_stun_port = ui->lineEdit_stun_port->text();
	std::uint16_t np2ps_port = PORT;
	std::uint16_t stun_port = STUN_PORT;
	if (!text_np2ps_port.trimmed().isEmpty()) {
		np2ps_port = std::stoi(text_np2ps_port.toStdString());
	}
	if (!text_stun_port.trimmed().isEmpty()) {	
		stun_port = std::stoi(text_stun_port.toStdString());
	}
    emit start_server_np2ps(QHostAddress::AnyIPv4, np2ps_port); //starts the NP2PS server
    emit start_server_stun(QHostAddress::AnyIPv4, stun_port); //starts the STUN server
}

void MainWindow::enable_add_news(){
}

void MainWindow::enable_add_article() {
	ui->pushButton_add_article->setEnabled(true);
}

void MainWindow::enable_print_peer() {
	ui->pushButton_print_peer->setEnabled(true);
}

void MainWindow::disable_new_peer(){
}

void MainWindow::set_article_related_buttons(bool state) {
	ui->pushButton_add_margin->setEnabled(state);
	ui->pushButton_view_margin->setEnabled(state);
	ui->pushButton_delete_article->setEnabled(state);
	ui->pushButton->setEnabled(state);
}

void MainWindow::set_newspaper_related_buttons(bool state) {
}

void MainWindow::check_selected_item() {
}

void MainWindow::check_item(QTreeWidgetItem* item) {
	if (item->text(1) == "Article") { //is article selected?
		if (ctx->peer.get_downloading_articles().find(item->text(2).toULongLong()) != ctx->peer.get_downloading_articles().end()) { //check if there isn't already an article request pending
			set_article_related_buttons(false);
		}
		else {
			set_article_related_buttons(true);
		}
		set_newspaper_related_buttons(false);
	}
	else if (item->text(1) == "Newspaper") { //is newspaper selected?
		if (ctx->peer.get_getting_article_list().find(item->text(2).toULongLong()) != ctx->peer.get_getting_article_list().end()) { //check if there isn't already an article list request pending
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

}


void MainWindow::on_pushButton_addJournalist_clicked()
{
	std::size_t public_id = ui->lineEdit_addJournalist->text().toULongLong();
	ctx->peer.add_journalist(public_id);
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

}


void MainWindow::on_pushButton_view_margin_clicked()
{

	// if (ui->treeWidget_newspaper->selectedItems().size() == 0) { //no item was selected
	// 	std::cout << "Please, select one item, thank you." << std::endl;
	// 	return;
	// }
	// else if (ui->treeWidget_newspaper->selectedItems().size() > 1) { //more than one item was selected
	// 	std::cout << "Please, select only one item, thank you." << std::endl;
	// 	return;
	// }
	// else if (ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent() == nullptr || //its a newspaper
	// 		 ui->treeWidget_newspaper->selectedItems().begin().i->t()->parent()->parent() == nullptr) { //or its a category
	// 	std::cout << "Please, select an article, thank you." << std::endl; //we want an article
	// 	return;
	// }
	// else {
	// 	auto article_selected_hash = ui->treeWidget_newspaper->selectedItems().begin().i->t()->text(2).toULongLong();
	// 	std::cout << "Viewing margin for article: " << article_selected_hash << std::endl;

	// 	auto article = ctx->peer.find_article( article_selected_hash);

	// 	auto [mb, me] = article.value()->margins();

	// 	for (; mb != me; mb++) { //iterate thorugh all the margins for found article
	// 		ui->plainTextEdit_margins->appendPlainText(QString::number(mb->first).append(':'));
	// 		ui->plainTextEdit_margins->appendPlainText(QString::fromStdString(mb->second.type).append(':'));
	// 		ui->plainTextEdit_margins->appendPlainText(QString::fromStdString(mb->second.content).append('\n'));
	// 	}
	// }
	auto article_selected_hash = ui->listWidget_articles->currentItem()->data(Qt::UserRole).toULongLong();
    auto news_id = ui->comboBox_news_select->currentData().toULongLong();
	emit view_margin(article_selected_hash, news_id);
}

void MainWindow::on_pushButton_add_margin_clicked()
{
//    if (ui->listWidget_articles->currentItem()
//		&& ui->listWidget_articles->currentItem()->data(Qt::UserRole).toULongLong() == article
//		&& ui->comboBox_news_select->currentData().toULongLong() == news_id
//	) {
//		display_article(news_id, article);
//	}
    auto article_selected_hash = ui->listWidget_articles->currentItem()->data(Qt::UserRole).toULongLong();
    auto news_id = ui->comboBox_news_select->currentData().toULongLong();
    std::cout << "Adding margin for article: " << article_selected_hash << std::endl;
    auto& article = ctx->peer.get_news(news_id).get_article(article_selected_hash);
    emit add_margin(&article);
}

void MainWindow::new_margin(std::string type, std::string contents) {
	auto article_selected_hash = ui->listWidget_articles->currentItem()->data(Qt::UserRole).toULongLong();
    auto news_id = ui->comboBox_news_select->currentData().toULongLong();
    auto& article = ctx->peer.get_news(news_id).get_article(article_selected_hash);
	article.add_margin(ctx->peer.get_public_id(), Margin(type, contents)); //add the margin with provided type and contents
}

void MainWindow::on_pushButton_testPeer1_clicked()
{
	ctx->peer.set_name("OnePeer"); //set the name of the peer

	ctx->peer.init_newspaper("OneNews"); //initializes new newspaper

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
	a.initialize_article(categories, path.toStdString(), ctx->peer, ctx->peer.get_news_db().at(ctx->peer.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->peer.enroll_new_article(std::move(a), false); //insert new article into the map of my newspaper, including the article's contents

	path = "../data/ecb.md";
	categories.insert("economy");
	Article a2;
	a2.initialize_article(categories, path.toStdString(), ctx->peer, ctx->peer.get_news_db().at(ctx->peer.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->peer.enroll_new_article(std::move(a2), false); //insert new article into the map of my newspaper, including the article's contents

	path = "../data/lorem.txt";
	categories.clear();
	categories.insert("txt");
	Article a3;
	a3.initialize_article(categories, path.toStdString(), ctx->peer, ctx->peer.get_news_db().at(ctx->peer.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->peer.enroll_new_article(std::move(a3), false); //insert new article into the map of my newspaper, including the article's contents

	ui->pushButton_testPeer1->setEnabled(false);
	ui->pushButton_testPeer2->setEnabled(false);
}


void MainWindow::on_pushButton_testPeer2_clicked()
{
	ctx->peer.set_name("TwoPeer"); //set the name of the peer

	ctx->peer.init_newspaper("TwoNews"); //initializes new newspaper

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
	a.initialize_article(categories, path.toStdString(), ctx->peer, ctx->peer.get_news_db().at(ctx->peer.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->peer.enroll_new_article(std::move(a), false); //insert new article into the map of my newspaper, including the article's contents

	categories.clear();
	categories.insert("economy");
	path = "../data/kurenie.md";
	Article a2;
	a2.initialize_article(categories, path.toStdString(), ctx->peer, ctx->peer.get_news_db().at(ctx->peer.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->peer.enroll_new_article(std::move(a2), false); //insert new article into the map of my newspaper, including the article's contents

	categories.insert("politics");
	path = "../data/taliani.md";
	Article a3;
	a3.initialize_article(categories, path.toStdString(), ctx->peer, ctx->peer.get_news_db().at(ctx->peer.get_my_news_id())); //intializes the article (partly) using data from listWidget_categories
	ctx->peer.enroll_new_article(std::move(a3), false); //insert new article into the map of my newspaper, including the article's contents

	ui->pushButton_testPeer1->setEnabled(false);
	ui->pushButton_testPeer2->setEnabled(false);
}


void MainWindow::on_pushButton_save_clicked()
{
    np2ps::Peer serialized_peer;
    ctx->peer.serialize(&serialized_peer);

	// QString file_name = QStandardPaths::writableLocation(
	// 	QStandardPaths::AppDataLocation
	// ).append(
	// 	tr("/user_info.nppsa")
	// );

	QString file_name = QString("../data/save").append(QString::number(ctx->peer.get_public_id()));

    std::ofstream file(file_name.toStdString());
    serialized_peer.SerializeToOstream(&file);
}

void MainWindow::on_pushButton_load_clicked()
{
    std::filesystem::remove("./archive.txt");
}

void MainWindow::on_pushButton_loadFromFile_clicked()
{
    emit signal_add_new_newspaper_from_file(
		QStandardPaths::writableLocation(
			QStandardPaths::AppDataLocation
		).append(
			tr("/user_info.nppsa")
		)
	);
}

void MainWindow::on_pushButtonFriends_clicked()
{
	QString ip_string = ui->lineEditFriends->text();
	QString pid_string = ui->lineEditFriendID->text();
	bool pid_cast_ok;
	pk_t pid = pid_string.toULongLong(&pid_cast_ok);
	if (!ctx->peer.add_friend(pid, ip_string) || !pid_cast_ok) {
		throw other_error("Adding friend failed for some reason.");
	}
	ctx->peer.generate_newspaper_list_request(pid);
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
        	article_list_create(data.toULongLong());
		}
		else {
			article_list_create_category(
				data.toULongLong(),
				ui->comboBox_categories->currentText().toStdString(),
				ui->lineEdit_search_articles->text()
			);
		}
    }
}


void MainWindow::on_comboBox_news_select_activated(int index)
{
    QVariant data = ui->comboBox_news_select->currentData();
	NewspaperEntry& news = ctx->peer.get_news(data.toULongLong());
    if (index != -1) {
        newspaper_updated(data.toULongLong());
    }
	if (data.toULongLong() != ctx->peer.get_public_id()
		&& (news.last_updated() + 300000) > GlobalMethods::get_time_now()
	) {
		ctx->peer.generate_article_list_message(news.get_id());
	}
}

void MainWindow::checked_display_article(pk_t news_id, hash_t article)
{
	if (ui->listWidget_articles->currentItem()
		&& ui->listWidget_articles->currentItem()->data(Qt::UserRole).toULongLong() == article
		&& ui->comboBox_news_select->currentData().toULongLong() == news_id
	) {
		display_article(news_id, article);
	}
}


void MainWindow::display_article(pk_t news_id, hash_t article)
{
	NewspaperEntry& news = ctx->peer.get_news(news_id);
	Article& article_header = news.get_article(article);

	if (!article_header.article_present()) { //check if article contains its contents
		ctx->peer.generate_article_all_message(
			news.get_id(),
			article_header.main_hash()
		); //no, and so it needs to be requested
	}
	else {
		ui->textBrowser->clear();

		//article content is present and we may print it

		ui->textBrowser->clear();
		QString contents = QString::fromStdString(article_header.read_contents());
		article_header.set_read();

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
		ui->comboBox_news_select->currentData().toULongLong(),
		item->data(Qt::UserRole).toULongLong()
	);
	ui->pushButton_add_margin->setEnabled(true);
	ui->pushButton_view_margin->setEnabled(true);
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
	// ctx->peer.news
}

void MainWindow::on_pushButton_informCoworkers_clicked()
{
	ctx->peer.inform_coworkers();
}

void MainWindow::on_pushButton_gossip_clicked()
{
	ctx->peer.generate_gossip_one_way_all();
}

void MainWindow::on_toolButton_articleList_clicked()
{
	QVariant data = ui->comboBox_news_select->currentData();
	pk_t news_id = data.toULongLong();
	if (news_id != ctx->peer.get_public_id()) {
		NewspaperEntry& news = ctx->peer.get_news(news_id);
		if (ui->comboBox_news_select->currentIndex() != -1) {
			newspaper_updated(data.toULongLong());
		}
		if (data.toULongLong() != ctx->peer.get_public_id()) {
			ctx->peer.generate_article_list_message(news.get_id());
		}
	}
	ctx->peer.generate_newspaper_list_request_connected();
}

void MainWindow::set_config_from_app() {
	fill_spinboxes();
	disable_save_cancel_enable_news_select();
}

void MainWindow::fill_config_news() {
	ui->comboBox_newsConfigSelect->clear();
	for (auto&& [news_pid, news] : ctx->peer.get_news()) {
		ui->comboBox_newsConfigSelect->addItem(tr(news.get_name().c_str()), QVariant((qulonglong) news_pid));
	}
}

void MainWindow::fill_spinboxes() {
	ui->spinBox_gossipRandoms->setValue(ctx->peer.slot_get_config_peer_gossips());
	ui->spinBox_listSizeDefault->setValue(ctx->peer.slot_get_config_peer_article_list_default());
	ui->spinBox_listSizeFirst->setValue(ctx->peer.slot_get_config_peer_article_list_first());
	ui->spinBox_listSizeDefault_autodownload->setValue(ctx->peer.slot_get_config_peer_article_list_default_percent());
	ui->spinBox_listSizeFirst_autodownload->setValue(ctx->peer.slot_get_config_peer_article_list_first_percent());

	pk_t news_id = ui->comboBox_newsConfigSelect->currentData().toULongLong();

	ui->spinBox_readToKeep->setValue(ctx->peer.slot_get_config_news_no_read_articles(news_id));
	ui->spinBox_unreadToKeep->setValue(ctx->peer.slot_get_config_news_no_unread_articles(news_id));
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
	if (index == TAB_BROWSER) { // newspaper browser tab
		QVariant data = ui->comboBox_news_select->currentData();
        newspaper_updated(data.toULongLong());
	}
	if (index == TAB_CHIEF_EDITOR) { // chief editor tab
		fill_pending_journalists();
	}
	if (index == TAB_JOURNALIST) { //journalist tab
		fill_journalist_news();
		print_journalist_articles();
        ui->pushButton_edit_article->setEnabled(false);
		// ui->pushButton_request_journalism->setEnabled(false);
	}
	if (index == TAB_SETTINGS) { //settings tab
		fill_config_news();
		set_config_from_app();
	}
}

void MainWindow::on_comboBox_newsConfigSelect_activated(int index)
{
	set_config_from_app();
}

void MainWindow::on_pushButton_config_save_clicked()
{
	ctx->peer.slot_set_config_peer_article_list_default(ui->spinBox_listSizeDefault->value());
	ctx->peer.slot_set_config_peer_article_list_first(ui->spinBox_listSizeFirst->value());
	ctx->peer.slot_set_config_peer_gossips(ui->spinBox_gossipRandoms->value());
	ctx->peer.slot_set_config_peer_article_list_default_percent(ui->spinBox_listSizeDefault_autodownload->value());
	ctx->peer.slot_set_config_peer_article_list_first_percent(ui->spinBox_listSizeFirst_autodownload->value());
	pk_t news_id = ui->comboBox_newsConfigSelect->currentData().toULongLong();
	ctx->peer.slot_set_config_news_no_read_articles(news_id, ui->spinBox_readToKeep->value());
	ctx->peer.slot_set_config_news_no_unread_articles(news_id, ui->spinBox_unreadToKeep->value());
	disable_save_cancel_enable_news_select();
}

void MainWindow::enable_save_cancel_disable_news_select() {
	ui->comboBox_newsConfigSelect->setEnabled(false);
	ui->pushButton_config_save->setEnabled(true);
	ui->pushButton_config_cancel->setEnabled(true);
}

void MainWindow::disable_save_cancel_enable_news_select() {
	ui->comboBox_newsConfigSelect->setEnabled(true);
	ui->pushButton_config_save->setEnabled(false);
	ui->pushButton_config_cancel->setEnabled(false);
}

void MainWindow::on_spinBox_readToKeep_valueChanged(int arg1)
{
	enable_save_cancel_disable_news_select();
}

void MainWindow::on_spinBox_unreadToKeep_valueChanged(int arg1)
{
	enable_save_cancel_disable_news_select();
}


void MainWindow::on_spinBox_gossipRandoms_valueChanged(int arg1)
{
	enable_save_cancel_disable_news_select();
}


void MainWindow::on_spinBox_listSizeFirst_valueChanged(int arg1)
{
	enable_save_cancel_disable_news_select();
}


void MainWindow::on_spinBox_listSizeDefault_valueChanged(int arg1)
{
	enable_save_cancel_disable_news_select();
}

void MainWindow::on_pushButton_config_cancel_clicked()
{
	fill_spinboxes();
	disable_save_cancel_enable_news_select();
}

void MainWindow::fill_journalist_news() {
	ui->comboBox_news_journalist->clear();
	for (auto&& [news_id, news] : ctx->peer.get_news()) {
		ui->comboBox_news_journalist->addItem(news.get_name().c_str(), QVariant((qulonglong) news_id));
	}
}

void MainWindow::print_journalist_articles() {
	NewspaperEntry& news = ctx->peer.get_news(ui->comboBox_news_journalist->currentData().toULongLong());
	ui->listWidget_journalist_articles->clear();
	auto [articles, articles_end] = news.get_newest_articles(0, ctx->peer.get_article_list_sort_config());
	for (; articles != articles_end; articles++) {
		Article& article = news.get_article(articles->second);
		if (article.author_id() == ctx->peer.get_public_id()) {
			QListWidgetItem* item = new QListWidgetItem(article.heading().c_str());
			item->setData(Qt::UserRole, QVariant((qulonglong)article.main_hash()));
			ui->listWidget_journalist_articles->addItem(item);
		}
	}
}

void MainWindow::on_comboBox_news_journalist_currentIndexChanged(int index)
{
	if (index != -1) {
		print_journalist_articles();
		bool i_am_journalist = ctx->peer.get_journalist_of().count(ui->comboBox_news_journalist->currentData().toULongLong()) > 0;
		ui->pushButton_request_journalism->setEnabled(!i_am_journalist);
		ui->pushButton_add_article->setEnabled(i_am_journalist);
		ui->pushButton_delete_article->setEnabled(i_am_journalist);
	}
}

void MainWindow::on_pushButton_edit_article_clicked()
{
    pk_t news_id = ui->comboBox_news_journalist->currentData().toULongLong();
	hash_t article_hash = ui->listWidget_journalist_articles->currentItem()->data(Qt::UserRole).toULongLong();
	emit signal_edit_article(news_id, article_hash);
}

void MainWindow::on_listWidget_journalist_articles_currentRowChanged(int currentRow)
{
    ui->pushButton_edit_article->setEnabled(true);
}

void MainWindow::slot_article_updated() {
	print_journalist_articles();
}

void MainWindow::fill_pending_journalists() {
	auto& pending_journalists = ctx->peer.get_pending_journalists();
	auto rbegin_it_pending_journalists = pending_journalists.rbegin();
	auto rend_it_pending_journalists = pending_journalists.rend();

	ui->listWidget_pending_journalists->clear();
	for (; rbegin_it_pending_journalists != rend_it_pending_journalists; rbegin_it_pending_journalists++) {
		QString item_text = QString::fromStdString(rbegin_it_pending_journalists->first).append(" (").append(QString::number(rbegin_it_pending_journalists->second)).append(")");
		QListWidgetItem* list_item = new QListWidgetItem(item_text);
		list_item->setData(Qt::UserRole, QVariant((qulonglong)rbegin_it_pending_journalists->second));
		ui->listWidget_pending_journalists->addItem(list_item);
	}
}

void MainWindow::remove_pending_journalist(qulonglong pid) {
	auto& pending_journalists = ctx->peer.get_pending_journalists();
	auto rbegin_it_pending_journalists = pending_journalists.rbegin();
	auto rend_it_pending_journalists = pending_journalists.rend();

	for (; rbegin_it_pending_journalists != rend_it_pending_journalists; rbegin_it_pending_journalists++) {
		if (rbegin_it_pending_journalists->second == pid) {
			break;
		}
	}
	ctx->peer.get_pending_journalists().erase(rbegin_it_pending_journalists->first);
}

void MainWindow::on_pushButton_confirm_journalist_clicked()
{
	qulonglong journalist_id = ui->listWidget_pending_journalists->currentItem()->data(Qt::UserRole).toULongLong();
	ctx->peer.generate_new_journalist(journalist_id);
	remove_pending_journalist(journalist_id);
    fill_pending_journalists();
    ui->pushButton_confirm_journalist->setEnabled(false);
    ui->pushButton_remove_journalist->setEnabled(false);
}

void MainWindow::on_pushButton_remove_journalist_clicked()
{
	qulonglong journalist_id = ui->listWidget_pending_journalists->currentItem()->data(Qt::UserRole).toULongLong();
	remove_pending_journalist(journalist_id);
	fill_pending_journalists();
}

void MainWindow::on_pushButton_request_journalism_clicked()
{
    ctx->peer.generate_journalist_request(ui->comboBox_news_journalist->currentData().toULongLong());
}

void MainWindow::on_pushButton_export_my_news_clicked()
{
	QHostAddress address(ui->lineEdit_ip_export->text());
	QString file_name = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::homePath(), tr("NP2PS news entry (*.ne.npps)"));
	ctx->peer.save_news_to_file(file_name.toStdString(), ctx->peer.get_public_id(), address);
}

void MainWindow::on_toolButton_import_news_clicked()
{
	QString file_name = QFileDialog::getOpenFileName(this, tr("Load Newspaper Entry File"), QDir::homePath(), tr("NP2PS news entry (*.ne.npps)"));
    if (file_name.trimmed().size() > 0 && std::filesystem::is_regular_file(
		std::filesystem::path(file_name.toStdString())
	))
	{
        ctx->peer.load_news_from_file(file_name.toStdString());
    }
}

void MainWindow::on_spinBox_listSizeFirst_autodownload_valueChanged(int arg1)
{
    enable_save_cancel_disable_news_select();
}


void MainWindow::on_spinBox_listSizeDefault_autodownload_valueChanged(int arg1)
{
    enable_save_cancel_disable_news_select();
}

void MainWindow::new_peer_creation_cancelled() {
	QApplication::quit();
}

void MainWindow::save_peer() {
	np2ps::Peer serialized_peer;
    ctx->peer.serialize(&serialized_peer);

	QString file_name = QStandardPaths::writableLocation(
		QStandardPaths::AppDataLocation
	).append(
		tr("/user_info.nppsa")
	);

    std::ofstream file(file_name.toStdString());
    serialized_peer.SerializeToOstream(&file);
}

void MainWindow::load_peer() {
	QString archive_path = QStandardPaths::writableLocation(
			QStandardPaths::AppDataLocation
		).append(
			tr("/user_info.nppsa")
		);

	std::filesystem::path path(archive_path.toStdString());
	if (std::filesystem::exists(path)) {
		emit signal_add_new_newspaper_from_file(archive_path);
	}
}

void MainWindow::on_radioButton_sort_created_clicked()
{
	ctx->peer.set_article_list_sort_config(ArticleListSort::Created);
	article_list_regenerate(ui->comboBox_news_select->currentData().toULongLong());
}

void MainWindow::on_radioButton_sort_modified_clicked()
{
	ctx->peer.set_article_list_sort_config(ArticleListSort::Modified);
	article_list_regenerate(ui->comboBox_news_select->currentData().toULongLong());
}

void MainWindow::on_pushButton_2_clicked()
{
    quint64 whom_to_identify = ui->lineEdit_custom_identify->text().toULongLong();
    ctx->peer.get_networking()->get_stun_client()->identify(whom_to_identify);
}

void MainWindow::on_pushButton_search_articles_clicked()
{
    article_list_regenerate(
        ui->comboBox_news_select->currentData().toULongLong()
    );
}

void MainWindow::on_lineEdit_search_articles_textChanged(const QString &arg1)
{
    if (arg1.trimmed().isEmpty()) {
        article_list_regenerate(
            ui->comboBox_news_select->currentData().toULongLong()
        );
    }
}


void MainWindow::on_lineEdit_search_articles_returnPressed()
{
    article_list_regenerate(
        ui->comboBox_news_select->currentData().toULongLong()
    );
}

void MainWindow::slot_new_journalist_request(pk_t id, std::string name) {
	if (ui->tabWidget->currentIndex() == TAB_CHIEF_EDITOR) {
		fill_pending_journalists();
	}
}

void MainWindow::slot_journalism_approved(pk_t news_id) {
	if (ui->tabWidget->currentIndex() == TAB_JOURNALIST) {
		fill_journalist_news();
		print_journalist_articles();
		ui->pushButton_edit_article->setEnabled(false);
	}
}

void MainWindow::on_listWidget_pending_journalists_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    ui->pushButton_confirm_journalist->setEnabled(true);
    ui->pushButton_remove_journalist->setEnabled(true);
}


void MainWindow::on_pushButton_set_ip_clicked()
{

}
