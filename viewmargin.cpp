#include "viewmargin.h"
#include "ui_viewmargin.h"
#include <QTreeWidget>

ViewMargin::ViewMargin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewMargin)
{
    ui->setupUi(this);
}

ViewMargin::~ViewMargin()
{
    delete ui;
}

void ViewMargin::set_context(ProgramContext* ctx_) {
    ctx = ctx_;
}

void ViewMargin::display_margins(hash_t article_id_, pk_t news_id_) {
    NewspaperEntry& news = ctx->peer.get_news(news_id_);
    Article& article = news.get_article(article_id_);

    article_id = article_id_;
    news_id = news_id_;

    ui->treeWidget->clear();
    QStringList list;
    list.append("Type");
    list.append("Contents");
    ui->treeWidget->setHeaderLabels(list);

    QTreeWidgetItem* tree_widget_item;
    auto [margin_it, margin_eit] = article.margins();
    while (margin_it != margin_eit) {
        tree_widget_item = new QTreeWidgetItem();
        tree_widget_item->setText(0, QString::fromStdString(margin_it->second.type));
        tree_widget_item->setText(1, QString::fromStdString(margin_it->second.content));
        tree_widget_item->setData(0, Qt::UserRole, QVariant((qulonglong) margin_it->first));
        ui->treeWidget->addTopLevelItem(tree_widget_item);
        margin_it++;
    }
    ui->label->setText(tr("Displaying margins for article ").append(QString::fromStdString(article.heading())));
    show();
}

void ViewMargin::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    NewspaperEntry& news = ctx->peer.get_news(news_id);
    Article& article = news.get_article(article_id);
    qulonglong margin_id = ui->treeWidget->selectedItems().first()->data(0, Qt::UserRole).toULongLong();
    ui->textBrowser_view_margin->setPlainText(
        QString::fromStdString(article.get_margin(margin_id).content)
    );
}
