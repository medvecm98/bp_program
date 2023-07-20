#ifndef NEWSPAPERLISTVIEW_H
#define NEWSPAPERLISTVIEW_H

#include <QWidget>
#include "programcontext.h"
#include <QListWidgetItem>

namespace Ui {
class NewspaperListView;
}

class NewspaperListView : public QWidget
{
    Q_OBJECT

public:
    explicit NewspaperListView(QWidget *parent = nullptr);
    ~NewspaperListView();

    void set_context(ProgramContext* ctx_);

public slots:
    void show_this();

signals:
    void news_changed();
    void signal_new_news(pk_t news);
    void signal_removed_news(pk_t news);

private slots:
    void on_listWidget_available_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_listWidget_subscribed_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_pushButton_clicked();

private:
    void recreate_lists();
    Ui::NewspaperListView *ui;
    ProgramContext* ctx;
};

#endif // NEWSPAPERLISTVIEW_H
