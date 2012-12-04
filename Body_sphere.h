/* 
 * File:   Body_sphere.h
 * Author: Jedediyah Williams
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
    void updateMassInertia();
    
    // Graphics methods
    void drawSphere(); 
    void initializeGL(); 
    void draw(bool wireframe); // OpenGL draw method
    
protected:
    // Sphere properties distinct from generic Body_object
    double Radius;  

};

#endif	/* BODY_SPHERE_H */

