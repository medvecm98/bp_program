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

/**
 * @brief Class for primary window of the application.
 * 
 * Handles most of the communication of user with backend of the application.
 * Displays articles and Newspaper tree.
 * 
 * Provides means for adding articles, margins, journalists, setting IP, viewing
 * articles either directly or via other, secondary windows.
 * 
 * Newspaper tree is another, more user friendly name for treeWidget_newspaper.
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	/**
	 * @brief Construct a new MainWindow object.
	 * 
	 * Generated via QtCreator.
	 */
	MainWindow(QWidget *parent = nullptr);

	/**
	 * @brief Sets the windows's program context.
	 */
	void setProgramContext(ProgramContext* ctx);

	/**
	 * @brief Add form to map of `subWindows`.
	 * 
	 * Adds pointer to the secondary window to the map of secondary windows, in
	 * order to show them when button is clicked or some other event will happen
	 * 
	 * Later on I used slots and signals for that, but this was working, so I
	 * left it here.
	 */
	void addForm(QString, QWidget*);

	/**
	 * @brief Destroy the MainWindow object
	 * 
	 * Generated via QtCreator.
	 */
	~MainWindow();

	/**
	 * @brief Generates article list for all newspapers in database.
	 * 
	 */
	void generate_article_list();

	void all_newspaper_updated();
	void newspaper_updated(pk_t nid);
	void article_list_regenerate(pk_t nid);
	void article_list_create(pk_t nid);
	void article_list_create_category(pk_t nid, std::string category);
	void display_article(pk_t news_id, hash_t article);
	void checked_display_article(pk_t news_id, hash_t article);

	void check_on_article(NewspaperEntry& news, hash_t article_selected_hash, article_optional& article_header);

public slots:
	/**
	 * @brief New newspaper was added to database.
	 * 
	 * Refreshes the Newspaper tree.
	 * 
	 * @param news_id Public identifier of newly added newspaper.
	 */
	void newspaper_added_to_db_noarg();

	/**
	 * @brief New newspaper was added to database.
	 * 
	 * Refreshes the Newspaper tree.
	 * 
	 * @param news_id Public identifier of newly added newspaper.
	 */
	void newspaper_added_to_db(pk_t news_id);

	/**
	 * @brief Enable Add article button.
	 * 
	 */
	void enable_add_article();

	/**
	 * @brief Enable Add news button.
	 * 
	 */
	void enable_add_news();

	/**
	 * @brief Enable Print peer button.
	 * 
	 */
	void enable_print_peer();

	/**
	 * @brief Disable New peer button.
	 * 
	 */
	void disable_new_peer();

	/**
	 * @brief New newspaper were created.
	 * 
	 * Refresh the Newspaper tree.
	 */
	void newspaper_created();

	/**
	 * @brief Network interfaces from Networking are available.
	 * 
	 * @param addresses_and_interfaces Pointer to vector of all interfaces and their addresses.
	 */
	void got_network_interfaces(address_vec_ptr addresses_and_interfaces);

	/**
	 * @brief New margin was created via Add margin window.
	 * 
	 * Margin is inserted to ist corresponding article.
	 * 
	 * @param type Type of margin.
	 * @param contents Contents of margin.
	 */
	void new_margin(std::string type, std::string contents);

	/**
	 * @brief Checks the selected item in Newspaper tree and enables, disables 
	 * buttons based on its state.
	 * 
	 * While the article or article list is being downloaded, Preview article
	 * or Get article list are disabled, respectively. Same goes with other 
	 * buttons that are related to either article or newspaper.
	 */
	void check_selected_item();

	void enable_save_cancel_disable_news_select();
	void disable_save_cancel_enable_news_select();
	void fill_spinboxes();
	void slot_article_updated();
	void fill_pending_journalists();
	void remove_pending_journalist(qulonglong pid);
	void new_peer_creation_cancelled();
	void save_peer();
	void load_peer();

private slots:
    void on_pushButton_loadFromFile_clicked();

private slots:
    void on_pushButton_load_clicked();

private slots:
    void on_pushButton_save_clicked();

