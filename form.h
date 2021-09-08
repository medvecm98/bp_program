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

private:
	Ui::Form *ui;
	ProgramContext *ctx;
};

#endif // FORM_H
