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
	ui->lineEdit_np2ps_port->clear();
	ui->lineEdit_stun_port->clear();
}

bool check_ip_valid(const QString& ipAddress)
{
    QRegularExpression ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionMatch match = ipRegex.match(ipAddress);
    return match.hasMatch();
}

void add_newspaper::on_buttonBox_accepted() {
	auto text_address_port = ui->lineEdit_ip->text();
    auto text_address = ui->lineEdit_ip->text();
    auto text_port = ui->lineEdit_np2ps_port->text();
    auto text_stun_port = ui->lineEdit_stun_port->text();
	port_t np2ps_port = PORT;
	port_t stun_port = STUN_PORT;
	if (!text_port.trimmed().isEmpty()) {
		try {
			np2ps_port = (std::uint16_t)std::stoi(text_port.toStdString());
		}
		catch(std::invalid_argument) {}
	}
	if (!text_stun_port.trimmed().isEmpty()) {
		try {
			stun_port = (std::uint16_t)std::stoi(text_stun_port.toStdString());
		}
		catch(std::invalid_argument) {}
	}
	if (check_ip_valid(text_address)) {
		ctx->peer.identify_newspaper(
			QHostAddress(text_address),
            np2ps_port,
            stun_port,
			ui->lineEdit_name->text().toStdString()
		);
	}
	else {
		ctx->peer.identify_newspaper(
			text_address,
			ui->lineEdit_name->text().toStdString()
		);
	}

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
