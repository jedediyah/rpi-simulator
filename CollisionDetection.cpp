/* 
 * File:   CollisionDetection.cpp
 * Author: carser
 * 
 * Created on November 10, 2012, 11:52 PM
 */

#include "CollisionDetection.h"
#include <iostream>
#include <armadillo>
#include <cmath>

using namespace std;
using namespace arma; 

CollisionDetection::CollisionDetection() {
}

CollisionDetection::CollisionDetection(const CollisionDetection& orig) {
}

CollisionDetection::~CollisionDetection() {
}

// Assumes that n is normalized 
mat arbitraryTangent(mat n) {
    vec Y = zeros(3,1);  Y(1) = 1;
    vec Z = zeros(3,1);  Z(2) = 1;
    if ( abs(dot(n,Y)) < 0.7 )
        return cross(n,Z);
    return Y; 
}

void CollisionDetection::findCollisions(Contact *Contacts, int &num_bodies, int &num_contacts, 
                int &num_subcontacts, Body_sphere *spheres, int &num_spheres) {
    cout << "Collision detection on " << num_spheres << " spheres..." << endl; 
    
    num_bodies = 0;
    num_contacts = 0;    // 
    num_subcontacts = 0; // 
    int cID = 0;         // The contact ID 
    int bID = 1;         // The body ID, for indexing  
    
    // Init body ids
    for (int s=0; s<num_spheres; s++ ) {  // spheres
        spheres[s].BodyIndex = -1; 
        spheres[s].ContactCount = 0;
    }
    
    ////////////////////////////////////////////////////////////////
    // Sphere-Sphere collision detection
    for (int s1=0; s1<num_spheres; s1++) {
        for (int s2=s1+1; s2<num_spheres; s2++) {
            // Check bounding sphere
            mat n = spheres[s2].u()-spheres[s1].u();
            double n_norm = norm(n,2);
            mat psi = zeros(1);  // 1x1 matrix
            psi(0) = n_norm -spheres[s2].radius() -spheres[s1].radius(); 
            //if ( psi -spheres[s1].radius() <   // Bounding spheres should include the radii...
            //        spheres[s1].bounding_radius()+spheres[s2].bounding_radius() ) {
                n = n/n_norm; // Normalize normal
                mat t = arbitraryTangent(n); 
                vec r1 =  n*spheres[s1].radius();
                mat r2 = -n*spheres[s2].radius();
                Contact c = Contact(cID++,s1,s2,n,t,r1,r2,psi);
                Contacts[num_contacts++] = c;
                if (spheres[s1].BodyIndex < 0 && ~spheres[s1].isStaticBody()) {
                    spheres[s1].BodyIndex = bID++;
                    spheres[s1].ContactCount++;
                    num_bodies++;
                }
                if (spheres[s2].BodyIndex < 0 && ~spheres[s2].isStaticBody()) {
                    spheres[s2].BodyIndex = bID++;
                    spheres[s2].ContactCount++;
                    num_bodies++;
                }
            //}
        }
    }
    
}


