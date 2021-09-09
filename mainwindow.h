#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QErrorMessage>
#include "programcontext.h"
#include "form.h"
#include <memory>
#include <QMap>
#include <filesystem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	void setProgramContext(ProgramContext*);
	void addForm(QString, QWidget*);
	~MainWindow();

public slots:
	void newspaper_added_to_db();

private slots:
	void on_pushButton_new_peer_released();

	void on_pushButton_print_peer_released();

	void on_pushButton_add_news_released();

	void article_list_received(pk_t newspaper_id);

	void on_pushButton_add_article_released();

	void on_lineEdit_article_path_textEdited(const QString &arg1);

	void on_pushButton_preview_article_released();

	void on_pushButton_article_list_released();

private:
	Ui::MainWindow *ui;
	ProgramContext* ctx;
	QMap<QString, QWidget*> subWindows;
};
#endif // MAINWINDOW_H
