#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QErrorMessage>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QFileDialog>
#include "programcontext.h"
#include "form.h"
#include <memory>
#include <QMap>
#include <filesystem>
#include <fstream>
#include "GlobalUsing.h"
#include <QTextBlock>

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
	void newspaper_added_to_db(pk_t news_id);
	void article_list_received(pk_t newspaper_id);
	void enable_add_article();
	void enable_add_news();
	void enable_print_peer();
	void disable_new_peer();
	void newspaper_identified(pk_t id, my_string newspaper_name, std::string newspaper_ip_domain);
	void newspaper_created();
	void got_network_interfaces(address_vec_ptr addresses_and_interfaces);
	void new_margin(std::string type, std::string contents);
	void check_selected_item();

private slots:
	void on_pushButton_new_peer_released();

	void on_pushButton_print_peer_released();

	void on_pushButton_add_news_released();


	void on_pushButton_add_article_released();

	void on_lineEdit_article_path_textEdited(const QString &arg1);

	void on_pushButton_preview_article_released();

	void on_pushButton_article_list_released();

	void on_pushButton_set_ip_released();

	void on_pushButton_released();

	void on_pushButton_external_article_released();

	void on_treeWidget_newspaper_itemActivated(QTreeWidgetItem *item, int column);

	void on_listWidget_articles_itemActivated(QListWidgetItem *item);

	void on_pushButton_select_files_released();

	void on_lineEdit_article_path_textChanged(const QString &arg1);

	void on_treeWidget_newspaper_itemClicked(QTreeWidgetItem *item, int column);

	void on_listWidget_articles_itemClicked(QListWidgetItem *item);

	void on_pushButton_save_released();

	void on_pushButton_load_released();

	void on_pushButton_2_released();

	void on_textEdit_article_cursorPositionChanged();

	void on_stunButton_clicked();

	void on_pushButton_3_clicked();

	void on_pushButton_delete_article_clicked();

	void on_pushButton_addJournalist_clicked();

	void on_pushButton_clicked();

	void on_pushButton_2_clicked();

	void on_pushButton_view_margin_clicked();

	void on_pushButton_remove_margin_clicked();

	void on_pushButton_add_margin_clicked();

signals:
	void add_new_article(QString path);

	void start_server(QHostAddress address);

	void add_margin(article_ptr article);

private:
	void check_item(QTreeWidgetItem* item);
	void generate_article_list();
	void set_article_related_buttons(bool state);
	void set_newspaper_related_buttons(bool state);

	Ui::MainWindow *ui;
	ProgramContext* ctx;
	QMap<QString, QWidget*> subWindows;
};
#endif // MAINWINDOW_H
