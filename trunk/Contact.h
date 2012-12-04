/* 
 * File:   Contact.h
 * Author: Jedediyah Williams
 *
 * Created on November 10, 2012, 11:08 PM
 */

#ifndef CONTACT_H
#define	CONTACT_H

#include <armadillo>

using namespace arma;

class Contact {
public:
    Contact(); 
    Contact(int contact_ID,
            int body1_type,
            int body2_type,
            int body1,
            int body2,
            mat normal,
            mat tangent,
            vec r1,
            mat r2,
            mat psi,
            char *collision_type);
    Contact(int contact_ID,
            int body1_type,
            int body2_type,
            int body1,
            int body2,
            mat normal,
            mat tangent,
            vec r1,
            mat r2,
            mat psi);
    
    Contact(const Contact& orig);
    virtual ~Contact();
    
    void print();
    
    // Public variables
    int contact_ID;
    int body1;   // 
    int body2;
    int body1_type;  // Sphere, mesh, etc
    int body2_type; 
    mat normal;  // From body 1 to body 2
    mat tangent;
    vec r1;   // [x;y;z] Vector from body_1 COM to contact point1 in WORLD frame 
    mat r2;      // For CDA, r2 (analogous to r1) may be 3xN 
    vec psi;     // Signed gap distance
    char *collision_type; 
    
private:
    
    

};

#endif	/* CONTACT_H */

