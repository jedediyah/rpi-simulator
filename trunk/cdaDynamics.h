/* 
 * File:   cdaDynamics.h
 * Author: carser
 *
 * Created on November 11, 2012, 10:57 PM
 */

#ifndef CDADYNAMICS_H
#define	CDADYNAMICS_H

#include "lcp_nsqp.h"
#include "Contact.h"
#include <armadillo> 

using namespace arma;

void cdaDynamics(Contact *Contacts, Body_sphere *spheres, Body_trimesh *trimeshes,
                 int &nb, int &nc, int &ns) {
    
    /////////////////////////
    // LCP TEST
    int nn = 7; 
    double A[49] = {     
        5.0000,    0.0000,    0.0000,   -0.0000,   -0.0000,    0.0000,        0.,
        0.0000,   17.5000,    5.4078,  -14.1578,  -14.1578,    5.4078,    1.0000,
        0.0000,    5.4078,   17.5000,    5.4078,  -14.1578,   -14.157,    1.0000,
       -0.0000,  -14.1578,    5.4078,   17.5000,    5.4078,  -14.1578,    1.0000,
       -0.0000,  -14.1578,  -14.1578,    5.4078,   17.5000,    5.4078,    1.0000,
        0.0000,    5.4078,  -14.1578,  -14.1578,    5.4078,   17.5000,    1.0000,
        0.2000,   -1.0000,   -1.0000,   -1.0000,   -1.0000,   -1.0000,         0. };
    double q[7] = {    -0.6434,
                        1.1521,
                        0.3560,
                       -0.9320,
                       -0.9320,
                        0.3560,
                             0 }; 
    double z0[7];  
    double w[7];
    int info;
    int iparamLCP;
    double dparamLCP[1] = {10e-5}; 
    
    lcp_nsqp(&nn,A,q,z0,w,&info,&iparamLCP,dparamLCP); 
    
    
    int nd = 0;  // Number of directions in discretized friction cone
    
    /////////////////////////////////////
    // Initialize submatrices 
    mat M = zeros(6*nb);
    mat Gn = zeros(6*nb,nc);
    mat GaT = zeros(ns-nc,6*nb);  // Note that this is GaT, not Ga
    mat U = eye(nc,nc);
    mat E = zeros(nd*nc,nc);
    mat E1 = zeros(nc, ns-nc);
    mat E2 = zeros(ns-nc, ns-nc);
    vec b; 
    if (nd > 0)
        b = zeros(6 * nb + (2 + nd) * nc + (ns - nc), 1);
    else
        b = zeros(6 * nb + nc + (ns - nc), 1);
    
    /////////////////////////////////////
    // Formulate dynamics 
    // M - mass matrix
//    for (int s=0; s<num_spheres; s++) {
//        
//    }
    
    int scID = 0;
    int idx = 1;
    
    
    
    
    
//    /////////////////////////////////////
//    // Prepare data for LCP solver 
//    int nn = 3;
//    double vect[9] = {1., 0., 0., 4., 5., 0., 0., 3., 2.};
//    double qq[3] = {0., 0., 1.};
//    double z[3];
//    double w[3]; 
//    int info; 
//    int iparamLCP;
//    double dparamLCP[1] = {10e-3};
//    
//    lcp_nsqp(&nn,vect,qq,z,w,&info,&iparamLCP,dparamLCP); 
    
}



#endif	/* CDADYNAMICS_H */