private slots:
	/**
	 * @brief Shows the New peer window.
	 * 
	 */
	void on_pushButton_new_peer_released();

	/**
	 * @brief Print some basic information about peer into console.
	 * 
	 */
	void on_pushButton_print_peer_released();

	/**
	 * @brief Opens the Add news window.
	 * 
	 */
	void on_pushButton_add_news_released();

	/**
	 * @brief Opens the Open file dialog.
	 * 
	 * After file is selected and confirmed, Add article window will open.
	 * 
	 */
	void on_pushButton_add_article_released();

	/**
	 * @brief Sets the IP based on selection made in comboBox_interfaces.
	 * 
	 */
	void on_pushButton_set_ip_released();

	/**
	 * @brief Calls the `check_item` function for item that was selected in the
	 * Newspaper tree.
	 * 
	 * @param item Item selected in Newspaper tree
	 */
	void on_treeWidget_newspaper_itemClicked(QTreeWidgetItem *item, int column);

	/**
	 * @brief Deletes article from database. Article entry is kept in the
	 * `_readers` list.
	 * 
	 * Sends the message about article removal to chief editor of newspaper
	 * that published this article.
	 */
	void on_pushButton_delete_article_clicked();

	/**
	 * @brief Adds a public identifier read from `lineEdit_addJournalist` into
	 * the `journalists_` list.
	 * 
	 */
	void on_pushButton_addJournalist_clicked();

	/**
	 * @brief Requests margins for selected article.
	 * 
	 * If more than one article is selected, or none, error message is printed
	 * into console.
	 */
	void on_pushButton_clicked();

	/**
	 * @brief Displays margins of selected article into Margins `plainTextEdit`.
	 * 
	 * Article is selected in Newspaper tree.
	 */
	void on_pushButton_view_margin_clicked();

	/**
	 * @brief Displays the Add margin window.
	 * 
	 * If more than one article is selected, or none, error message is printed
	 * into console.
	 * 
	 */
	void on_pushButton_add_margin_clicked();

	void on_pushButton_testPeer1_clicked();

	void on_pushButton_testPeer2_clicked();

    void on_pushButtonFriends_clicked();

    void on_comboBox_categories_currentIndexChanged(int index);

    void on_comboBox_news_select_currentIndexChanged(int index);

    void on_comboBox_categories_activated(int index);

    void on_comboBox_news_select_activated(int index);

    void on_listWidget_articles_itemClicked(QListWidgetItem *item);

    void on_pushButton_add_news_clicked();

    void on_toolButton_addNewspaper_clicked();

    void on_toolButton_removeNewspaper_clicked();

    void on_pushButton_informCoworkers_clicked();

    void on_pushButton_gossip_clicked();

    void on_toolButton_articleList_clicked();

	void set_config_from_app();

    void on_tabWidget_currentChanged(int index);

    void on_comboBox_newsConfigSelect_activated(int index);

    void on_pushButton_config_save_clicked();

    void on_spinBox_readToKeep_valueChanged(int arg1);

    void on_spinBox_unreadToKeep_valueChanged(int arg1);

    void on_spinBox_gossipRandoms_valueChanged(int arg1);

    void on_spinBox_listSizeFirst_valueChanged(int arg1);

    void on_spinBox_listSizeDefault_valueChanged(int arg1);

    void on_pushButton_config_cancel_clicked();

    void on_comboBox_news_journalist_currentIndexChanged(int index);

    void on_pushButton_edit_article_clicked();

    void on_listWidget_journalist_articles_currentRowChanged(int currentRow);

    void on_pushButton_confirm_journalist_clicked();

    void on_pushButton_remove_journalist_clicked();

    void on_pushButton_request_journalism_clicked();

    void on_pushButton_export_my_news_clicked();

    void on_toolButton_import_news_clicked();

    void on_spinBox_listSizeFirst_autodownload_valueChanged(int arg1);

    void on_spinBox_listSizeDefault_autodownload_valueChanged(int arg1);

signals:
	/**
	 * @brief Emits when Add article button is clicked and file is selected
	 * to open the Add artincle window.
	 * 
	 * @param path Path of file seleted as article's contents.
	 */
	void add_new_article(QString path);

	/**
	 * @brief Emits when IP address was selected in `comboBox_interfaces`.
	 * 
	 * Starts the STUN server and NP2PS receiver.
	 * 
	 * @param address Address on which will server listen.
	 */
	void start_server(QHostAddress address);

	/**
	 * @brief Emits when Add margin was clicked.
	 * 
	 * Opens Add margin window.
	 * 
	 * @param article 
	 */
	void add_margin(article_ptr article);

	void signal_add_new_newspaper_from_file(QString path);

	void signal_add_new_newspaper_pk(QString id_qstring);

	void signal_edit_article(qulonglong news_pid, qulonglong article_hash);

private:
	/**
	 * @brief Sets the buttons for given item in Newspaper tree.
	 * 
	 * When article is selected, all the article related buttons are enabled
	 * and all the newspaper related buttons are disabled.
	 * 
	 * When article is selected but is downloading at the moment, all the 
	 * article related buttons are disabled.
	 * 
	 * When newspaper is selected, all the newspaper related buttons are enabled
	 * and all the article related buttons are disabled.
	 * 
	 * When there is already a process of getting an article list for given
	 * newspaper happening, all the newspaper related buttons are disabled.
	 * 
	 * @param item Item, for which the buttons should be checked.
	 */
	void check_item(QTreeWidgetItem* item);

	

	/**
	 * @brief Enables and disables the article buttons based on argument
	 * 
	 * @param state What state should the article related buttons be set.
	 */
	void set_article_related_buttons(bool state);

	/**
	 * @brief Enables and disables the newspaper buttons based on argument
	 * 
	 * @param state What state should the newspaper related buttons be set.
	 */
	void set_newspaper_related_buttons(bool state);

	void fill_config_news();

	void fill_journalist_news();
	void print_journalist_articles();

	Ui::MainWindow *ui;
	ProgramContext* ctx;
	QMap<QString, QWidget*> subWindows;
};
#endif // MAINWINDOW_H
