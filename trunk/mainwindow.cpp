

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);

    Simulation SIM;   // Instance of a simulation 
        
    glWidget = new GLWidget;
    glWidget->sim = SIM; 
    
    ui->verticalLayout_2->addWidget( glWidget );

    A_info = new QAction(tr("Info"), this);
    ui->menuBar->addAction( A_info );
     connect( A_info , SIGNAL( triggered() ), this, SLOT( S_info() ) );

    // Secound timer for labelupdate() speed same as in GLWidget ~ 60-64 fps
    QTimer *timer = new QTimer;
        connect(timer, SIGNAL(timeout()), this, SLOT(S_labelUpdate()));
    timer->start( 50 );

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionE_xit_triggered()
{
    this->window()->close();
}

// About window
void MainWindow::S_info()
{
    about *inf = new about;
    inf->setWindowFlags(Qt::Tool);
    inf->move(int(this->x() + 0.5 * this->width() - 0.5 * inf->width()),
              int(this->y() + 0.5 * this->height() - 0.5 * inf->height()));

    inf->setWindowIcon(QIcon("j.png"));
    inf->setWindowModality(Qt::ApplicationModal);
    inf->show();
    inf->activateWindow();
}

void MainWindow::S_labelUpdate()
{
QVariant v;

     v = glWidget->Camera[0];
    ui->label_X->setText( v.toString() );
     v = glWidget->Camera[1];
    ui->label_Y->setText( v.toString() );
     v = glWidget->Camera[2];
    ui->label_Z->setText( v.toString() );
}
