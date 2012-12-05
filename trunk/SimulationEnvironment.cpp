/* 
 * File:   SimulationEnvironment.cpp
 * Author: Jedediyah Williams
 * 
 * Created on November 10, 2012, 4:48 PM
 */

#include "SimulationEnvironment.h"
#include "SimulatorDefenitions.h"

#include <cmath>
//#define ARMA_NO_DEBUG    // TODO: add for optimization
#include <armadillo>
//#include <boost/thread.hpp>
//#include <pthread.h>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define LeftClick 0     // Mouse integers 
#define MiddleClick 1
#define RightClick 2
#define ScrollUp 3
#define ScrollDown 4

#define Shift 1         // Keyboard modifiers 
#define Ctrl 2
#define ShiftCtrl 3

#define objMOVE 1       // Classifies how to move active object
#define objMOVE_X 2
#define objMOVE_Y 3
#define objMOVE_Z 4

// Global variables
static GLsizei width = 1200;    // OpenGL window size.
static GLsizei height = 900;   
static GLsizei edge_buffer = 3; // Buffering text near panel edges
static GLsizei LeftPanelWidth = 200; 
static GLsizei InfoPanelWidth = 180; 
static GLsizei InfoPanelHeight = 130; 
static Simulation SIM;          // Instance of simulator
static pthread_t simThread;     // For multithreading the simulator
static int rc;                  // Used for simThread
static bool wireframe = false; 
static double gridColor[] = {0.5, 0.5, 0.5};
static long font_left_panel = (long)GLUT_BITMAP_HELVETICA_10; // Font selection. 
static long font_simInfo = (long)GLUT_BITMAP_HELVETICA_10;
static bool drawSimInfo = true;

static vec::fixed<3> Camera;
static vec::fixed<3> CameraPrev; 
static vec::fixed<3> CamLookAt = zeros(3); 
static vec::fixed<3> CamLookAtPrev = zeros(3); 
static vec::fixed<3> X = zeros(3);
static vec::fixed<3> Y = zeros(3); 
static vec::fixed<3> Z = zeros(3); 
static double dx;
static double dz; 

// Mouse rotation and zoom 
static bool   enableMouseRotation = false; 
static bool   enableMouseTranslation = false; 
static bool   enableObjectMove = false; 
static double rotScale = 0.01;         // How quickly mouse rotates view
static double CamInit[3];
static double CamXrot;
static double CamZrot; 
static double camRXp, camRYp; 
static double camXi, camYi;
static double camZoomFactor = 1.0;     // Change of 10% every zoom
static double objMoveFactor = 58.0;    // Smaller -> faster reaction when moving object

// Mouse translation 
static vec CamLook;
static vec CamRight;
static vec CamUp;

// Mouse move object
static int moveState = 0; 
static vec obj_Ui;  // Initial position  

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
void updateLookVectors();
void updateCameraPosition();
void mousePassiveMoveEvent(int x, int y);

