#include "form.h"
#include "ui_form.h"

Form::Form(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Form)
{
	ui->setupUi(this);
}

Form::~Form()
{
	delete ui;
}

void Form::on_checkBox_create_newspaper_stateChanged(int arg1)
{
	if (ui->checkBox_create_newspaper->isChecked()) {
		ui->lineEdit_newspaper_name->setCursor(Qt::IBeamCursor);
		ui->lineEdit_newspaper_name->setEnabled(true);
	}
	else {
		ui->lineEdit_newspaper_name->setCursor(Qt::ForbiddenCursor);
		ui->lineEdit_newspaper_name->setEnabled(false);
	}
}

void Form::on_buttonBox_accepted()
{
	if (!ui->lineEdit_name->text().isEmpty()) {
		ctx->p.set_name(ui->lineEdit_name->text().toStdString());
	}
	if (ui->checkBox_create_newspaper->isChecked() && !ui->lineEdit_newspaper_name->text().isEmpty()) {
		ctx->p.init_newspaper(ui->lineEdit_newspaper_name->text().toStdString());
	}
	this->hide();
}

void Form::on_buttonBox_rejected()
{
    ui->lineEdit_name->clear();
    ui->lineEdit_newspaper_name->clear();
    ui->checkBox_create_newspaper->setChecked(false);
    this->hide();
}
