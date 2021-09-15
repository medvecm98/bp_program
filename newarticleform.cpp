#include "newarticleform.h"
#include "ui_newarticleform.h"

NewArticleForm::NewArticleForm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::NewArticleForm)
{
	ui->setupUi(this);
}

NewArticleForm::~NewArticleForm()
{
	delete ui;
}

void NewArticleForm::set_program_context(ProgramContext* c) {
	ctx = c;
}

void NewArticleForm::on_pushButton_file_path_released()
{
	const char* env_p = std::getenv("HOME");
	auto fileName = QFileDialog::getOpenFileName(this, "Select files", env_p, "Text Files (*.txt);;Markdown (*.md)");
	if (!fileName.isNull()) {
		ui->lineEdit_file_path->clear();
		ui->lineEdit_file_path->setText(fileName);
		//on_pushButton_add_article_released();
	}
}

