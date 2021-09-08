#include "add_newspaper.h"
#include "ui_add_newspaper.h"

add_newspaper::add_newspaper(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::add_newspaper)
{
	ui->setupUi(this);
}

add_newspaper::~add_newspaper()
{
	delete ui;
}

void add_newspaper::clear_all_lineEdit() {
	ui->lineEdit_public_id->clear();
	ui->lineEdit_name->clear();
	ui->lineEdit_ip->clear();
}

void add_newspaper::on_buttonBox_accepted() {
	ctx->p.add_new_newspaper(ui->lineEdit_public_id->text().toULongLong(), ui->lineEdit_name->text().toStdString(),
							 ui->lineEdit_ip->text().toStdString());
	clear_all_lineEdit();
	this->hide();
}

