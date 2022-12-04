/********************************************************************************
** Form generated from reading UI file 'categoriesform.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CATEGORIESFORM_H
#define UI_CATEGORIESFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CategoriesForm
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_select_newspaper;
    QComboBox *comboBox_newspapers;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_category;
    QLineEdit *lineEdit_category;
    QPushButton *pushButton_add_category;
    QLabel *label_categories;
    QListWidget *listWidget_categories;
    QPushButton *pushButton_remove_category;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_accept;
    QPushButton *pushButton_cancel;

    void setupUi(QWidget *CategoriesForm)
    {
        if (CategoriesForm->objectName().isEmpty())
            CategoriesForm->setObjectName(QString::fromUtf8("CategoriesForm"));
        CategoriesForm->resize(382, 424);
        gridLayout = new QGridLayout(CategoriesForm);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_select_newspaper = new QLabel(CategoriesForm);
        label_select_newspaper->setObjectName(QString::fromUtf8("label_select_newspaper"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_select_newspaper->sizePolicy().hasHeightForWidth());
        label_select_newspaper->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(label_select_newspaper);

        comboBox_newspapers = new QComboBox(CategoriesForm);
        comboBox_newspapers->setObjectName(QString::fromUtf8("comboBox_newspapers"));

        horizontalLayout->addWidget(comboBox_newspapers);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(10);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_category = new QLabel(CategoriesForm);
        label_category->setObjectName(QString::fromUtf8("label_category"));
        label_category->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_3->addWidget(label_category);

        lineEdit_category = new QLineEdit(CategoriesForm);
        lineEdit_category->setObjectName(QString::fromUtf8("lineEdit_category"));

        horizontalLayout_3->addWidget(lineEdit_category);

        pushButton_add_category = new QPushButton(CategoriesForm);
        pushButton_add_category->setObjectName(QString::fromUtf8("pushButton_add_category"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton_add_category->sizePolicy().hasHeightForWidth());
        pushButton_add_category->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(pushButton_add_category);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(1, 6);
        horizontalLayout_3->setStretch(2, 1);

        verticalLayout->addLayout(horizontalLayout_3);

        label_categories = new QLabel(CategoriesForm);
        label_categories->setObjectName(QString::fromUtf8("label_categories"));
        label_categories->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_categories);

        listWidget_categories = new QListWidget(CategoriesForm);
        listWidget_categories->setObjectName(QString::fromUtf8("listWidget_categories"));

        verticalLayout->addWidget(listWidget_categories);

        pushButton_remove_category = new QPushButton(CategoriesForm);
        pushButton_remove_category->setObjectName(QString::fromUtf8("pushButton_remove_category"));
        sizePolicy1.setHeightForWidth(pushButton_remove_category->sizePolicy().hasHeightForWidth());
        pushButton_remove_category->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(pushButton_remove_category);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButton_accept = new QPushButton(CategoriesForm);
        pushButton_accept->setObjectName(QString::fromUtf8("pushButton_accept"));

        horizontalLayout_4->addWidget(pushButton_accept);

        pushButton_cancel = new QPushButton(CategoriesForm);
        pushButton_cancel->setObjectName(QString::fromUtf8("pushButton_cancel"));

        horizontalLayout_4->addWidget(pushButton_cancel);


        verticalLayout->addLayout(horizontalLayout_4);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(CategoriesForm);

        QMetaObject::connectSlotsByName(CategoriesForm);
    } // setupUi

    void retranslateUi(QWidget *CategoriesForm)
    {
        CategoriesForm->setWindowTitle(QCoreApplication::translate("CategoriesForm", "Add article", nullptr));
        label_select_newspaper->setText(QCoreApplication::translate("CategoriesForm", "Select newspaper:", nullptr));
        label_category->setText(QCoreApplication::translate("CategoriesForm", "Category:", nullptr));
        pushButton_add_category->setText(QCoreApplication::translate("CategoriesForm", "Add category", nullptr));
        label_categories->setText(QCoreApplication::translate("CategoriesForm", "Categories for article:", nullptr));
        pushButton_remove_category->setText(QCoreApplication::translate("CategoriesForm", "Remove category", nullptr));
        pushButton_accept->setText(QCoreApplication::translate("CategoriesForm", "Accept", nullptr));
        pushButton_cancel->setText(QCoreApplication::translate("CategoriesForm", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CategoriesForm: public Ui_CategoriesForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CATEGORIESFORM_H
