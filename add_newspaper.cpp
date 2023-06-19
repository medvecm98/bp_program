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
	ui->lineEdit_name->clear();
	ui->lineEdit_ip->clear();
}

void add_newspaper::on_buttonBox_accepted() {
	ctx->peer.identify_newspaper(QHostAddress(ui->lineEdit_ip->text()), ui->lineEdit_name->text().toStdString());

	clear_all_lineEdit();
	this->hide();
}

/**
 * @brief Action to carry on, when user cancels all the input he made into the window.
 * 
 * Function will only clear all lineEdit-s.
 */
void add_newspaper::on_buttonBox_rejected()
{
    clear_all_lineEdit();
    this->hide();
}
