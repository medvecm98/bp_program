#ifndef ADDMARGIN_H
#define ADDMARGIN_H

#include <QWidget>
#include "programcontext.h"

namespace Ui {
class AddMargin;
}

class AddMargin : public QWidget
{
	Q_OBJECT

public:
	explicit AddMargin(QWidget *parent = nullptr);
	void set_program_context(ProgramContext*);
	~AddMargin();

public slots:
	void show_this(article_ptr);

signals:
	void new_margin(std::string type, std::string contents);

private slots:
	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

private:
	Ui::AddMargin *ui;
	ProgramContext* ctx;
	article_ptr article;
};

#endif // ADDMARGIN_H
