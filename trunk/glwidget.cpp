

#include "glwidget.h"
#include "Body_trimesh.h"

GLWidget::GLWidget()
{
    startTimer( 11 ); //64-65fps
    rotX = rotY = rotZ = 0.f;
    col = 0;
    wireframe = false;
}

void GLWidget::initializeGL()
{
    cout << "Initializing GL." << endl;
    //initialization of OpenGLglColor3f(0.0, 0.0, 0.3);

    glClearColor(0.5f, 0.5f, 0.5f, 0.f);
    //resizeGL( 400 , 300 );

    glShadeModel( GL_SMOOTH );
    //glShadeModel (GL_FLAT);
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    
    
        //glClearColor(0.0, 0.0, 0.0, 0.0);
        //glEnable(GL_DEPTH_TEST); // Enable depth testing.

        // Turn on OpenGL lighting.
        glEnable(GL_LIGHTING);

        // Light property vectors.
        float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
        float lightDifAndSpec0[] = { 1.0, 1.0, 5.0, 1.0 };
        float lightDifAndSpec1[] = { 0.0, 1.0, 0.0, 1.0 };
        float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };
	float lightPos[] = { 0.0, 10.0, 20.0, 1.0 };  // Light position

        // Light0 properties.
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

        // Light1 properties.
        //glEnable(GL_LIGHT1); // Enable particular light source.
        //glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmb);
        //glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDifAndSpec1);
        //glLightfv(GL_LIGHT1, GL_SPECULAR, lightDifAndSpec1);

        glEnable(GL_LIGHT0); // Enable particular light source.
        
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local v
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.
        
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.
        
        // Cull back faces.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

    
    sim.addCube();
    sim.addSphere();
}

// The main DRAW function 
void GLWidget::paintGL()
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 0.5);
    
    // Wire-frame or not
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//    cube.stepDynamics(.01);  
//    cube.updateWorld_Verts(); 
//    cube.draw();
    
    cout << " Sim time... " << endl; 
    sim.draw();
    

    glFlush(); 
}

void GLWidget::resizeGL(int width, int height)
{
    //proces resize keep good aspect ratio for 3D scene

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

   // glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //glOrtho(-150.0, 150.0, -150.0, 150.0, -10.0, 10.0);
   glOrtho(-20.0, 20.0, -20.0, 20.0, -10.0, 10.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
  //process mouse events for rotate/move inside 3D scene
    cout << "Mouse Press" << endl;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
 //process keyboard events
    cout << "Mouse Event" << endl; 
}

void GLWidget::timerEvent(QTimerEvent *event)
{
    updateGL();
}
