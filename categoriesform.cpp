#include "categoriesform.h"
#include "ui_categoriesform.h"

/**
 * @brief Construct a new Categories Form:: Categories Form object.
 * 
 * Generated by QtCreator.
 */
CategoriesForm::CategoriesForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CategoriesForm)
{
	ui->setupUi(this);
	categories.clear();
	ui->pushButton_remove_category->setEnabled(false);
}

CategoriesForm::~CategoriesForm()
{
	delete ui;
}

void CategoriesForm::on_pushButton_add_category_clicked()
{
	QString category = ui->lineEdit_category->text();
	
	if (!category.trimmed().isEmpty()) { //if category isn't just whitespace or nothing at all
		categories.insert(category.toStdString());
		ui->listWidget_categories->addItem(category);
	}
}


void CategoriesForm::on_pushButton_remove_category_clicked()
{
	QString category;
	for (auto&& list_cat : ui->listWidget_categories->selectedItems()) {
		category = list_cat->text();
		qDeleteAll(ui->listWidget_categories->selectedItems()); //remove categories from listWidget_categories
		auto it = categories.find(category.toStdString()); //remove categories from set
		categories.erase(it);
	}
}


void CategoriesForm::on_pushButton_accept_clicked()
{
	Article a;
	pk_t news_id = ui->comboBox_newspapers->currentText().split(':').last().trimmed().toULongLong(); //ID of news that this article will be inserted into
	a.initialize_article(categories, path.toStdString(), ctx->peer, ctx->peer.get_news_db().at(news_id)); //intializes the article (partly) using data from listWidget_categories
	ctx->peer.upload_external_article(a); //upload article to newspaper readers and other (chief) editors
	ctx->peer.enroll_new_article(std::move(a), false); //insert new article into the map of my newspaper, including the article's contents
	this->close();
}


void CategoriesForm::on_pushButton_cancel_clicked()
{
	this->close();
}

void CategoriesForm::closeEvent(QCloseEvent *event) {
	categories.clear();
	ui->listWidget_categories->clear();
	path.clear();
	ui->lineEdit_category->clear();
}

void CategoriesForm::showEvent(QShowEvent *event) {
	ui->comboBox_newspapers->clear();

	QString entry;
	for (auto&& news : ctx->peer.get_news_db()) {
		//loop fills the comboBox_newspapers with all newspaper that peer either created,
		//... or ever subscribed to

		entry = QString("Newspaper; name: ").append(QString::fromStdString(news.second.get_name())).append(" with id: ").append(QString::number(news.second.get_id()));
		ui->comboBox_newspapers->addItem(entry); //adds to comboBox_newspapers
	}
}

void CategoriesForm::add_new_article(QString path) {
	this->show();

	this->path = path;
}

void CategoriesForm::on_listWidget_categories_itemSelectionChanged()
{
	if (ui->listWidget_categories->selectedItems().size() > 0) { //at least one category is selected
		ui->pushButton_remove_category->setEnabled(true);
	}
	else {
		ui->pushButton_remove_category->setEnabled(false);
	}
}
