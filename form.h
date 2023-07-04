#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "programcontext.h"

namespace Ui {
class Form;
}

/**
 * @brief This weirdly named class in fact represents the New peer window.
 * 
 */
class Form : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief Construct a new Form object
	 * 
	 * Created by QtCreator
	 */
	explicit Form(QWidget *parent = nullptr);

	/**
	 * @brief Sets the Program Context.
	 * 
	 * @param c Program context to set.
	 */
	void setProgramContext(ProgramContext* c) {
		ctx = c;
	}

	/**
	 * @brief Destroy the Form object
	 * 
	 * Created by QtCreator.
	 */
	~Form();

private slots:
	/**
	 * @brief When the checkbox is either checked or unchecked.
	 * 
	 * Enables the `lineEdit_newspaper_name` if checked.
	 * Disables the `lineEdit_newspaper_name` if unchecked.
	 */
	void on_checkBox_create_newspaper_stateChanged(int arg1);

	/**
	 * @brief OK button behaviour.
	 * 
	 * Name of the peer is set.
	 * 
	 * If the `checkBox_create_newspaper` was checked and the 
	 * `lineEdit_newspaper_name` was filled, new news are founded. Also this
	 * will disable the New peer button in MainWindow because it is not longer
	 * neccesary.
	 */
	void on_buttonBox_accepted();

    /**
     * @brief Cancel button behaviour.
     * 
	 * Clears the lineEdit-s.
     */
	void on_buttonBox_rejected();

signals:
	/**
	 * @brief Enables the Add article button.
	 */
	void enable_add_article();

	/**
	 * @brief Enables the Add newspaper button.
	 */
	void enable_add_newspaper();

	/**
	 * @brief Enables the print peer button.
	 */
	void enable_print_peer();

	/**
	 * @brief Disables the New peer button.
	 */
	void disable_new_peer();

	/**
	 * @brief Signals that new newspaper was founded.
	 * 
	 * So it may be inserted into database
	 */
	void created_newspaper();

	void creation_cancelled();

private:
	Ui::Form *ui;
	ProgramContext *ctx; //program context
};

#endif // FORM_H
