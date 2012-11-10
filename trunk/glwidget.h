


#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QtOpenGL>
#include "Simulation.h"
#include "Body_trimesh.h"

class GLWidget : public QGLWidget
{
public:
    GLWidget();

    float rotX, rotY, rotZ; //i want access these variable so public
    void resizeGL(int width, int height);
    short int col;
    Simulation sim;  
    double Camera[3]; 
    double CamInit[3];
    double CamXrot;
    double CamYrot; 
    double camRXp, camRYp; 
    double camXi, camYi;
    //double camXf, camYf; 
    
    bool wireframe; 

protected:
    void initializeGL();
    void worldLighting();
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event);

private:
    
    


};

#endif // GLWIDGET_H
