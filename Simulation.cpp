/* 
 * File:   Simulation.cpp
 * Author: Jedediyah Williams
 * 
 * Created on September 28, 2012, 8:00 PM
 */

#include "Simulation.h"
#include "cdaDynamics.h"
#include "lcpDynamics.h"

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

string d2s(double d);

const char PATH_LICENSE[] = "2391033789&Courtesy_License&&&USR&2010&7_1_2010&1000&PATH&GEN&31_12_2010&0_0_0&0&0_0";

Simulation::Simulation() {
    step_size = 0.01; 
    TotalBodyCount = 0;
    Num_Bodies = 0; 
    Num_Trimeshes = 0;
    Num_Spheres = 0;
    num_contacts = 0;
    running = false; 
    ActiveBody_Type = -1; 
    drawContacts = false;        // TODO: Init as false, and toggle on 'c' 
}

Simulation::Simulation(const Simulation& orig) {
}

Simulation::~Simulation() {
}

//void Simulation::setParent(SimulationEnvironment parent) { PARENT = parent; }
//SimulationEnvironment Simulation::parent() { return PARENT; }

int Simulation::num_bodies()            { return Num_Bodies; }
int Simulation::num_spheres()           { return Num_Spheres; }
int Simulation::num_trimeshes()         { return Num_Trimeshes; }

void Simulation::setActiveBody( int body_type, int body_index ) {
    ActiveBody_Type = body_type;
    ActiveBody_Index = body_index; 
}

void Simulation::setActiveBodyPosition(vec u) {
    if (ActiveBody_Type == SPHERE) {
        Sphere_Bodies[ActiveBody_Index].setPosition(u[0],u[1],u[2]); 
    }
    else if (ActiveBody_Type == TRIMESH) {
        Trimesh_Bodies[ActiveBody_Index].setPosition(u[0],u[1],u[2]); 
        Trimesh_Bodies[ActiveBody_Index].updateWorld_Verts(); 
    }
}
vec Simulation::activeBodyPosition()    { 
    if (ActiveBody_Type == SPHERE) {
        return Sphere_Bodies[ActiveBody_Index].u();
    }
    else if (ActiveBody_Type == TRIMESH) {
        return Trimesh_Bodies[ActiveBody_Index].u();
    }
}
int Simulation::activeBody_type()       { return ActiveBody_Type; }
int Simulation::activeBody_index()      { return ActiveBody_Index; }

void Simulation::printBodies() {
    
    for (int i=0; i<Num_Trimeshes; i++) { // Draw all Trimeshes
        Trimesh_Bodies[i].printAllData(); 
    }
    
    for (int i=0; i<Num_Spheres; i++) {   // Draw all Spheres
        Sphere_Bodies[i].print();
    }
}

void Simulation::draw(bool wireframe) { 
    
//    // Material property vectors.
//    float matAmb[] = {0.0, 0.0, 1.0, 1.0};
//    float matDif[] = {0.0, 0.0, 1.0, 1.0};
//    float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
//    float matShine[] = { 50.0 };
//    float matEmission[] = {0.0, 0.0, 0.0, 1.0};
//       // Material properties.
//    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
//    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
//    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
//    glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
    
   // Material property vectors.
   float matAmbAndDif1[] = {0.9, 0.0, 0.0, 1.0};
   float matAmbAndDif2[] = {0.0, 0.9, 0.0, 1.0};
   float matSpec[] = {1.0, 1.0, 1.0, 1.0};
   float matShine[] = {50.0};
   // Material properties 
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    
    for (int i=0; i<Num_Trimeshes; i++) { // Draw all Trimeshes
        Trimesh_Bodies[i].draw();
    }
    
    for (int i=0; i<Num_Spheres; i++) {   // Draw all Spheres
        Sphere_Bodies[i].draw(wireframe);
    }
   
    
    // Draw contacts
   if (drawContacts) {
    for (int c=0; c<num_contacts; c++) {
        vec::fixed<3> p1, p2;

        // p2
        if (Contacts[c].body2_type == SPHERE)
            p2 = Sphere_Bodies[Contacts[c].body2].u() + Contacts[c].r2;
        else if (Contacts[c].body2_type == TRIMESH)
            p2 = Trimesh_Bodies[Contacts[c].body2].u() + Contacts[c].r2;

        // p1
        if (Contacts[c].body1_type == SPHERE)
            p1 = Sphere_Bodies[Contacts[c].body1].u() + Contacts[c].r1;
        else if (Contacts[c].body1_type == TRIMESH)
            p1 = Trimesh_Bodies[Contacts[c].body1].u() + Contacts[c].r1;
        else if (Contacts[c].body1_type == GROUND) {
            p1 = p2;       // If ground, then same as p2
            p1[2] = 0.0;   // but with z value of zero
        }

        // Draw contact line
        glDisable(GL_LIGHTING);
        glColor3f(1.,1.,1.);
        glBegin(GL_LINES);
         glVertex3f(p1[0],p1[1],p1[2]);
         glVertex3f(p2[0],p2[1],p2[2]);
        glEnd();
    }
   }

}