using namespace std;
using namespace arma; 

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
    if (id ==2)
        exit(0);  // Quit
}
void menuAddObject(int objID) {
    //cout << "Add Object... [" << objID << "]" << endl; 
    switch(objID) {
        case 1:
            //cout << "Adding sphere" << endl;
            SIM.addSphere(); 
            //cout << SIM.activeBody_type() << ", " << SIM.activeBody_index() << endl; 
            break;
        case 2: 
            //cout << "Adding tetrahedron" << endl;
            SIM.addTetrahedron();
            break;
        case 3: 
            //cout << "Adding cube (hexahedron)" << endl;
            SIM.addCube();
            break;
        case 4: 
            //cout << "Adding octahedron" << endl;
            SIM.addOctahedron();
            break;
        case 5: 
            //cout << "Adding dodecahedron" << endl;
            SIM.addDodecahedron();
            break;
        case 6: 
            //cout << "Adding icosahedron" << endl;
            SIM.addIcosahedron(); 
            break;
        case 7:
            SIM.addTeaPot();
            break;
            
    }
    drawScene();  
    
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
   // Sub menus
   int imenuMain, imenuAdd, imenuQuit;

   imenuAdd = glutCreateMenu(menuAddObject);
   glutAddMenuEntry("Sphere",1); 
   glutAddMenuEntry("Tetrahedron",2);  
   glutAddMenuEntry("Cube (hexahedron)",3);
   glutAddMenuEntry("Octahedron",4);
   glutAddMenuEntry("Dodecahedron",5);
   glutAddMenuEntry("Icosahedron",6);
   glutAddMenuEntry("Tea Pot",7);
   
   imenuQuit = glutCreateMenu(menuQuit);

   // Create the menu
   glutCreateMenu(menuMain);
   glutAddSubMenu("Add Object", imenuAdd);
   glutAddMenuEntry("Quit",imenuQuit);

   // The menu is attached to a mouse button.
   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void initializeGL(int argc, char **argv)
{
    // GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);  // Double buffer (gets rid of flickering)
    glutInitWindowSize(width, height);
    glutInitWindowPosition(300, 100); 
    glutCreateWindow("RPI - Simulator");
    //glutSetIconTitle("I");
    glutDisplayFunc(drawScene); 
    glutReshapeFunc(resize);  
    glutKeyboardFunc(keyInput);                     // Keyboard input
    glutMouseFunc(mousePressEvent);                 // Mouse press
    glutMotionFunc(mouseMoveEvent);                 // Active mouse motion
    glutPassiveMotionFunc(mousePassiveMoveEvent);   // Passive mouse motion
    
    glEnable(GL_DEPTH_TEST);    // Enable depth testing.
    glEnable(GL_SCISSOR_TEST);  // For separating the viewports. 
    
    CamInit[0]=-6.0;        CamInit[1]=-10.3205;    CamInit[2]=6.0;
    Camera[0]=CamInit[0];   Camera[1]=CamInit[1];   Camera[2]=CamInit[2]; 
    X[0] = 1.0;             Y[1] = 1.0;             Z[2] = 1.0;  // Unit vectors
    
    
//                            Body_trimesh tet = Body_trimesh("meshes/tetrahedron.poly");
//                            tet.setPosition(0.,0.,1.);
//                            tet.setStatic(true);
//                            tet.updateWorld_Verts();
//                            SIM.addBody(tet); 
//                            
//                            Body_trimesh tet2 = Body_trimesh("meshes/tetrahedron.poly");
//                            tet2.setPosition(0.1,1.,0.);
//                            tet2.setStatic(true);
//                            tet2.updateWorld_Verts();
//                            SIM.addBody(tet2); 
//
//                            Body_sphere sph = Body_sphere();
//                            sph.setRadius(0.2);
//                            sph.setPosition(0,0.1,2);
//                            SIM.addBody(sph); 
//    // Make a tetrahedron grid
//    for (double x=-6; x<=6; x+=2){
//        for (double y=-6; y<=6; y+=2) {
//            Body_trimesh tet = Body_trimesh("meshes/icosahedron.poly");
//            tet.setPosition(x,y,2);
//            tet.setStatic(true);
//            tet.updateWorld_Verts();
//            SIM.addBody(tet); 
//        }
//    }
//    // Make some spheres
//    for (double x=-4; x<=4; x+=2){
//        for (double y=-6; y<=6; y+=2) {
//            Body_sphere sph = Body_sphere();
//            //sph.setRadius(0.8);
//            x += (rand()%1000)/2000.0;
//            y += (rand()%1000)/2000.0;
//            sph.setPosition(x,y,4.0);
//            SIM.addBody(sph);
//        }
//    }
//    for (double x=-4; x<=4; x+=2){
//        for (double y=-6; y<=6; y+=2) {
//            Body_sphere sph = Body_sphere();
//            //sph.setRadius(0.8);
//            x += (rand()%1000)/2000.0;
//            y += (rand()%1000)/2000.0;
//            sph.setPosition(x,y,7.0 + (rand()%1000)/2000.0 );
//            SIM.addBody(sph);
//        }
//    }
//    for (double x=-1; x<=1; x+=1){
//        for (double y=-2; y<=2; y+=1) {
//            Body_sphere sph = Body_sphere();
//            //sph.setRadius(0.8);
//            x += (rand()%1000)/2000.0;
//            y += (rand()%1000)/2000.0;
//            sph.setPosition(x,y,9.0 + (rand()%1000)/2000.0 );
//            SIM.addBody(sph);
//        }
//    }
    
    updateLookVectors();    
    makeMenu(); 
    drawScene();
    
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

// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string) {  
   char *c;
   for (c = string; *c != NULL; c++) glutBitmapCharacter(font, *c);
}

void worldLighting() {
    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);

//    // Light property vectors.
//    float lightAmb0[] =         { 0.5, 0.5, 0.5, 1.0 };
//    float lightDifAndSpec0[] =  { 0.5, 0.5, 0.5, 1.0 };
//    float globAmb0[] =          { 0.2, 0.2, 0.2, 1.0 };
//    float lightPos0[] =         { 2.0, 2.0, 5.0, 1.0 };
//
//    // Light0 properties.
//    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb0);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);
//    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0); 
//    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb0); // Global ambient light.
    
    
       // Light property vectors.
   float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
   float lightDifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
   float lightPos0[] = { 0.0, 1.5, 3.0, 1.0 };
   float globAmb[] = { 0.2, 0.2, 0.2, 1.0 };
    // Light properties.
   glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
   glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);
   glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);   
   
   glEnable(GL_LIGHT0); // Enable particular light source.
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); // Global ambient light.
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Enable two-sided lighting.
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint.
    
    //glEnable(GL_LIGHT0); // Enable particular light source.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint

    // Cull back faces.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Draw sphere at light
