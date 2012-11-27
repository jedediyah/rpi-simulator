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
#include "SimulatorDefenitions.h"

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
    vec T; 
    if ( abs(dot(n,Z)) < 0.7 )
        T = cross(n,Z);
    else
        T = cross(n,Y); 
    return T / norm(T,2); 
}

void CollisionDetection::findCollisions(Contact *Contacts, int &num_bodies, int &num_contacts, 
                int &num_subcontacts, Body_sphere *spheres, int &num_spheres, 
                                      Body_trimesh *meshes,   int &num_meshes) {
    //cout << "Collision detection on " << num_spheres << " spheres..." << endl; 
    
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
    for (int b=0; b<num_meshes; b++) {      // Trimeshes
        meshes[b].BodyIndex = -1;
        meshes[b].ContactCount = 0;
    }
    
    
    ////////////////////////////////////////////////////////////////
    // SPHERE-GROUND collision detection
    for (int s = 0; s < num_spheres; s++) {
        if (!spheres[s].isStaticBody()) {
            mat psi = zeros(1);
            psi.at(0) = spheres[s].u().at(2) - spheres[s].radius(); // psi=height-radius
            if (psi[0] > 0.5)
                continue; 
            mat n = zeros(3, 1);
            n.at(2) = 1.0; // Normal to sphere is always up (+z direction)
            
            mat t = arbitraryTangent(n);
            vec r1 = n;    // Won't be used... since ground is static.
            mat r2 = -n * spheres[s].radius(); 
            Contact c = Contact(cID++,GROUND,SPHERE, -3, s, n, t, r1, r2, psi);   // -3 is our special int for GROUND
            Contacts[num_contacts++] = c;

            spheres[s].ContactCount++; // Already made sure not static 
            if (spheres[s].BodyIndex < 0) {
                spheres[s].BodyIndex = bID++;
                num_bodies++;
            }
        }
    }
    
    
    ////////////////////////////////////////////////////////////////
    // MESH-GROUND collision detection
    for (int b=0; b<num_meshes; b++) {
        if (!meshes[b].isStaticBody()) {
            for (int v=0; v<meshes[b].num_verts(); v++) {  // For every vertex 
                
                mat psi = zeros(1);
                psi[0] = meshes[b].world_verts()[3*v+2];  // psi=z value
                if (psi[0] > 0.5)  // TODO: this is a hardcoded epsilon 
                    continue; 
                mat n = zeros(3, 1);
                n.at(2) = 1.0; // Normal to sphere is always up (+z direction)

                mat t = arbitraryTangent(n);
                vec r1 = n;    // Won't be used... since ground is static.
                vec p2 = zeros(3,1);
                p2[0] = meshes[b].world_verts()[3*v+0];
                p2[1] = meshes[b].world_verts()[3*v+1];
                p2[2] = meshes[b].world_verts()[3*v+2];
                mat r2 = p2 - meshes[b].u(); 
                Contact c = Contact(cID++,GROUND,TRIMESH, -3, b, n, t, r1, r2, psi);   // -3 is arbitrary
                Contacts[num_contacts++] = c;

                meshes[b].ContactCount++; // Already made sure not static 
                if (meshes[b].BodyIndex < 0) {
                    meshes[b].BodyIndex = bID++;
                    num_bodies++;
                }
                
            }
        }
    }
    
    ////////////////////////////////////////////////////////////////
    // SPHERE-SPHERE collision detection
    for (int s1=0; s1<num_spheres; s1++) {
        for (int s2=s1+1; s2<num_spheres; s2++) {
            if ( spheres[s1].isStaticBody() && spheres[s2].isStaticBody() )
                continue; 
            
            // Check bounding sphere
            mat n = spheres[s2].u()-spheres[s1].u();
            double n_norm = norm(n,2);
            mat psi = zeros(1);  // 1x1 matrix
            psi(0) = n_norm -spheres[s2].radius() -spheres[s1].radius(); 
            //if ( psi.at(0) -spheres[s1].radius() <   // Bounding spheres should include the radii...
            //        spheres[s1].bounding_radius()+spheres[s2].bounding_radius() ) {
                n = n/n_norm; // Normalize normal
                mat t = arbitraryTangent(n); 
                vec r1 =  n*spheres[s1].radius();
                mat r2 = -n*spheres[s2].radius();
                Contact c = Contact(cID++,SPHERE,SPHERE,s1,s2,n,t,r1,r2,psi);
                Contacts[num_contacts++] = c;
                if ( !spheres[s1].isStaticBody() ) {
                    spheres[s1].ContactCount++;  
                    if (spheres[s1].BodyIndex < 0) {
                        spheres[s1].BodyIndex = bID++;
                        num_bodies++;
                    }
                }
                if ( !spheres[s2].isStaticBody() ) {
                    spheres[s2].ContactCount++;  
                    if (spheres[s2].BodyIndex < 0) {
                        spheres[s2].BodyIndex = bID++;
                        num_bodies++;
                    }
                }
            //}
        }
    }
    
    
    ////////////////////////////////////////////////////////////////
    // MESH-SPHERE collision detection
    
    
   
    
    
}