// Body object
bool Simulation::addBody(Body_object &body) { 
    Bodies[Num_Bodies++] = body; 
    TotalBodyCount++;
    return true;
}

// Add SPHERE
bool Simulation::addBody(Body_sphere &body) {
    ActiveBody_Type = SPHERE;
    ActiveBody_Index = Num_Spheres; 
    Sphere_Bodies[Num_Spheres++] = body;
    TotalBodyCount++;
    return true;
}

// Add TRIMESH
bool Simulation::addBody(Body_trimesh &body) {
    ActiveBody_Type = TRIMESH;
    ActiveBody_Index = Num_Trimeshes;        
    
    vec color = zeros(3);
    color[0] = (rand()%1000)/1000.0;    // Assign random color 
    color[1] = (rand()%1000)/1000.0;
    color[2] = (rand()%1000)/1000.0;
    body.setColor(color);
    
    Trimesh_Bodies[Num_Trimeshes++] = body; 
    TotalBodyCount++;
    return true;
}


bool Simulation::removeBody(int bodyID) {

}

void Simulation::addSphere() {
    Body_sphere sphere = Body_sphere();
    
//    if ( Num_Spheres == 1 ) {
//        sphere.setRadius(1.0);
//        sphere.setPosition(0, 0, 7);
//        sphere.setName("s2"); 
//        sphere.setFext(0,0,-9.8,0,0,0);
//        //sphere.setQuaternion(.0707, .2666, .5332, .7998);
//    }
//    else if (Num_Spheres == 2 ) {
//        sphere.setRadius(1.0);
//        sphere.setPosition(-.3,0,10.3);
//        sphere.setName("s3"); 
//        sphere.setFext(0,0,-9.8,0,0,0);
//        //sphere.setVelocity(0,2,0,0,0,0);
//        //sphere.setStatic(true); 
//    }
//    else {
////        sphere.setPosition(-14,-5,4.0);
////        sphere.setRadius(3.0);
//        sphere.setVelocity(0,0,0,0,0,0);
//        sphere.setPosition(.2,0,3.5);
//        //sphere.setFext(0,0,-9.8,0,0,0);
//        sphere.setRadius(1.5);
//        sphere.setStatic(true); 
//        sphere.setName("s1"); 
//    }
    this->addBody(sphere);  
}

void Simulation::addTetrahedron() {
    Body_trimesh tetrahedron = Body_trimesh("meshes/tetrahedron.poly");
    this->addBody(tetrahedron); 
}

void Simulation::addCube() {
//    if (Num_Trimeshes == 0) {
//        Body_trimesh cube = Body_trimesh("meshes/cube.poly");
//        //cube.scale(2.0); 
//        cube.setVelocity(0.5,0.,0.,1,2,.3);
//        cube.initializeGL(); 
//        cube.setQuaternion(.9888, .0399, .0799, .1198); 
//        cube.updateWorld_Verts();
//        cube.setName("c1");
//        cube.setPosition(-0.0, 0.0, 0.0);
//        this->addBody(cube);
//    }
//    else {
//        Body_trimesh cube = Body_trimesh("meshes/cube.poly");
//        cube.scale(5.0); 
//        cube.setPosition(12.0, -4.0, 0.0);
//        cube.setVelocity(-0.3,0.,0.,1,2,.3);
//        cube.initializeGL(); 
//        cube.setQuaternion(.9888, .0399, .0799, .1198); 
//        cube.updateWorld_Verts();
//        cube.setName("c2"); 
//        this->addBody(cube);
//    }
    Body_trimesh cube = Body_trimesh("meshes/cube.poly");
    this->addBody(cube);
}

    
void Simulation::addOctahedron() {
    Body_trimesh octahedron = Body_trimesh("meshes/octahedron.poly");
    this->addBody(octahedron); 
}
void Simulation::addDodecahedron() {
    Body_trimesh dodecahedron = Body_trimesh("meshes/dodecahedron.poly");
    this->addBody(dodecahedron);   
}
void Simulation::addIcosahedron() {
    Body_trimesh icosahedron = Body_trimesh("meshes/icosahedron.poly");
    this->addBody(icosahedron);   
}
void Simulation::addTeaPot() {
    Body_trimesh teapot = Body_trimesh("meshes/teapot2.obj");
    this->addBody(teapot);   
    cout << "teapot added" << endl; 
}

