/* 
 * File:   Simulation.cpp
 * Author: carser
 * 
 * Created on September 28, 2012, 8:00 PM
 */

#include "Simulation.h"
#include "Body_trimesh.h"

Simulation::Simulation() {
}

Simulation::Simulation(const Simulation& orig) {
}

Simulation::~Simulation() {
}


const Body_sphere&  Simulation::getOneSingleSphere() { return oneSingleSphere; } 

double Simulation::getRad() { return oneSingleSphere.radius(); }

void Simulation::setOneSingleSphereRadius(double r) { oneSingleSphere.setRadius(r); } 

void Simulation::draw() { 
    //oneSingleSphere.draw(); 
}


bool Simulation::addBody(Body_object body) { Bodies.push_back(body); }
bool Simulation::removeBody(int bodyID) {}

void Simulation::addCube() {
   
}
