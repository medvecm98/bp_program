/********************************************************************************
** Form generated from reading UI file 'addmargin.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDMARGIN_H
#define UI_ADDMARGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddMargin
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QPlainTextEdit *plainTextEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *AddMargin)
    {
        if (AddMargin->objectName().isEmpty())
            AddMargin->setObjectName(QString::fromUtf8("AddMargin"));
        AddMargin->resize(327, 282);
        gridLayout = new QGridLayout(AddMargin);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(AddMargin);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);

        lineEdit = new QLineEdit(AddMargin);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        verticalLayout->addWidget(lineEdit);

        label_2 = new QLabel(AddMargin);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_2);

        plainTextEdit = new QPlainTextEdit(AddMargin);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        buttonBox = new QDialogButtonBox(AddMargin);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(AddMargin);

        QMetaObject::connectSlotsByName(AddMargin);
    } // setupUi

    void retranslateUi(QWidget *AddMargin)
    {
        AddMargin->setWindowTitle(QCoreApplication::translate("AddMargin", "Add margin", nullptr));
        label->setText(QCoreApplication::translate("AddMargin", "Type", nullptr));
        label_2->setText(QCoreApplication::translate("AddMargin", "Contents", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddMargin: public Ui_AddMargin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDMARGIN_H
