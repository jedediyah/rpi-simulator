/* 
 * File:   Body_sphere.h
 * Author: carser
 *
 * Created on September 28, 2012, 7:53 PM
 */

#include "Body_object.h"

#ifndef BODY_SPHERE_H
#define	BODY_SPHERE_H

class Body_sphere : public Body_object {
public:
    Body_sphere();
    Body_sphere(const Body_sphere& orig);
    virtual ~Body_sphere();
    
    double radius();
    void setRadius(double r); 
    void draw(); 
    
private:
    // Sphere properties distanct from generic Body_object
    double Radius;  

};

#endif	/* BODY_SPHERE_H */

