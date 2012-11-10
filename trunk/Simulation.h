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

#ifndef SIMULATION_H
#define	SIMULATION_H

class Simulation {
public:
    Simulation();
    Simulation(const Simulation& orig);
    virtual ~Simulation();

    void draw(bool wireframe); 
    
    bool addBody(Body_object &body); 
    bool addBody(Body_sphere &body);
    bool addBody(Body_trimesh &body);  
    
    bool removeBody(int bodyID); 
    
    void addCube(); 
    void addSphere();
    
private:
    
    int TotalBodyCount;
    
    Body_object Bodies[100];
    int Num_Bodies; 
    Body_trimesh Trimesh_Bodies[100];
    int Num_Trimeshes;
    Body_sphere Sphere_Bodies[100];
    int Num_Spheres; 

};

#endif	/* SIMULATION_H */

