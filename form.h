#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "programcontext.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
	Q_OBJECT

public:
	explicit Form(QWidget *parent = nullptr);
	void setProgramContext(ProgramContext* c) {
		ctx = c;
	}
	~Form();

private slots:
	void on_checkBox_create_newspaper_stateChanged(int arg1);

	void on_buttonBox_accepted();

    void on_buttonBox_rejected();

signals:
	void enable_add_article();
	void enable_add_newspaper();
	void enable_print_peer();
	void disable_new_peer();
	void created_newspaper();

private:
	Ui::Form *ui;
	ProgramContext *ctx;
};

#endif // FORM_H
