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

void EditArticle::showa(qulonglong news_pid_, qulonglong article_hash_) {
    news_pid = news_pid_;
    article_hash = article_hash_;
    Article& article = ctx->p.get_news(news_pid).get_article(article_hash);
    QString text = QString::fromStdString(article.read_contents());
    ui->plainTextEdit_article_edit->setPlainText(text);
    ui->textBrowser_article_browser->setMarkdown(text);
    ui->pushButton_save_article->setEnabled(false);
    this->show();
}

void EditArticle::on_plainTextEdit_article_edit_textChanged()
{
    ui->textBrowser_article_browser->setMarkdown(ui->plainTextEdit_article_edit->toPlainText());
    if (!ui->pushButton_save_article->isEnabled()) {
        ui->pushButton_save_article->setEnabled(true);
    }
}

void EditArticle::on_pushButton_save_article_clicked()
{
    QString dir_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QString root_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	QDir dir(dir_path);
	QDir rdir(root_path);
	if (!dir.exists()) {
		rdir.mkpath(dir.path());
	}
	dir_path += "/";
    QString file_name = "new_article_temp.md";
    file_name.prepend(dir_path);
    QFile file(file_name);
    file.open(QIODevice::ReadWrite | QIODevice::Truncate);
    QTextStream stream(&file);
    std::cout << ui->plainTextEdit_article_edit->toPlainText().toStdString() << std::endl;
    stream << ui->plainTextEdit_article_edit->toPlainText();
    file.close();
    this->close();
    emit signal_article_updated(news_pid, article_hash, file_name.toStdString());
}

void EditArticle::on_pushButton_cancel_article_clicked()
{
    ui->plainTextEdit_article_edit->clear();
    ui->textBrowser_article_browser->clear();
    this->close();
}

