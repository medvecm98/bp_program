/********************************************************************************
** Form generated from reading UI file 'add_newspaper.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADD_NEWSPAPER_H
#define UI_ADD_NEWSPAPER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_add_newspaper
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label_2;
    QLineEdit *lineEdit_name;
    QLabel *label_3;
    QLineEdit *lineEdit_ip;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *add_newspaper)
    {
        if (add_newspaper->objectName().isEmpty())
            add_newspaper->setObjectName(QString::fromUtf8("add_newspaper"));
        add_newspaper->resize(265, 104);
        gridLayout = new QGridLayout(add_newspaper);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFormAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_2 = new QLabel(add_newspaper);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignJustify|Qt::AlignVCenter);

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        lineEdit_name = new QLineEdit(add_newspaper);
        lineEdit_name->setObjectName(QString::fromUtf8("lineEdit_name"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit_name);

        label_3 = new QLabel(add_newspaper);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        lineEdit_ip = new QLineEdit(add_newspaper);
        lineEdit_ip->setObjectName(QString::fromUtf8("lineEdit_ip"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit_ip);


        verticalLayout->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(add_newspaper);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);


        retranslateUi(add_newspaper);

        QMetaObject::connectSlotsByName(add_newspaper);
    } // setupUi

    void retranslateUi(QWidget *add_newspaper)
    {
        add_newspaper->setWindowTitle(QCoreApplication::translate("add_newspaper", "Add newspaper", nullptr));
        label_2->setText(QCoreApplication::translate("add_newspaper", "name", nullptr));
        label_3->setText(QCoreApplication::translate("add_newspaper", "ip", nullptr));
    } // retranslateUi

};

namespace Ui {
    class add_newspaper: public Ui_add_newspaper {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADD_NEWSPAPER_H
