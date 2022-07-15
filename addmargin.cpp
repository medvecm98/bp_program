#include "addmargin.h"
#include "ui_addmargin.h"

AddMargin::AddMargin(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AddMargin)
{
	ui->setupUi(this);
}

AddMargin::~AddMargin()
{
	delete ui;
}

void AddMargin::show_this(article_ptr a) {
	this->show();
	article = a;
}

void AddMargin::on_buttonBox_accepted()
{
	this->hide();
	emit new_margin(ui->lineEdit->text().toStdString(), ui->plainTextEdit->toPlainText().toStdString());
}


void AddMargin::on_buttonBox_rejected()
{
	ui->lineEdit->clear();
	ui->plainTextEdit->clear();
	this->close();
}

void AddMargin::set_program_context(ProgramContext* c) {
	ctx = c;
}