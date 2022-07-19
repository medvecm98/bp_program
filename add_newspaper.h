#ifndef ADD_NEWSPAPER_H
#define ADD_NEWSPAPER_H

#include <QWidget>
#include <QTreeWidget>
#include "programcontext.h"

namespace Ui {
class add_newspaper;
}

class add_newspaper : public QWidget
{
	Q_OBJECT

public:
	explicit add_newspaper(QWidget *parent = nullptr);
	void setProgramContext(ProgramContext *c) {
		ctx = c;
	}
	~add_newspaper();

signals:
	/**
	 * @brief Signals addition of new newspaper into newspaper database.
	 * 
	 * Handy for article list generation, enabling buttons, and more.
	 */
	void new_newspaper_in_db();

private slots:

	/**
	 * @brief Action to carry on when user accepts input he made into window.
	 * 
	 * Function will identify newspaper based on information provided in lineEdit-s.
	 * All lineEdit-s are cleared.
	 */
	void on_buttonBox_accepted();

	/**
 	* @brief Action to carry on, when user cancels all the input he made into the window.
 	* 
 	* Function will only clear all lineEdit-s.
 	*/
    void on_buttonBox_rejected();

private:
	Ui::add_newspaper *ui;

	/**
	 * @brief Context of the program, mainly used for Peer class.
	 */
	ProgramContext *ctx;

	/**
	 * @brief To clear all the add_newspaper lineEdit-s.
	 * 
	 * One after another, text lineEdit-s is cleared.
	 */
	void clear_all_lineEdit();
};

#endif // ADD_NEWSPAPER_H