//    glDisable(GL_LIGHTING);
//    glPushMatrix();
//        glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
//        glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
//        glColor3f(0.0, 1.0, 0.0); 
//        glutWireSphere(0.05, 8, 8);
//    glPopMatrix();
//    glEnable(GL_LIGHTING);
}

// Draw the panel on the left
void draw_PANEL_left() {
    glViewport(0,0,LeftPanelWidth,height); 
    glScissor(0,0,LeftPanelWidth,height);
    glClearColor(0.4453f, 0.4453f, 0.4453f, 0.f);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-LeftPanelWidth/2.0, LeftPanelWidth/2.0, 
            -height/2.0, height/2.0, 
            0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);  // Don't light text 
    glColor3f(0.0,0.0,0.0);  // Black font
    
    int d = -15;
    int os = 15; 
    glRasterPos3f(-LeftPanelWidth/2.0 + edge_buffer,height/2.0+d,0.0); 
    writeBitmapString((void*)font_left_panel, "Active Body Information");
    d-=os; 
    
    if (SIM.activeBody_type() >= 0) {
        // Body name
        glRasterPos3f(-LeftPanelWidth/2.0 + edge_buffer,height/2.0+d,0.0); 
        writeBitmapString((void*)font_left_panel, SIM.text_activeBodyName());
        d-=os; 
        
        // Static body status
        glRasterPos3f(-LeftPanelWidth/2.0 + edge_buffer,height/2.0+d,0.0); 
        writeBitmapString((void*)font_left_panel, SIM.text_activeBodyIsStatic());
        d-=os; 

        // Position 
        glRasterPos3f(-LeftPanelWidth/2.0 + edge_buffer,height/2.0+d,0.0); 
        writeBitmapString((void*)font_left_panel, SIM.text_activeBodyPosition());
        d-=os; 

        // Quaternion 
        glRasterPos3f(-LeftPanelWidth/2.0 + edge_buffer,height/2.0+d,0.0); 
        writeBitmapString((void*)font_left_panel, SIM.text_activeBodyRotation());
        d-=os; 
    }
}

// Draw the main panel, including the simulated bodies
void draw_PANEL_main() {
    glViewport(LeftPanelWidth,0,width-LeftPanelWidth,height);
    glScissor(LeftPanelWidth,0,width-LeftPanelWidth,height);
    glClearColor(0.2235f, 0.2235f, 0.2235f, 1.0f);   // Blender color
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear
    
    // Wire-frame or not
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE);
    }
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60.0,
                       ((double)width-(double)LeftPanelWidth)/(double)height,
                       0.1,
                       500);
    
    // Rotate camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(Camera[0], Camera[1], Camera[2],  // Camera position
              CamLookAt[0],CamLookAt[1],CamLookAt[2], 
              0,0,1);  // Z-direction is up

        // Draw sphere at CamLookAt
//        glPushMatrix();
//        glTranslatef(CamLookAt[0],CamLookAt[1],CamLookAt[2]);
//        glutWireSphere(.1,20,20);
//        glPopMatrix();

    worldLighting();            // Lighting
    drawGrid();                 // Grid
    SIM.draw(wireframe);        // Simulation objects
}

