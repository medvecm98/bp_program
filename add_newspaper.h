#ifndef ADD_NEWSPAPER_H
#define ADD_NEWSPAPER_H

#include <QWidget>
#include <QTreeWidget>
#include "programcontext.h"

namespace Ui {
class add_newspaper;
}

class add_newspaper : public QWidget
{
	Q_OBJECT

public:
	explicit add_newspaper(QWidget *parent = nullptr);
	void setProgramContext(ProgramContext *c) {
		ctx = c;
	}
	~add_newspaper();

signals:
	void new_newspaper_in_db();

private slots:
	void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
	Ui::add_newspaper *ui;
	ProgramContext *ctx;

	void clear_all_lineEdit();
};

#endif // ADD_NEWSPAPER_H
