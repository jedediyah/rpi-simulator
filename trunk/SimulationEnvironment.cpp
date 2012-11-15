/* 
 * File:   SimulationEnvironment.cpp
 * Author: carser
 * 
 * Created on November 10, 2012, 4:48 PM
 */

#include "SimulationEnvironment.h"

#include <cmath>
//#include <boost/thread.hpp>
#include <pthread.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif


// Global variables
static GLsizei width, height;   // OpenGL window size.
static Simulation SIM;          // Instance of simulator
static pthread_t simThread; 
static int rc; 
static bool wireframe = false; 

double Camera[3]; 
double CamInit[3];
double CamXrot;
double CamZrot; 
double camRXp, camRYp; 
double camXi, camYi;
double camZoomFactor = 1.0;  // Change of 10% every zoom
double gridColor[] = {0.5, 0.5, 0.5};

// STATIC methods
void makeMenu(void);
void run();
void drawScene(); 
void resize(int w, int h);  
void keyInput(unsigned char key, int x, int y);
void initializeGL(int argc, char **argv); 
void worldLighting();
void mousePressEvent(int button, int state, int x, int y);
void mouseMoveEvent(int x, int y);
void updateCameraPosition();
void mousePassiveMoveEvent(int x, int y);

using namespace std;
//using namespace boost; 

// Constructors 
SimulationEnvironment::SimulationEnvironment(int argc, char **argv) {
 
    initializeGL( argc, argv );
    
}

SimulationEnvironment::SimulationEnvironment(const SimulationEnvironment& orig) {
}

SimulationEnvironment::~SimulationEnvironment() {
}

void menuMain(int id) {
    cout << "Main Menu, " << id << endl;
}
void menuAddObject(int objID) {
    cout << "Add Object... [" << objID << "]" << endl; 
    switch(objID) {
        case 1:
            cout << "Adding sphere" << endl;
            break;
        case 2: 
            cout << "Adding tetrahedron" << endl;
            break;
        case 3: 
            cout << "Adding cube (hexahedron)" << endl;
            break;
        case 4: 
            cout << "Adding octahedron" << endl;
            break;
        case 5: 
            cout << "Adding dodecahedron" << endl;
            break;
        case 6: 
            cout << "Adding icosahedron" << endl;
            break;
            
    }
    
}

void menuLighting(int id) {
    cout << "Lighting" << endl;
}

void menuQuit(int id) {
    cout << "Quitting..." << endl; 
}

