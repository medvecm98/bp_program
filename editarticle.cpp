#include "editarticle.h"
#include "ui_editarticle.h"

EditArticle::EditArticle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditArticle)
{
    ui->setupUi(this);
}

EditArticle::~EditArticle()
{
    delete ui;
}

void EditArticle::set_program_context(ProgramContext* context) {
    ctx = context;
}

void EditArticle::showa(qulonglong news_pid, qulonglong article_hash) {
    Article& article = ctx->p.get_news(news_pid).get_article(article_hash);
    QString text = QString::fromStdString(article.read_contents());
    ui->plainTextEdit_article_edit->setPlainText(text);
    ui->textBrowser_article_browser->setMarkdown(text);
    this->show();
}

void EditArticle::on_plainTextEdit_article_edit_textChanged()
{
    ui->textBrowser_article_browser->setMarkdown(ui->plainTextEdit_article_edit->toPlainText());
}
