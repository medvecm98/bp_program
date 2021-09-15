#ifndef NEWARTICLEFORM_H
#define NEWARTICLEFORM_H

#include <QWidget>
#include "programcontext.h"
#include <QFileDialog>

namespace Ui {
class NewArticleForm;
}

class NewArticleForm : public QWidget
{
	Q_OBJECT

public:
	explicit NewArticleForm(QWidget *parent = nullptr);
	~NewArticleForm();
	void set_program_context(ProgramContext*);

private slots:
	void on_pushButton_file_path_released();

private:
	Ui::NewArticleForm *ui;
	ProgramContext* ctx;
};

#endif // NEWARTICLEFORM_H