void Simulation::setRun(bool r) {
    running = r; 
}

bool Simulation::isRunning() { return running; }

void Simulation::step() {  // dt is now step_size...
    
    // Collision detection 
    tic();
    Step_Counter++; 
    CD.findCollisions(Contacts, Num_Bodies, num_contacts, num_subcontacts, 
                                Sphere_Bodies, Num_Spheres, 
                                Trimesh_Bodies, Num_Trimeshes); 
    timer_collision_detection = toc();
    
    tic();
    vec z;  // Will hold the result of the dynamics 
    //cdaDynamics(Contacts, Sphere_Bodies, Trimesh_Bodies, Num_Bodies, num_contacts, num_subcontacts );
    z = lcpDynamics(Contacts, Sphere_Bodies, Num_Spheres, Trimesh_Bodies, Num_Trimeshes,
                Num_Bodies, num_contacts, step_size );
    timer_dynamics = toc(); 
    
    tic();
    // Kinematic update: update each object's NU, then step. 
//    for (int i=0; i<Num_Trimeshes; i++) { 
//        Trimesh_Bodies[i].stepDynamics(step_size);
//        Trimesh_Bodies[i].updateWorld_Verts();  
//    }
    
    int bID; 
    // Spheres
    for (int i=0; i<Num_Spheres; i++) {   
        if ( !Sphere_Bodies[i].isStaticBody() ) {
            if ( Sphere_Bodies[i].ContactCount > 0 ) { // If !static, and had contact
                bID = Sphere_Bodies[i].BodyIndex;  
                Sphere_Bodies[i].setVelocity( z.at(6*bID-6), 
                                              z.at(6*bID-5),
                                              z.at(6*bID-4),
                                              z.at(6*bID-3),
                                              z.at(6*bID-2),
                                              z.at(6*bID-1));  
                Sphere_Bodies[i].stepDynamics(step_size);
            } 
            else {
                Sphere_Bodies[i].applyAext(step_size); 
                Sphere_Bodies[i].stepDynamics(step_size);
            }
        }
    }
    // Meshes
    for (int i=0; i<Num_Trimeshes; i++) {   
        if ( !Trimesh_Bodies[i].isStaticBody() ) {
            if ( Trimesh_Bodies[i].ContactCount > 0 ) { // If !static, and had contact
                bID = Trimesh_Bodies[i].BodyIndex;  
                Trimesh_Bodies[i].setVelocity( z.at(6*bID-6), 
                                              z.at(6*bID-5),
                                              z.at(6*bID-4),
                                              z.at(6*bID-3),
                                              z.at(6*bID-2),
                                              z.at(6*bID-1));  
                Trimesh_Bodies[i].stepDynamics(step_size);
            } 
            else {
                Trimesh_Bodies[i].applyAext(step_size); 
                Trimesh_Bodies[i].stepDynamics(step_size);
            }
            Trimesh_Bodies[i].updateWorld_Verts();  // Annoying to forget
        }
    }
    timer_kinematic_update = toc();
    
    tic();
    draw(false);
    timer_graphics = toc();
    
//    while (running) {
//        cout << " Sim, running... " << endl; 
//    }
    
}

int Simulation::stepCounter() { return Step_Counter; }

