/********************************************************************************
** Form generated from reading UI file 'form.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FORM_H
#define UI_FORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_name;
    QLineEdit *lineEdit_name;
    QCheckBox *checkBox_create_newspaper;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_newspaper_name;
    QLineEdit *lineEdit_newspaper_name;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QString::fromUtf8("Form"));
        Form->resize(450, 196);
        gridLayout = new QGridLayout(Form);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_name = new QLabel(Form);
        label_name->setObjectName(QString::fromUtf8("label_name"));

        horizontalLayout->addWidget(label_name);

        lineEdit_name = new QLineEdit(Form);
        lineEdit_name->setObjectName(QString::fromUtf8("lineEdit_name"));

        horizontalLayout->addWidget(lineEdit_name);


        verticalLayout->addLayout(horizontalLayout);

        checkBox_create_newspaper = new QCheckBox(Form);
        checkBox_create_newspaper->setObjectName(QString::fromUtf8("checkBox_create_newspaper"));
        checkBox_create_newspaper->setTristate(false);

        verticalLayout->addWidget(checkBox_create_newspaper);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_newspaper_name = new QLabel(Form);
        label_newspaper_name->setObjectName(QString::fromUtf8("label_newspaper_name"));

        horizontalLayout_2->addWidget(label_newspaper_name);

        lineEdit_newspaper_name = new QLineEdit(Form);
        lineEdit_newspaper_name->setObjectName(QString::fromUtf8("lineEdit_newspaper_name"));
        lineEdit_newspaper_name->setEnabled(false);
        lineEdit_newspaper_name->setCursor(QCursor(Qt::ForbiddenCursor));

        horizontalLayout_2->addWidget(lineEdit_newspaper_name);


        verticalLayout->addLayout(horizontalLayout_2);


        gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);

        buttonBox = new QDialogButtonBox(Form);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 1, 0, 1, 1);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QCoreApplication::translate("Form", "New peer", nullptr));
        label_name->setText(QCoreApplication::translate("Form", "Name", nullptr));
        checkBox_create_newspaper->setText(QCoreApplication::translate("Form", "Create newspaper", nullptr));
        label_newspaper_name->setText(QCoreApplication::translate("Form", "Newspaper name", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FORM_H
