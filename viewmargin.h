#ifndef VIEWMARGIN_H
#define VIEWMARGIN_H

#include <QWidget>
#include "programcontext.h"
#include <QTreeWidgetItem>

namespace Ui {
class ViewMargin;
}

class ViewMargin : public QWidget
{
    Q_OBJECT

public:
    explicit ViewMargin(QWidget *parent = nullptr);
    ~ViewMargin();

    void set_context(ProgramContext* ctx_);

public slots:
    void display_margins(hash_t article_id_, pk_t news_id);

private slots:
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::ViewMargin *ui;
    hash_t article_id;
    pk_t news_id;
    ProgramContext* ctx;
};

#endif // VIEWMARGIN_H