char* Simulation::text_activeBodyName() {
    string outputString;
    if (ActiveBody_Type == SPHERE) {
        outputString = "Name: " + Sphere_Bodies[ActiveBody_Index].name();
    }
    else if (ActiveBody_Type == TRIMESH) {
        outputString = "Name: " + Trimesh_Bodies[ActiveBody_Index].name();
    }
    return (char*)outputString.c_str(); 
}
char* Simulation::text_activeBodyPosition() {
    string outputString;
    if (ActiveBody_Type == SPHERE) {
        outputString = "Position: (" + 
                d2s(Sphere_Bodies[ActiveBody_Index].u()[0]) + ", " + 
                d2s(Sphere_Bodies[ActiveBody_Index].u()[1]) + ", " + 
                d2s(Sphere_Bodies[ActiveBody_Index].u()[2]) + ")"; 
    }
    else if (ActiveBody_Type == TRIMESH) {
         outputString = "Position: (" + 
                d2s(Trimesh_Bodies[ActiveBody_Index].u()[0]) + ", " + 
                d2s(Trimesh_Bodies[ActiveBody_Index].u()[1]) + ", " + 
                d2s(Trimesh_Bodies[ActiveBody_Index].u()[2]) + ")"; 
    }
    return (char*)outputString.c_str(); 
}
char* Simulation::text_activeBodyRotation() {
    string outputString;
    if (ActiveBody_Type == SPHERE) {
        outputString = "Quaternion: (" + 
                d2s(Sphere_Bodies[ActiveBody_Index].quat()[0]) + ", " +
                d2s(Sphere_Bodies[ActiveBody_Index].quat()[1]) + ", " + 
                d2s(Sphere_Bodies[ActiveBody_Index].quat()[2]) + ", " + 
                d2s(Sphere_Bodies[ActiveBody_Index].quat()[3]) + ")"; 
    }
    else if (ActiveBody_Type == TRIMESH) {
         outputString = "Quaternion: (" + 
                d2s(Trimesh_Bodies[ActiveBody_Index].quat()[0]) + ", " + 
                d2s(Trimesh_Bodies[ActiveBody_Index].quat()[1]) + ", " +
                d2s(Trimesh_Bodies[ActiveBody_Index].quat()[2]) + ", " + 
                d2s(Trimesh_Bodies[ActiveBody_Index].quat()[3]) + ")"; 
    }
    return (char*)outputString.c_str();     
}
char* Simulation::text_activeBodyIsStatic() {
    string outputString;
    bool isStatic;
    if (ActiveBody_Type == SPHERE) {
        outputString = "Static Body: ";
        isStatic = Sphere_Bodies[ActiveBody_Index].isStaticBody();
    }
    else if (ActiveBody_Type == TRIMESH) {
        outputString = "Static Body: ";
        isStatic = Trimesh_Bodies[ActiveBody_Index].isStaticBody();
    }
    if (isStatic)
        outputString += "TRUE";
    else
        outputString += "FALSE"; 
    return (char*)outputString.c_str();   
}

// tic and toc: behavior similar to MATLAB's
void Simulation::tic() {
    TIME = clock(); 
}
double Simulation::toc() {
    return (clock() - TIME) / (double) CLOCKS_PER_SEC;  
} 

char* Simulation::Time_collision_detection() {
    string outputString = "";  
    std::stringstream ss;
    ss << timer_dynamics; 
    outputString = ss.str(); 
    outputString = "Dynamics time: " + outputString;  
    return (char*) outputString.c_str();  
}
char* Simulation::Time_dynamics() {
    string outputString = "";  
    std::stringstream ss;
    ss << timer_collision_detection; 
    outputString = ss.str(); 
    outputString = "Collision detection time: " + outputString;  
    return (char*) outputString.c_str();  
}
char* Simulation::Time_graphics() {
    string outputString = "";  
    std::stringstream ss;
    ss << timer_graphics; 
    outputString = ss.str(); 
    outputString = "Graphics time: " + outputString;  
    return (char*) outputString.c_str();  
}
char* Simulation::Time_kinematic_update() {
    string outputString = "";  
    std::stringstream ss;
    ss << timer_kinematic_update; 
    outputString = ss.str(); 
    outputString = "Kinematic update time: " + outputString;  
    return (char*) outputString.c_str();
}
char* Simulation::Solver_iterations() {
    string outputString = "";  
    std::stringstream ss;
    ss << 0.0;          // TODO get iterations
    outputString = ss.str(); 
    outputString = "Solver iterations: " + outputString;  
    return (char*) outputString.c_str();     
}

void Simulation::toggleDrawContacts() { drawContacts = !drawContacts; }
void Simulation::toggleActiveBodyStatic() {
    if (ActiveBody_Type == SPHERE) {
        Sphere_Bodies[ActiveBody_Index].setStatic(!Sphere_Bodies[ActiveBody_Index].isStaticBody());
    }
    else if (ActiveBody_Type == TRIMESH) {
        Trimesh_Bodies[ActiveBody_Index].setStatic(!Trimesh_Bodies[ActiveBody_Index].isStaticBody());
    }
}
void Simulation::toggleActiveBodyVisible() {
    if (ActiveBody_Type == SPHERE) {
        Sphere_Bodies[ActiveBody_Index].setVisible(!Sphere_Bodies[ActiveBody_Index].isVisible());
    }
    else if (ActiveBody_Type == TRIMESH) {
        Trimesh_Bodies[ActiveBody_Index].setVisible(!Trimesh_Bodies[ActiveBody_Index].isVisible());
    }
}

// Convert a double to a string
string d2s(double d) {
    std::stringstream ss;
    ss << d;
    return ss.str();
}


