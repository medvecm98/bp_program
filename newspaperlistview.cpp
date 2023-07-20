#include "newspaperlistview.h"
#include "ui_newspaperlistview.h"

NewspaperListView::NewspaperListView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewspaperListView)
{
    ui->setupUi(this);
}

NewspaperListView::~NewspaperListView()
{
    delete ui;
}

void NewspaperListView::set_context(ProgramContext* ctx_) {
    ctx = ctx_;
}

void NewspaperListView::recreate_lists() {
    ui->pushButton->setEnabled(false);
    ui->listWidget_subscribed->clear();
    ui->listWidget_available->clear();

    auto& news_potential = ctx->peer.get_newspaper_potential();
    auto& news_current = ctx->peer.get_news_db();

    for (auto&& np : news_potential) {
        NewspaperEntry& entry = std::get<NewspaperEntry>(np.second);
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(entry.get_name()));
        item->setData(Qt::UserRole, QVariant((qulonglong) entry.get_id()));
        ui->listWidget_available->addItem(item);
    }

    for (auto&& nc : news_current) {
        NewspaperEntry& entry = nc.second;
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(entry.get_name()));
        item->setData(Qt::UserRole, QVariant((qulonglong) entry.get_id()));
        ui->listWidget_subscribed->addItem(item);
    }
}

void NewspaperListView::show_this() {
    recreate_lists();

    show();
}

void NewspaperListView::on_listWidget_available_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    ui->pushButton->setEnabled(true);
}


void NewspaperListView::on_listWidget_subscribed_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
}


void NewspaperListView::on_pushButton_clicked()
{
    auto item = ui->listWidget_available->currentItem();
    pk_t news_id = item->data(Qt::UserRole).toULongLong();
    emit signal_new_news(news_id);
    recreate_lists();
}
