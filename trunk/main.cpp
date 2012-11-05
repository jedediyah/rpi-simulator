

#include <QtGui/QApplication>
#include "mainwindow.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);      // I think this has to go here...
    
    QApplication app(argc, argv);
    MainWindow w;

    w.setWindowTitle( "Simulator" );

    w.show();
    return app.exec();
}
