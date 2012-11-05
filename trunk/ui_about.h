/********************************************************************************
** Form generated from reading UI file 'about.ui'
**
** Created: Sun Nov 4 23:27:29 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_about
{
public:
    QLabel *label;

    void setupUi(QDialog *about)
    {
        if (about->objectName().isEmpty())
            about->setObjectName(QString::fromUtf8("about"));
        about->setWindowModality(Qt::ApplicationModal);
        about->resize(396, 264);
        about->setMinimumSize(QSize(396, 264));
        about->setMaximumSize(QSize(396, 264));
        about->setContextMenuPolicy(Qt::NoContextMenu);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icon/icon"), QSize(), QIcon::Normal, QIcon::Off);
        about->setWindowIcon(icon);
        about->setModal(true);
        label = new QLabel(about);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 371, 251));
        label->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        label->setWordWrap(true);
        label->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

        retranslateUi(about);

        QMetaObject::connectSlotsByName(about);
    } // setupUi

    void retranslateUi(QDialog *about)
    {
        about->setWindowTitle(QApplication::translate("about", "About", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("about", "<HTML><BODY>Open Simulator is a rigid body physics simulator.  The dynamics behind OSim is the same as is implemented in the </a href=\"http://code.google.com/p/rpi-matlab-simulator/\">RPI-MATLAB-Simulator</a>.</BODY></HTML>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class about: public Ui_about {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_H
