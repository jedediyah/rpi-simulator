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
double CamYrot; 
double camRXp, camRYp; 
double camXi, camYi;

// STATIC methods
void makeMenu(void);
void run();
void drawScene(); 
void resize(int w, int h);  
void keyInput(unsigned char key, int x, int y);
void initializeGL(int argc, char **argv); 
void worldLighting();

using namespace std;
//using namespace boost; 

// Constructors 
SimulationEnvironment::SimulationEnvironment(int argc, char **argv) {
 
    initializeGL( argc, argv );
    //SIM.setParent(this); 
    
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
    glutInitWindowSize(1200, 800);
    glutInitWindowPosition(300, 100); 
    glutCreateWindow("RPI - Simulator");
    glutDisplayFunc(drawScene); 
    glutReshapeFunc(resize);  
    glutKeyboardFunc(keyInput);
    makeMenu(); 
    
    CamInit[0]=0;           CamInit[1]=0;           CamInit[2]=-20;
    Camera[0]=CamInit[0];   Camera[1]=CamInit[1];   Camera[2]=CamInit[2];  
    
    worldLighting(); 
    
    SIM.addCube();      // TODO
    SIM.addCube();
    SIM.addSphere();
    SIM.addSphere();
    SIM.addSphere();
    
    drawScene();
}

void worldLighting() {
    //glClearColor(0.1529f, 0.1529f, 0.1529f, 1.f);   // Blender color
    glClearColor(0.2529f, 0.2529f, 0.2529f, 0.f);
    
    glEnable(GL_DEPTH_TEST); // Enable depth testing.

    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);

    // Light property vectors.
    float lightAmb[] =         { 0.0, 0.0, 0.0, 1.0 };
    float lightDifAndSpec0[] = { 1.0, 1.0, 1.0, 1.0 };
    float globAmb[] =          { 0.2, 0.2, 0.2, 1.0 };
    float lightPosition[] =    { 0.0, 0.0, -30.0, 1.0 };

    // Light0 properties.
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition); 
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
    
    glEnable(GL_LIGHT0); // Enable particular light source.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint

    // Cull back faces.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
}

// Drawing routine.
void drawScene(void)
{  
    //cout << " DRAWING " << endl;
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    worldLighting();
    
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
              0,1,0);  // Y-direction is up
    
    SIM.draw(wireframe);
    
    glFlush(); 
}


// OpenGL window reshape routine.
void resize(int width, int height)
{
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int side = min(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

   // glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //glOrtho(-20.0, 20.0, -20.0, 20.0, -100.0, 100.0);
   gluPerspective(70, width/height, 0.1, 100);
   
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
   cout << "Hello World! Thread ID, " << tid << endl;
   
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
               for (int i=0; i<10; i++) {   // Runs a certain number of times
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


// Main method 
int SimulationEnvironment::start() {
   glutMainLoop(); 
   return 0;  
}