// Display information about simulation properties, e.g. solver time, framerate, etc.
void draw_PANEL_simInfo() {
    glViewport(LeftPanelWidth, height-InfoPanelHeight, InfoPanelWidth, InfoPanelHeight);
    glScissor(LeftPanelWidth, height-InfoPanelHeight, InfoPanelWidth, InfoPanelHeight);
//    glClearColor(0.2529f, 0.2529f, 0.2529f, 0.f);
//    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-InfoPanelWidth/2.0, InfoPanelWidth/2.0, 
            -InfoPanelWidth/2.0, InfoPanelWidth/2.0, 
            0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);  // Don't light text 
    glColor3f(1.0,1.0,1.0);  // Black font
    
    int d = 9;
    int os = 18; 
    
    glRasterPos3f(-InfoPanelWidth/2.0 + edge_buffer, InfoPanelHeight/2.0+d, 0.0); 
    writeBitmapString((void*)font_simInfo, "::: SIMULATION INFORMATION :::");
    d-=os;
    
    glRasterPos3f(-InfoPanelWidth/2.0 + edge_buffer, InfoPanelHeight/2.0+d, 0.0); 
    writeBitmapString( (void*)font_simInfo, SIM.Time_dynamics() );
    d-=os; 
    
    glRasterPos3f(-InfoPanelWidth/2.0 + edge_buffer, InfoPanelHeight/2.0+d, 0.0); 
    writeBitmapString( (void*)font_simInfo, SIM.Time_collision_detection() );
    d-=os;
    
    glRasterPos3f(-InfoPanelWidth/2.0 + edge_buffer, InfoPanelHeight/2.0+d, 0.0); 
    writeBitmapString( (void*)font_simInfo, SIM.Solver_iterations() );
    d-=os;
    
    glRasterPos3f(-InfoPanelWidth/2.0 + edge_buffer, InfoPanelHeight/2.0+d, 0.0); 
    writeBitmapString( (void*)font_simInfo, SIM.Time_kinematic_update() );
    d-=os;
    
    glRasterPos3f(-InfoPanelWidth/2.0 + edge_buffer, InfoPanelHeight/2.0+d, 0.0); 
    writeBitmapString( (void*)font_simInfo, SIM.Time_graphics() );
    d-=os;
}

// Drawing routine.
void drawScene(void)
{  
    draw_PANEL_main();
    if (drawSimInfo)
        draw_PANEL_simInfo();
    draw_PANEL_left();
    
    glutSwapBuffers();
}


// OpenGL window reshape routine.
void resize(int w, int h)
{
    width = w; 
    height = h;
}

