/********************************************************************************
** Form generated from reading UI file 'newarticleform.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWARTICLEFORM_H
#define UI_NEWARTICLEFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NewArticleForm
{
public:
    QGridLayout *gridLayout_3;
    QDialogButtonBox *buttonBox;
    QGridLayout *gridLayout_2;
    QGridLayout *gridLayout;
    QPushButton *pushButton_add_category;
    QLineEdit *lineEdit_categories;
    QLabel *label_category;
    QLabel *label_file_path;
    QPushButton *pushButton_file_path;
    QLineEdit *lineEdit_file_path;
    QPushButton *pushButton_load;
    QPushButton *pushButton_remove_category;
    QComboBox *comboBox;
    QLabel *label_already_loaded;
    QPushButton *pushButton_set_level;
    QLabel *label_categories;
    QSpinBox *spinBox;
    QListView *listView_paragraph_select;
    QListView *listView;
    QLabel *label_paragraphs;

    void setupUi(QWidget *NewArticleForm)
    {
        if (NewArticleForm->objectName().isEmpty())
            NewArticleForm->setObjectName(QString::fromUtf8("NewArticleForm"));
        NewArticleForm->resize(1085, 705);
        gridLayout_3 = new QGridLayout(NewArticleForm);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        buttonBox = new QDialogButtonBox(NewArticleForm);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout_3->addWidget(buttonBox, 1, 0, 1, 1);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        pushButton_add_category = new QPushButton(NewArticleForm);
        pushButton_add_category->setObjectName(QString::fromUtf8("pushButton_add_category"));
        pushButton_add_category->setEnabled(false);

        gridLayout->addWidget(pushButton_add_category, 4, 4, 1, 1);

        lineEdit_categories = new QLineEdit(NewArticleForm);
        lineEdit_categories->setObjectName(QString::fromUtf8("lineEdit_categories"));
        lineEdit_categories->setEnabled(false);

        gridLayout->addWidget(lineEdit_categories, 4, 1, 1, 3);

        label_category = new QLabel(NewArticleForm);
        label_category->setObjectName(QString::fromUtf8("label_category"));

        gridLayout->addWidget(label_category, 4, 0, 1, 1);

        label_file_path = new QLabel(NewArticleForm);
        label_file_path->setObjectName(QString::fromUtf8("label_file_path"));

        gridLayout->addWidget(label_file_path, 0, 0, 1, 1);

        pushButton_file_path = new QPushButton(NewArticleForm);
        pushButton_file_path->setObjectName(QString::fromUtf8("pushButton_file_path"));

        gridLayout->addWidget(pushButton_file_path, 0, 4, 1, 1);

        lineEdit_file_path = new QLineEdit(NewArticleForm);
        lineEdit_file_path->setObjectName(QString::fromUtf8("lineEdit_file_path"));

        gridLayout->addWidget(lineEdit_file_path, 0, 1, 1, 3);

        pushButton_load = new QPushButton(NewArticleForm);
        pushButton_load->setObjectName(QString::fromUtf8("pushButton_load"));
        pushButton_load->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_load->sizePolicy().hasHeightForWidth());
        pushButton_load->setSizePolicy(sizePolicy);

        gridLayout->addWidget(pushButton_load, 0, 5, 2, 1);

        pushButton_remove_category = new QPushButton(NewArticleForm);
        pushButton_remove_category->setObjectName(QString::fromUtf8("pushButton_remove_category"));
        pushButton_remove_category->setEnabled(false);

        gridLayout->addWidget(pushButton_remove_category, 4, 5, 1, 1);

        comboBox = new QComboBox(NewArticleForm);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setEnabled(true);

        gridLayout->addWidget(comboBox, 1, 1, 1, 4);

        label_already_loaded = new QLabel(NewArticleForm);
        label_already_loaded->setObjectName(QString::fromUtf8("label_already_loaded"));

        gridLayout->addWidget(label_already_loaded, 1, 0, 1, 1);


        gridLayout_2->addLayout(gridLayout, 3, 0, 1, 4);

        pushButton_set_level = new QPushButton(NewArticleForm);
        pushButton_set_level->setObjectName(QString::fromUtf8("pushButton_set_level"));
        pushButton_set_level->setEnabled(false);

        gridLayout_2->addWidget(pushButton_set_level, 8, 3, 1, 1);

        label_categories = new QLabel(NewArticleForm);
        label_categories->setObjectName(QString::fromUtf8("label_categories"));

        gridLayout_2->addWidget(label_categories, 5, 0, 1, 2);

        spinBox = new QSpinBox(NewArticleForm);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setEnabled(false);
        spinBox->setMaximum(255);

        gridLayout_2->addWidget(spinBox, 8, 2, 1, 1);

        listView_paragraph_select = new QListView(NewArticleForm);
        listView_paragraph_select->setObjectName(QString::fromUtf8("listView_paragraph_select"));
        listView_paragraph_select->setEnabled(false);

        gridLayout_2->addWidget(listView_paragraph_select, 7, 0, 2, 2);

        listView = new QListView(NewArticleForm);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setEnabled(false);

        gridLayout_2->addWidget(listView, 7, 2, 1, 2);

        label_paragraphs = new QLabel(NewArticleForm);
        label_paragraphs->setObjectName(QString::fromUtf8("label_paragraphs"));

        gridLayout_2->addWidget(label_paragraphs, 5, 2, 1, 2);


        gridLayout_3->addLayout(gridLayout_2, 0, 0, 1, 1);


        retranslateUi(NewArticleForm);

        QMetaObject::connectSlotsByName(NewArticleForm);
    } // setupUi

    void retranslateUi(QWidget *NewArticleForm)
    {
        NewArticleForm->setWindowTitle(QCoreApplication::translate("NewArticleForm", "Form", nullptr));
        pushButton_add_category->setText(QCoreApplication::translate("NewArticleForm", "Add", nullptr));
        label_category->setText(QCoreApplication::translate("NewArticleForm", "Category", nullptr));
        label_file_path->setText(QCoreApplication::translate("NewArticleForm", "File path", nullptr));
        pushButton_file_path->setText(QCoreApplication::translate("NewArticleForm", "Select file", nullptr));
        pushButton_load->setText(QCoreApplication::translate("NewArticleForm", "Load or select", nullptr));
        pushButton_remove_category->setText(QCoreApplication::translate("NewArticleForm", "Remove", nullptr));
        label_already_loaded->setText(QCoreApplication::translate("NewArticleForm", "Already loaded articles", nullptr));
        pushButton_set_level->setText(QCoreApplication::translate("NewArticleForm", "Set level", nullptr));
        label_categories->setText(QCoreApplication::translate("NewArticleForm", "Categories", nullptr));
        label_paragraphs->setText(QCoreApplication::translate("NewArticleForm", "Paragraphs and levels", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewArticleForm: public Ui_NewArticleForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWARTICLEFORM_H
