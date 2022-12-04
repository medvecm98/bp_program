/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QPushButton *pushButton_testPeer2;
    QPushButton *pushButton_testPeer1;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_new_peer;
    QPushButton *pushButton_add_news;
    QLabel *label_3;
    QTreeWidget *treeWidget_newspaper;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *pushButton_article_list;
    QPushButton *pushButton_external_article;
    QLabel *label;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *lineEdit_addJournalist;
    QPushButton *pushButton_addJournalist;
    QLabel *label_4;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *pushButton_add_margin;
    QPushButton *pushButton_view_margin;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit_margin;
    QPushButton *pushButton;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    QComboBox *comboBox_interfacs;
    QPushButton *pushButton_set_ip;
    QVBoxLayout *verticalLayout;
    QLabel *label_article;
    QTextEdit *textEdit_article;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_add_article;
    QPushButton *pushButton_delete_article;
    QPushButton *pushButton_print_peer;
    QLabel *label_margin;
    QPlainTextEdit *plainTextEdit_margins;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1070, 647);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_testPeer2 = new QPushButton(centralwidget);
        pushButton_testPeer2->setObjectName(QString::fromUtf8("pushButton_testPeer2"));

        gridLayout->addWidget(pushButton_testPeer2, 1, 1, 1, 1);

        pushButton_testPeer1 = new QPushButton(centralwidget);
        pushButton_testPeer1->setObjectName(QString::fromUtf8("pushButton_testPeer1"));

        gridLayout->addWidget(pushButton_testPeer1, 1, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButton_new_peer = new QPushButton(centralwidget);
        pushButton_new_peer->setObjectName(QString::fromUtf8("pushButton_new_peer"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_new_peer->sizePolicy().hasHeightForWidth());
        pushButton_new_peer->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(pushButton_new_peer);

        pushButton_add_news = new QPushButton(centralwidget);
        pushButton_add_news->setObjectName(QString::fromUtf8("pushButton_add_news"));
        pushButton_add_news->setEnabled(false);
        sizePolicy.setHeightForWidth(pushButton_add_news->sizePolicy().hasHeightForWidth());
        pushButton_add_news->setSizePolicy(sizePolicy);

        horizontalLayout_4->addWidget(pushButton_add_news);


        verticalLayout_2->addLayout(horizontalLayout_4);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_3);

        treeWidget_newspaper = new QTreeWidget(centralwidget);
        treeWidget_newspaper->setObjectName(QString::fromUtf8("treeWidget_newspaper"));

        verticalLayout_2->addWidget(treeWidget_newspaper);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        pushButton_article_list = new QPushButton(centralwidget);
        pushButton_article_list->setObjectName(QString::fromUtf8("pushButton_article_list"));
        pushButton_article_list->setEnabled(false);

        horizontalLayout_6->addWidget(pushButton_article_list);

        pushButton_external_article = new QPushButton(centralwidget);
        pushButton_external_article->setObjectName(QString::fromUtf8("pushButton_external_article"));
        pushButton_external_article->setEnabled(false);

        horizontalLayout_6->addWidget(pushButton_external_article);


        verticalLayout_2->addLayout(horizontalLayout_6);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        lineEdit_addJournalist = new QLineEdit(centralwidget);
        lineEdit_addJournalist->setObjectName(QString::fromUtf8("lineEdit_addJournalist"));

        horizontalLayout_3->addWidget(lineEdit_addJournalist);

        pushButton_addJournalist = new QPushButton(centralwidget);
        pushButton_addJournalist->setObjectName(QString::fromUtf8("pushButton_addJournalist"));

        horizontalLayout_3->addWidget(pushButton_addJournalist);


        verticalLayout_2->addLayout(horizontalLayout_3);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_4);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        pushButton_add_margin = new QPushButton(centralwidget);
        pushButton_add_margin->setObjectName(QString::fromUtf8("pushButton_add_margin"));
        pushButton_add_margin->setEnabled(false);

        horizontalLayout_8->addWidget(pushButton_add_margin);

        pushButton_view_margin = new QPushButton(centralwidget);
        pushButton_view_margin->setObjectName(QString::fromUtf8("pushButton_view_margin"));
        pushButton_view_margin->setEnabled(false);

        horizontalLayout_8->addWidget(pushButton_view_margin);


        verticalLayout_3->addLayout(horizontalLayout_8);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lineEdit_margin = new QLineEdit(centralwidget);
        lineEdit_margin->setObjectName(QString::fromUtf8("lineEdit_margin"));

        horizontalLayout_2->addWidget(lineEdit_margin);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setEnabled(false);

        horizontalLayout_2->addWidget(pushButton);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout_2->addLayout(verticalLayout_3);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        comboBox_interfacs = new QComboBox(centralwidget);
        comboBox_interfacs->setObjectName(QString::fromUtf8("comboBox_interfacs"));

        horizontalLayout->addWidget(comboBox_interfacs);

        pushButton_set_ip = new QPushButton(centralwidget);
        pushButton_set_ip->setObjectName(QString::fromUtf8("pushButton_set_ip"));

        horizontalLayout->addWidget(pushButton_set_ip);

        horizontalLayout->setStretch(0, 7);
        horizontalLayout->setStretch(1, 1);

        verticalLayout_2->addLayout(horizontalLayout);


        horizontalLayout_5->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_article = new QLabel(centralwidget);
        label_article->setObjectName(QString::fromUtf8("label_article"));
        label_article->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_article);

        textEdit_article = new QTextEdit(centralwidget);
        textEdit_article->setObjectName(QString::fromUtf8("textEdit_article"));
        textEdit_article->setReadOnly(true);

        verticalLayout->addWidget(textEdit_article);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        pushButton_add_article = new QPushButton(centralwidget);
        pushButton_add_article->setObjectName(QString::fromUtf8("pushButton_add_article"));
        pushButton_add_article->setEnabled(false);
        sizePolicy.setHeightForWidth(pushButton_add_article->sizePolicy().hasHeightForWidth());
        pushButton_add_article->setSizePolicy(sizePolicy);

        horizontalLayout_7->addWidget(pushButton_add_article);

        pushButton_delete_article = new QPushButton(centralwidget);
        pushButton_delete_article->setObjectName(QString::fromUtf8("pushButton_delete_article"));
        pushButton_delete_article->setEnabled(false);

        horizontalLayout_7->addWidget(pushButton_delete_article);

        pushButton_print_peer = new QPushButton(centralwidget);
        pushButton_print_peer->setObjectName(QString::fromUtf8("pushButton_print_peer"));
        pushButton_print_peer->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton_print_peer->sizePolicy().hasHeightForWidth());
        pushButton_print_peer->setSizePolicy(sizePolicy1);

        horizontalLayout_7->addWidget(pushButton_print_peer);


        verticalLayout->addLayout(horizontalLayout_7);

        label_margin = new QLabel(centralwidget);
        label_margin->setObjectName(QString::fromUtf8("label_margin"));
        label_margin->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_margin);

        plainTextEdit_margins = new QPlainTextEdit(centralwidget);
        plainTextEdit_margins->setObjectName(QString::fromUtf8("plainTextEdit_margins"));

        verticalLayout->addWidget(plainTextEdit_margins);


        horizontalLayout_5->addLayout(verticalLayout);

        horizontalLayout_5->setStretch(0, 1);
        horizontalLayout_5->setStretch(1, 2);

        gridLayout->addLayout(horizontalLayout_5, 0, 0, 1, 2);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Newspaper P2P sharing", nullptr));
        pushButton_testPeer2->setText(QCoreApplication::translate("MainWindow", "Test Peer 2 create", nullptr));
        pushButton_testPeer1->setText(QCoreApplication::translate("MainWindow", "Test Peer 1 create", nullptr));
        pushButton_new_peer->setText(QCoreApplication::translate("MainWindow", "New peer", nullptr));
        pushButton_add_news->setText(QCoreApplication::translate("MainWindow", "Add news", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Articles and newspapers", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget_newspaper->headerItem();
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("MainWindow", "ID", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("MainWindow", "Type", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "Name", nullptr));
        pushButton_article_list->setText(QCoreApplication::translate("MainWindow", "Get Article List", nullptr));
        pushButton_external_article->setText(QCoreApplication::translate("MainWindow", "Preview article", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Journalists", nullptr));
        pushButton_addJournalist->setText(QCoreApplication::translate("MainWindow", "Add journalist", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "Margins", nullptr));
        pushButton_add_margin->setText(QCoreApplication::translate("MainWindow", "Add margin", nullptr));
        pushButton_view_margin->setText(QCoreApplication::translate("MainWindow", "View Margin", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Request margin", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Set IP", nullptr));
        pushButton_set_ip->setText(QCoreApplication::translate("MainWindow", "Set IP", nullptr));
        label_article->setText(QCoreApplication::translate("MainWindow", "Article", nullptr));
        textEdit_article->setPlaceholderText(QCoreApplication::translate("MainWindow", "README: To ease the testing, we have provided two testing peers. Peer 1 and Peer 2. Both of them will create their own newspaper and will add some articles with categories. However, we couldn't automate setting the IP, so that needs to be done manually, using the dropdown menu and Set IP button.", nullptr));
        pushButton_add_article->setText(QCoreApplication::translate("MainWindow", "Add article", nullptr));
        pushButton_delete_article->setText(QCoreApplication::translate("MainWindow", "Delete article", nullptr));
        pushButton_print_peer->setText(QCoreApplication::translate("MainWindow", "print Peer", nullptr));
        label_margin->setText(QCoreApplication::translate("MainWindow", "Margin contents", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
