/* 
 * File:   Simulation.cpp
 * Author: carser
 * 
 * Created on September 28, 2012, 8:00 PM
 */

#include "Simulation.h"
//#include "Body_trimesh.h"


Simulation::Simulation() {
    int N = 100; // Default maximum number of bodies per array
    TotalBodyCount = 0;
    Num_Bodies = 0; 
    Num_Trimeshes = 0;
    Num_Spheres = 0;
//    Bodies = new Body_object[N];
//    Trimesh_Bodies = new Body_trimesh[N]; 
//    Sphere_Bodies = new Body_sphere[N]; 
}

Simulation::Simulation(const Simulation& orig) {
}

Simulation::~Simulation() {
}

void Simulation::draw() { 
    cout << "---------------------" << endl;
    cout << "   Drawing " << TotalBodyCount << " bodies... " << endl;
    //for (int i=0; i<Bodies.size(); i++)
    //    Bodies.at(i).draw();
//    for (int i=0; i<Mesh_Bodies.size(); i++) {
//        cout << " Name: " << Mesh_Bodies.at(i).name() << endl; 
//        Mesh_Bodies.at(i).draw();
//    }
//    for (int i=0; i<Sphere_Bodies.size(); i++) {
//        Sphere_Bodies.at(i).draw();
//        cout << " Name: " << Sphere_Bodies.at(i).name() << endl;
//    }
    for (int i=0; i<Num_Trimeshes; i++)
        Trimesh_Bodies[i].draw();
    for (int i=0; i<Num_Spheres; i++)
        Sphere_Bodies[i].draw(); 
    
}

// Body object
bool Simulation::addBody(Body_object &body) { 
//    Bodies.push_back(body); 
//    Num_Bodies++;
    
    Bodies[Num_Bodies++] = body; 
    TotalBodyCount++;
    return true;
}

// Add SPHERE
bool Simulation::addBody(Body_sphere &body) {
//    Sphere_Bodies.push_back(body);
//    Num_Bodies++;
    
    Sphere_Bodies[Num_Spheres++] = body;
    TotalBodyCount++;
    return true;
}

// Add TRIMESH
bool Simulation::addBody(Body_trimesh &body) {
//    Mesh_Bodies.push_back(body); 
//    Num_Bodies++;
    
    Trimesh_Bodies[Num_Trimeshes++] = body; 
    TotalBodyCount++;
    return true;
}


bool Simulation::removeBody(int bodyID) {

}

void Simulation::addCube() {
    Body_trimesh cube = Body_trimesh("cube.poly");
    cube.scale(13.0); 
    cube.setVelocity(0.3,0.,0.,1,2,.3);
    cube.initializeGL(); 
   
    this->addBody(cube); 
}

void Simulation::addSphere() {
    Body_sphere sphere = Body_sphere();
    sphere.setPosition(-13., 3., 0.0);
    
    this->addBody(sphere); 
    
}