// Function pointer to serve as an independent thread 
void *runSim(void *threadid)
{
   long tid;
   tid = (long)threadid;
   cout << "New thread.  Thread ID, " << tid << endl;
   
   cout << "Starting SIM... " << endl;
   while(SIM.isRunning()) {
        SIM.step();
        cout << "Stepping ..." << endl;
        //drawScene();   <-- Won't work unless context is changed
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
           
       case 99: // c    toggle drawcontacts
           SIM.toggleDrawContacts();
           drawScene(); 
           break;
           
       case 105: // i   toggle draw simulation information
           drawSimInfo = !drawSimInfo; 
           drawScene(); 
           break;
           
       case 112: // p
           SIM.printBodies(); 
           break;
           
       case 9: // [tab]
           wireframe = !wireframe; 
           drawScene();
           break;
           
       case 103: // g   Move the active object
           if (SIM.activeBody_type() < 0) break;  // If there are no bodies, do nothing.
           obj_Ui = SIM.activeBodyPosition(); 
           camXi = x;
           camYi = y;
           enableObjectMove = true; 
           moveState = objMOVE;                 
           mousePassiveMoveEvent(x, y);
           break;
           
       case 113: // q   quit
           exit(0);
           break; 
           
       case 115: // s   toggle active body static 
           if (SIM.activeBody_type() < 0) break;  // If there are no bodies, do nothing.
           SIM.toggleActiveBodyStatic(); 
           drawScene();  
           break;
           
       case 118: // v   toggle active body visible
           if (SIM.activeBody_type() < 0) break;  // If there are no bodies, do nothing.
           SIM.toggleActiveBodyVisible(); 
           drawScene();
           break;

       case 120: // x   Move object on x axis
           if (enableObjectMove) {
               moveState = objMOVE_X;           
               mousePassiveMoveEvent(x, y);
           }
           break;
           
       case 121: // y   Move object on y axis
           if (enableObjectMove) {
                moveState = objMOVE_Y;           
                mousePassiveMoveEvent(x, y);
           }
           break;
           
       case 122: // z   Move object on z axis 
           if (enableObjectMove) {
               moveState = objMOVE_Z;
               mousePassiveMoveEvent(x, y);
           }
           break; 
           
       case 27: // [ESC]   Cancels whatever action is being done
           if(enableObjectMove) {
               enableObjectMove =! enableObjectMove; 
               moveState = 0; 
               // Put active object back where it started 
               SIM.setActiveBodyPosition(obj_Ui); 
               drawScene(); 
           }
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
//    cout << "Shift: " << glutGetModifiers() << endl ;
    
    int modifier = glutGetModifiers();  // No shift nor control -> 0
                                        // Shift -> 1
                                        // Ctrl -> 2
                                        // Shift && Ctrl -> 3
    // Begin rotation 
    if (button == MiddleClick && state == 0 && modifier == 0) { 
        camXi = x;
        camYi = y;
        camRXp = CamXrot;
        camRYp = CamZrot;
        enableMouseRotation = true;
    }
    // Begin translation 
    else if (button == MiddleClick && state == 0 && modifier == Shift) { 
        camXi = x;
        camYi = y; 
        CamLookAtPrev = CamLookAt; 
        CameraPrev = Camera; 
        updateLookVectors();   // This might not be necessary here 
        enableMouseTranslation = true; 
    }
    // Disable mouse rotation and translation 
    else if ((button == LeftClick || button == MiddleClick) && state == 1 ) {
        enableMouseRotation = false; 
        enableMouseTranslation = false; 
        updateLookVectors(); 
    }
    // Finished placing a moving object
    else if (button == LeftClick) {
        if (enableObjectMove) {
            enableObjectMove = !enableObjectMove; 
            moveState = 0; 
        }
    }
    // Zoom in
    else if (button == 3) {
        camZoomFactor *= 0.9;   // Zoom in
        updateCameraPosition(); // Update camera position 
        drawScene();            // Redraw from new camera position 
    }
    // Zoom out
    else if (button == 4) {
        camZoomFactor *= 1.1;   // Zoom out
        updateCameraPosition(); // Update camera position 
        drawScene();            // Redraw from new camera position 
    }
} 

void mouseMoveEvent(int x, int z)
{
    if (enableMouseRotation) {          // Rotate entire scene
        CamXrot = camRXp + (camYi-z)*rotScale;  // TODO limit within range 
        CamZrot = camRYp + (camXi-x)*rotScale; 
        updateCameraPosition();  // TODO: Not necessary here?
    }
    else if (enableMouseTranslation) {  // Translate entire scene
        dz = camYi-z;
        dx = x-camXi; 
        double tfact = 70.0/camZoomFactor;  // Translation factor.  
        Camera = CameraPrev - CamRight*(dx/tfact) - CamUp*(dz/tfact);
        CamLookAt = CamLookAtPrev - CamRight*(dx/tfact) - CamUp*(dz/tfact);
        drawScene(); 
    }
}

void mousePassiveMoveEvent(int x, int z) {
    if (enableObjectMove && moveState == objMOVE) { // Move object tangent to camera
        dz = camYi-z;   
        dx = x-camXi; 
        vec Unew = obj_Ui + CamRight*(dx/objMoveFactor*camZoomFactor) + CamUp*(dz/objMoveFactor*camZoomFactor);
        SIM.setActiveBodyPosition(Unew); 
        drawScene(); 
    }
    else if (enableObjectMove && moveState == objMOVE_X) { // Move object on X axis
        dz = camYi-z;   
        dx = x-camXi; 
        vec Unew = obj_Ui + X*(dx/objMoveFactor*camZoomFactor);
        SIM.setActiveBodyPosition(Unew); 
        drawScene(); 
    }
    else if (enableObjectMove && moveState == objMOVE_Y) { // Move object on Y axis
        dz = camYi-z;   
        dx = x-camXi; 
        vec Unew = obj_Ui + Y*(dx/objMoveFactor*camZoomFactor);
        SIM.setActiveBodyPosition(Unew); 
        drawScene(); 
    }
    else if (enableObjectMove && moveState == objMOVE_Z) { // Move object on Z axis
        dz = camYi-z;   
        dx = x-camXi; 
        vec Unew = obj_Ui + Z*(dz/objMoveFactor*camZoomFactor);
        SIM.setActiveBodyPosition(Unew); 
        drawScene(); 
    }
}

// Update the vectors that describe the camera view
void updateLookVectors() {
        CamLook = CamLookAt - Camera;  
        CamRight = cross(CamLook,Z); 
        CamUp = cross(CamRight,CamLook);
        
        CamLook = CamLook / norm(CamLook,2);        // Normalize vectors
        CamRight = CamRight / norm(CamRight,2); 
        CamUp = CamUp / norm(CamUp,2); 
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
    Camera[0] = r(0) + CamLookAt[0];
    Camera[1] = r(1) + CamLookAt[1];
    Camera[2] = r(2) + CamLookAt[2]; 
    
    drawScene();
}

// Main method 
int SimulationEnvironment::start() {
    glutMainLoop(); 
    return 0;  
}

