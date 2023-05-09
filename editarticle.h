#ifndef EDITARTICLE_H
#define EDITARTICLE_H

#include <QWidget>
#include "programcontext.h"

namespace Ui {
class EditArticle;
}

class EditArticle : public QWidget
{
    Q_OBJECT

public:
    explicit EditArticle(QWidget *parent = nullptr);
    ~EditArticle();
    void set_program_context(ProgramContext* context);

public slots:
    void showa(qulonglong news_pid, qulonglong article_hash);

private slots:
    void on_plainTextEdit_article_edit_textChanged();

    void on_pushButton_save_article_clicked();

    void on_pushButton_cancel_article_clicked();

signals:
    void signal_article_updated(qulonglong news_pid, qulonglong article_hash, std::string file_path);

private:
    Ui::EditArticle *ui;
    ProgramContext *ctx; //program context
    qulonglong news_pid;
    qulonglong article_hash;
};

#endif // EDITARTICLE_H
