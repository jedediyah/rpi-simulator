/* 
 * File:   Simulation.cpp
 * Author: carser
 * 
 * Created on September 28, 2012, 8:00 PM
 */

#include "Simulation.h"
#include "cdaDynamics.h"
#include "lcpDynamics.h"

const char PATH_LICENSE[] = "2391033789&Courtesy_License&&&USR&2010&7_1_2010&1000&PATH&GEN&31_12_2010&0_0_0&0&0_0";

Simulation::Simulation() {
    step_size = 0.01; 
    TotalBodyCount = 0;
    Num_Bodies = 0; 
    Num_Trimeshes = 0;
    Num_Spheres = 0;
    num_contacts = 0;
    running = false; 
}

Simulation::Simulation(const Simulation& orig) {
}

Simulation::~Simulation() {
}

//void Simulation::setParent(SimulationEnvironment parent) { PARENT = parent; }
//SimulationEnvironment Simulation::parent() { return PARENT; }

void Simulation::printBodies() {
    for (int i=0; i<Num_Trimeshes; i++) { // Draw all Trimeshes
        Trimesh_Bodies[i].print(); 
    }
    
    for (int i=0; i<Num_Spheres; i++) {   // Draw all Spheres
        Sphere_Bodies[i].print();
    }
}

void Simulation::draw(bool wireframe) { 
    
    for (int i=0; i<Num_Trimeshes; i++) { // Draw all Trimeshes
        Trimesh_Bodies[i].draw();
    }
    
    for (int i=0; i<Num_Spheres; i++) {   // Draw all Spheres
        Sphere_Bodies[i].draw(wireframe);
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
    Sphere_Bodies[Num_Spheres++] = body;
    TotalBodyCount++;
    return true;
}

// Add TRIMESH
bool Simulation::addBody(Body_trimesh &body) {
    Trimesh_Bodies[Num_Trimeshes++] = body; 
    TotalBodyCount++;
    return true;
}


bool Simulation::removeBody(int bodyID) {

}

void Simulation::addCube() {
    if (Num_Trimeshes == 0) {
        Body_trimesh cube = Body_trimesh("meshes/cube.poly");
        //cube.scale(2.0); 
        cube.setVelocity(0.5,0.,0.,1,2,.3);
        cube.initializeGL(); 
        cube.setQuaternion(.9888, .0399, .0799, .1198); 
        cube.updateWorld_Verts();
        cube.setName("c1");
        cube.setPosition(-0.0, 0.0, 0.0);
        this->addBody(cube);
    }
    else {
        Body_trimesh cube = Body_trimesh("meshes/cube.poly");
        cube.scale(5.0); 
        cube.setPosition(12.0, -4.0, 0.0);
        cube.setVelocity(-0.3,0.,0.,1,2,.3);
        cube.initializeGL(); 
        cube.setQuaternion(.9888, .0399, .0799, .1198); 
        cube.updateWorld_Verts();
        cube.setName("c2"); 
        this->addBody(cube);
    }
   
     
}

void Simulation::addSphere() {
    Body_sphere sphere = Body_sphere();
    
    if ( Num_Spheres == 1 ) {
        sphere.setPosition(-13., 3., 1.2);
        sphere.setName("s2"); 
        sphere.setQuaternion(.0707, .2666, .5332, .7998);
    }
    else if (Num_Spheres == 2 ) {
        sphere.setRadius(2.0);
        sphere.setPosition(5,2,2.5);
        sphere.setName("s3"); 
        //sphere.setVelocity(0,2,0,0,0,0);
        //sphere.setStatic(true); 
    }
    else {
        sphere.setPosition(-14,-5,4.0);
        sphere.setRadius(3.0);
        sphere.setVelocity(1,0,0,0,0,0);
        sphere.setName("s1"); 
    }
    
    this->addBody(sphere); 
    
}

void Simulation::setRun(bool r) {
    running = r; 
}

bool Simulation::isRunning() { return running; }

void Simulation::step() {  // dt is now step_size...
    
    // Collision detection 
    CD.findCollisions(Contacts, num_bodies, num_contacts, num_subcontacts, Sphere_Bodies, Num_Spheres); 
    
    vec z;  // Will hold the result of the dynamics 
    //cdaDynamics(Contacts, Sphere_Bodies, Trimesh_Bodies, num_bodies, num_contacts, num_subcontacts );
    z = lcpDynamics(Contacts, Sphere_Bodies, Num_Spheres, Trimesh_Bodies, Num_Trimeshes,
                num_bodies, num_contacts, step_size );
    
    // Kinematic update: update each object's NU, then step. 
    for (int i=0; i<Num_Trimeshes; i++) { 
        Trimesh_Bodies[i].stepDynamics(step_size);
        Trimesh_Bodies[i].updateWorld_Verts();  
    }
    
    int bID; 
    for (int i=0; i<Num_Spheres; i++) {   // Draw all Spheres
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
                Sphere_Bodies[i].stepDynamics(step_size);
            }
        }
    }
    
    draw(false);
    
//    while (running) {
//        cout << " Sim, running... " << endl; 
//    }
    
}