// Routine to make the menu.
void makeMenu(void)
{
   // The sub-menu is created first (because it should be visible when the top
   // menu is created): its callback function is registered and menu entries added.
   int imenuMain, imenuAdd, imenuQuit;


   imenuAdd = glutCreateMenu(menuAddObject);
   glutAddMenuEntry("Sphere",1); 
   glutAddMenuEntry("Tetrahedron",2);  
   glutAddMenuEntry("Cube (hexahedron)",3);
   glutAddMenuEntry("Octahedron",4);
   glutAddMenuEntry("Dodecahedron",5);
   glutAddMenuEntry("Icosahedron",6);
   
   imenuQuit = glutCreateMenu(menuQuit);


   // The top menu is created: its callback function is registered and menu entries,
   // including a submenu, added.
   glutCreateMenu(menuMain);
   glutAddSubMenu("Add Object", imenuAdd);
   glutAddMenuEntry("Quit",imenuQuit);

   // The menu is attached to a mouse button.
   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void initializeGL(int argc, char **argv)
{
    cout << "Initializing GL." << endl;
    
    // GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB); 
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(300, 100); 
    glutCreateWindow("RPI - Simulator");
    glutDisplayFunc(drawScene); 
    glutReshapeFunc(resize);  
    glutKeyboardFunc(keyInput);                 // Keyboard input
    glutMouseFunc(mousePressEvent);             // Mouse press
    glutMotionFunc(mouseMoveEvent);             // Active mouse motion
    glutPassiveMotionFunc(mousePassiveMoveEvent);      // Passive mouse motion
    
    glEnable(GL_DEPTH_TEST); // Enable depth testing.
    
    makeMenu(); 
    
    //CamInit[0]=-20;           CamInit[1]=-34.641;           CamInit[2]=30;
    CamInit[0]=-10.0;           CamInit[1]=-14.3205;           CamInit[2]=12.0;
    Camera[0]=CamInit[0];   Camera[1]=CamInit[1];   Camera[2]=CamInit[2]; 
    
    drawScene();
    
    
//    SIM.addCube();      // TODO
//    SIM.addCube();
    SIM.addSphere();
    SIM.addSphere();
//    SIM.addSphere();
    
}

// draws a 10x10 grid at Z=0, centered at the origin (x,y) = (0,0) 
void drawGrid() {
    //cout << "GRID" << endl;
    glDisable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL); 
    
    glEnable (GL_LINE_SMOOTH);          // TODO: Can go to INIT
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    
    
    glColor3f(gridColor[0],gridColor[1],gridColor[2]);
    double xmin = -5.0; 
    double xmax =  5.0;
    double ymin = -5.0; 
    double ymax =  5.0;
        // Grid rows.
        for (double y=ymin; y<=-1.0; y++) {     // Bottom rows
            glBegin(GL_LINES);
                glVertex3f(xmin,y,0.0);
                glVertex3f(xmax,y,0.0);
            glEnd();
        }
        glBegin(GL_LINES);                       // Middle row 
            glVertex3f(xmin,0.0,0.0);
            glVertex3f(0.0,0.0,0.0);
        glEnd();
        glLineWidth(2.0);
        glColor3f(1.0,0.0,0.0);
        glBegin(GL_LINES);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(1.0,0.0,0.0);
        glEnd();
        glLineWidth(1.0);
        glColor3f(gridColor[0],gridColor[1],gridColor[2]); 
        glBegin(GL_LINES);
            glVertex3f(1.0,0.0,0.0);
            glVertex3f(xmax,0.0,0.0);
        glEnd();
        for (double y=1.0; y<=ymax; y++) {       // Top rows
            glBegin(GL_LINES);
                glVertex3f(xmin,y,0.0);
                glVertex3f(xmax,y,0.0);
            glEnd();
        }
        
        // Grid columns.
        for (double x=xmin; x<=-1.0; x++) {     // Bottom columns
            glBegin(GL_LINES);
                glVertex3f(x,ymin,0.0);
                glVertex3f(x,ymax,0.0);
            glEnd();
        }
        glBegin(GL_LINES);                      // Middle column
            glVertex3f(0.0,ymin,0.0);
            glVertex3f(0.0,0.0,0.0);
        glEnd();
        glLineWidth(2.0);
        glColor3f(0.0,1.0,0.0);
        glBegin(GL_LINES);
            glVertex3f(0.0,0.0,0.0);
            glVertex3f(0.0,1.0,0.0);
        glEnd();
        glLineWidth(1.0);
        glColor3f(gridColor[0],gridColor[1],gridColor[2]); 
        glBegin(GL_LINES);
            glVertex3f(0.0,1.0,0.0);
            glVertex3f(0.0,ymax,0.0);
        glEnd();
        for (double x=1.0; x<=xmax; x++) {       // Top columns
            glBegin(GL_LINES);
                glVertex3f(x,ymin,0.0);
                glVertex3f(x,ymax,0.0);
            glEnd();
        }        
        
    // Let's finish drawing the world coordinate vectors...
    glLineWidth(2.0);
    glColor3f(0.0,0.0,1.0);             // Z VECTOR
    glBegin(GL_LINES);
        glVertex3f(0.0,0.0,0.0);
        glVertex3f(0.0,0.0,1.0);
    glEnd();
    glLineWidth(1.0);
    
    glDisable(GL_COLOR_MATERIAL); 
    glEnable(GL_LIGHTING);
}

void worldLighting() {
    //glClearColor(0.1529f, 0.1529f, 0.1529f, 1.f);   // Blender color
    glClearColor(0.2529f, 0.2529f, 0.2529f, 0.f);
    
    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);

    // Light property vectors.
    float lightAmb0[] =         { 0.5, 0.5, 0.5, 1.0 };
    float lightDifAndSpec0[] =  { 0.5, 0.5, 0.5, 1.0 };
    float globAmb0[] =          { 0.2, 0.2, 0.2, 1.0 };
    float lightPos0[] =         { 0.0, 0.0, 30.0, 1.0 };

    // Light0 properties.
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0); 
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb0); // Global ambient light.
    
    glEnable(GL_LIGHT0); // Enable particular light source.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint

    // Cull back faces.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Draw sphere at light
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
        glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
        glColor3f(0.0, 1.0, 0.0); 
        glutWireSphere(0.05, 8, 8);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

// Drawing routine.
void drawScene(void)
{  
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear
    glColor3f(1.0, 1.0, 0.5);     // Color of objects

    // Wire-frame or not
    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Rotate camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(Camera[0],Camera[1],Camera[2],  // Camera position
              0,0,0,   // Look at center 
              0,0,1);  // Z-direction is up
    worldLighting();            // Lighting
    drawGrid();                 // Grid
    SIM.draw(wireframe);        // Simulation objects
    
    glFlush(); 
}


