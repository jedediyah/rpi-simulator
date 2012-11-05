#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "glwidget.h"

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    GLWidget *glWidget;
    Ui::MainWindowClass *ui;

    QAction *A_info;

private slots:
    void on_actionE_xit_triggered();
    void S_labelUpdate();
    void S_info();
};

#endif // MAINWINDOW_H
