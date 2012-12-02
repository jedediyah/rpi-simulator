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
#include <time.h>
#include <math.h>
#include <sstream>

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
    
    void addSphere();
    void addTetrahedron();
    void addCube();
    void addOctahedron();
    void addDodecahedron();
    void addIcosahedron();
    void addTeaPot();
    
    void printBodies();
    
    void setRun(bool r);
    bool isRunning();
    void step();
    int  stepCounter();
    
    void setActiveBody(int body_type, int body_index);
    void setActiveBodyPosition(vec u);
    vec activeBodyPosition(); 
    char* text_activeBodyName();  
    char* text_activeBodyPosition(); 
    char* text_activeBodyRotation();
    char* text_activeBodyIsStatic();
    int activeBody_type();
    int activeBody_index(); 
    
    void tic();
    double toc();
    
    char* Time_collision_detection(); 
    char* Time_dynamics();
    char* Time_kinematic_update();
    char* Time_graphics();
    char* Solver_iterations();  
    
    void toggleDrawContacts();
    void toggleActiveBodyStatic();
    void toggleActiveBodyVisible();

private:
    
    //SimulationEnvironment PARENT; won't work
    
    bool running; 
    double step_size; 
    int Step_Counter; 
    
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
    
    // Timers for display 
    clock_t TIME;   // For timing, used by tic() and toc()
    double timer_collision_detection;
    double timer_dynamics; 
    double timer_kinematic_update; 
    double timer_graphics;
    
    bool drawContacts; 
    

};

#endif	/* SIMULATION_H */

