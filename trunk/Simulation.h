/* 
 * File:   Simulation.h
 * Author: carser
 *
 * Created on September 28, 2012, 8:00 PM
 */

#include <vector>
#include "Body_object.h"
#include "Body_trimesh.h"
#include "Body_sphere.h"
#include "CollisionDetection.h"
#include "Contact.h"
#include "SimulatorDefenitions.h"

#ifndef SIMULATION_H
#define	SIMULATION_H

class Simulation {
public:
    Simulation();
    Simulation(const Simulation& orig);
    virtual ~Simulation();

    //void setParent(SimulationEnvironment parent); 
    //SimulationEnvironment parent(); 
    
    int num_bodies();
    int num_spheres(); 
    int num_trimeshes();
    
    void draw(bool wireframe); 
    
    bool addBody(Body_object &body); 
    bool addBody(Body_sphere &body);
    bool addBody(Body_trimesh &body);  
    
    bool removeBody(int bodyID); 
    
    void addCube(); 
    void addSphere();
    
    void printBodies();
    
    void setRun(bool r);
    bool isRunning();
    void step();
    
    void setActiveBody(int body_type, int body_index);
    void setActiveBodyPosition(vec u);
    vec activeBodyPosition(); 
    int activeBody_type();
    int activeBody_index(); 
    
    
private:
    
    //SimulationEnvironment PARENT; won't work
    
    bool running; 
    double step_size; 
    
    int TotalBodyCount;
    
    Body_object Bodies[100];
    int Num_Bodies; 
    Body_trimesh Trimesh_Bodies[100];
    int Num_Trimeshes;
    Body_sphere Sphere_Bodies[100];
    int Num_Spheres; 
    
    CollisionDetection CD;
    Contact Contacts[1000];  // Since not dynamic, be careful to not overfill...
    int num_contacts;
    int num_subcontacts;
    
    
    int ActiveBody_Type;
    int ActiveBody_Index; 
    

};

#endif	/* SIMULATION_H */

