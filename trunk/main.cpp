/* 
 * File:   main.cpp
 * Author: carser
 *
 * Created on November 10, 2012, 4:34 PM
 */

#include <cstdlib>
#include "SimulationEnvironment.h"

// Main routine.
int main(int argc, char **argv) 
{
    SimulationEnvironment simEnvironment( argc, argv );
    
    return simEnvironment.start(); 
}

