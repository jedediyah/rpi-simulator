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

    void draw(); 
    
    bool addBody(Body_object &body); 
    bool addBody(Body_sphere &body);
    bool addBody(Body_trimesh &body);  
    
    bool removeBody(int bodyID); 
    
    void addCube(); 
    void addSphere();
    
private:
    //Body_sphere oneSingleSphere; 
    
    
    // Currently, I'll be working only with these body types.
    // It would be nice if they could all be in the same vector.  Alas. 
//    vector<Body_object> Bodies; 
//    vector<Body_trimesh> Mesh_Bodies;
//    vector<Body_sphere> Sphere_Bodies; 
    
    int TotalBodyCount;
    
    Body_object Bodies[100];
    int Num_Bodies; 
    Body_trimesh Trimesh_Bodies[100];
    int Num_Trimeshes;
    Body_sphere Sphere_Bodies[100];
    int Num_Spheres; 

};

#endif	/* SIMULATION_H */