// OpenGL window reshape routine.
void resize(int width, int height)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int side = min(width, height);
    //glViewport((width - side) / 2, (height - side) / 2, side, side);
    
   // glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //glOrtho(-20.0, 20.0, -20.0, 20.0, -100.0, 100.0);
   gluPerspective(60, width/height, 0.1, 500);
   
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
}

void SimulationEnvironment::test(unsigned char key) {
    cout << "Class Press: " << key << endl;
}


void *runSim(void *threadid)
{
    
   long tid;
   tid = (long)threadid;
   cout << "New thread.  Thread ID, " << tid << endl;
   
   cout << "Starting SIM... " << endl;
   while(SIM.isRunning()) {
        SIM.step();
        cout << "Stepping ..." << endl;
        //drawScene(); 
    }
   cout << "Finished running SIM." << endl;
   
   pthread_exit(NULL);
}


// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   // cout << "K:" << (int)key << endl;
   switch (key) 
   {
       case 32: // Space bar
           if (SIM.isRunning()) {
               SIM.setRun(false);
           } else {
//                SIM.setRun(true); 
//                pthread_t thread;
//                int rc;
//                rc = pthread_create(&thread, NULL, runSim, (void *)0);
//                if (rc){
//                   cout << "Error:unable to create thread," << rc << endl;
//                   exit(-1);
//                }
               for (int i=0; i<1; i++) {   // Runs a certain number of times
                   SIM.step();
                   drawScene();
               }
           }
           break;
       case 115: // s
           cout << "Adding sphere..." << endl;
           SIM.addSphere();
           break;
       case 112: // p
           SIM.printBodies(); 
           break;
       case 9: // [tab]
           wireframe = !wireframe; 
           drawScene();
           break;
           
       case 113: // q
           exit(0);
           break; 
       case 27: // [ESC]
           exit(0);
           break;
       default:
           break;
   }
}


void mousePressEvent(int button, int state, int x, int y)
{
  //process mouse events for rotate/move inside 3D scene
//    cout << "Mouse Press" << endl;
//    cout << "Button: " << button << endl; 
//    cout << "State: " << state << endl; 
    
    if (button == 0) {
        camRXp = CamXrot;
        camRYp = CamZrot;
        camXi = x;
        camYi = y;
        //cout << "Starting rotation with (" << camRXp << ", " << camRYp << ")" << endl;
    }
    else if (button == 3) {
        //cout << "Zoom in " << endl;
        // Update camera position
        camZoomFactor *= 0.9;   // Zoom
        updateCameraPosition(); // Update camera position 
        drawScene();            // Redraw from new camera position 
    }
    else if (button == 4) {
        //cout << "Zoom out " << endl; 
        // Update camera position
        camZoomFactor *= 1.1;   // Zoom
        updateCameraPosition(); // Update camera position 
        drawScene();            // Redraw from new camera position 
    }
}

void mouseMoveEvent(int x, int z)
{
 //process keyboard events
    //cout << "Mouse Event" << endl; 
    //cout << "   dX = " << x-camXi << ",   dZ = " << camYi-z << endl;  
     
    double rotScale = 0.01;
    CamXrot = camRXp + (camYi-z)*rotScale; 
    CamZrot = camRYp + (camXi-x)*rotScale; 
    
    updateCameraPosition();  
}

void updateCameraPosition() {
    //cout << "Updating rotation (" << CamXrot << ", " << CamZrot << ")" << endl;
    
    mat Rx = zeros(3,3);  // Init rotation matrices
    mat Rz = zeros(3,3);
    
    Rx(0,0) = 1;
    Rx(1,1) = cos(CamXrot); 
    Rx(2,2) = cos(CamXrot);
    Rx(1,2) = -sin(CamXrot);
    Rx(2,1) = sin(CamXrot);
    //Rx.print();
    
    Rz(2,2) = 1;
    Rz(0,0) = cos(CamZrot);
    Rz(1,1) = cos(CamZrot);
    Rz(0,1) = -sin(CamZrot);
    Rz(1,0) = sin(CamZrot); 
    
    mat zi = zeros(3,1);
    zi(0,0) = CamInit[0] * camZoomFactor;
    zi(1,0) = CamInit[1] * camZoomFactor;
    zi(2,0) = CamInit[2] * camZoomFactor; 
    
    mat r = Rz*Rx*zi;   
    Camera[0] = r(0);
    Camera[1] = r(1);
    Camera[2] = r(2); 
    
    drawScene();
}

void mousePassiveMoveEvent(int x, int y) {
    //cout << "Passive Move " << x << ", " << y << endl;
}


// Main method 
int SimulationEnvironment::start() {
    glutMainLoop(); 
    return 0;  
}

