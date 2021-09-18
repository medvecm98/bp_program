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

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/shared_ptr_helper.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/utility.hpp>

#include <boost/serialization/shared_ptr_helper.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/tracking.hpp>

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

private:
	Ui::MainWindow *ui;
	ProgramContext* ctx;
	QMap<QString, QWidget*> subWindows;
};
#endif // MAINWINDOW_H
