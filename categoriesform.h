#ifndef CATEGORIESFORM_H
#define CATEGORIESFORM_H

#include <QWidget>
#include <QListWidget>

#include <algorithm>

#include "programcontext.h"

namespace Ui {
class CategoriesForm;
}

class CategoriesForm : public QWidget
{
	Q_OBJECT

public:
	explicit CategoriesForm(QWidget *parent = nullptr);
	~CategoriesForm();
	std::set<std::string> categories;
	void set_program_context(ProgramContext* c) {
		ctx = c;
	}

public slots:
	void add_new_article(QString path);

private slots:
	void on_pushButton_add_category_clicked();

	void on_pushButton_remove_category_clicked();

	void on_pushButton_accept_clicked();

	void on_pushButton_cancel_clicked();

	void closeEvent(QCloseEvent *event) override;

	void showEvent(QShowEvent *event) override;

	void on_listWidget_categories_itemClicked(QListWidgetItem *item);

	void on_listWidget_categories_itemSelectionChanged();

private:
	Ui::CategoriesForm *ui;
	ProgramContext* ctx;
	QString path;
};

#endif // CATEGORIESFORM_H
