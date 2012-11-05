/* 
 * File:   Simulation.h
 * Author: carser
 *
 * Created on September 28, 2012, 8:00 PM
 */

#include <vector>
#include "Body_sphere.h"

#ifndef SIMULATION_H
#define	SIMULATION_H

class Simulation {
public:
    Simulation();
    Simulation(const Simulation& orig);
    virtual ~Simulation();
    
    const Body_sphere& getOneSingleSphere(); 
    double getRad();
    void setOneSingleSphereRadius(double r); 
    void draw(); 
    
    bool addBody(Body_object body); 
    bool removeBody(int bodyID); 
    
    void addCube(); 
    
private:
    Body_sphere oneSingleSphere; 
    vector<Body_object> Bodies;  

};

#endif	/* SIMULATION_H */

