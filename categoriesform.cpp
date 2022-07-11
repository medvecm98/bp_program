#include "categoriesform.h"
#include "ui_categoriesform.h"

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
	
	if (!category.trimmed().isEmpty()) {
		categories.insert(category.toStdString());
		ui->listWidget_categories->addItem(category);
	}
}


void CategoriesForm::on_pushButton_remove_category_clicked()
{
	QString category;
	for (auto&& list_cat : ui->listWidget_categories->selectedItems()) {
		category = list_cat->text();
		qDeleteAll(ui->listWidget_categories->selectedItems());
		//auto it = std::find(categories.begin(), categories.end(), category.toStdString());
		auto it = categories.find(category.toStdString());;
		categories.erase(it);
	}
}


void CategoriesForm::on_pushButton_accept_clicked()
{
	Article a;
	pk_t news_id = ui->comboBox_newspapers->currentText().split(':').last().trimmed().toULongLong();
	a.initialize_article(categories, path.toStdString(), ctx->p, ctx->p.get_news_db().at(news_id));
	if (news_id == ctx->p.get_public_key()) {
		ctx->p.enroll_new_article(std::move(a), false);
	}
	else {
		ctx->p.upload_external_article(a);
	}
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
	for (auto&& news : ctx->p.get_news_db()) {
		entry = QString("Newspaper; name: ").append(QString::fromStdString(news.second.get_name())).append(" with id: ").append(QString::number(news.second.get_id()));
		ui->comboBox_newspapers->addItem(entry);
	}
}

void CategoriesForm::add_new_article(QString path) {
	this->show();

	this->path = path;
}

void CategoriesForm::on_listWidget_categories_itemClicked(QListWidgetItem *item)
{

}


void CategoriesForm::on_listWidget_categories_itemSelectionChanged()
{
	if (ui->listWidget_categories->selectedItems().size() > 0) {
		ui->pushButton_remove_category->setEnabled(true);
	}
	else {
		ui->pushButton_remove_category->setEnabled(false);
	}
}

