/* 
 * File:   SimulationEnvironment.h
 * Author: carser
 *
 * Created on November 10, 2012, 4:48 PM
 */

#ifndef SIMULATIONENVIRONMENT_H
#define	SIMULATIONENVIRONMENT_H

#include "Simulation.h"

class SimulationEnvironment {
public:
    SimulationEnvironment(int argc, char **argv);
    SimulationEnvironment(const SimulationEnvironment& orig);
    virtual ~SimulationEnvironment();
    
    int start();
    
    void test(unsigned char key);
    
private:
    //Simulation simulator; 

};

#endif	/* SIMULATIONENVIRONMENT_H */

