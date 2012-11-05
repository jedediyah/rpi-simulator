/* 
 * File:   Body_sphere.cpp
 * Author: carser
 * 
 * Created on September 28, 2012, 7:53 PM
 */


#include "Body_sphere.h"
#include "Body_object.h"
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif


Body_sphere::Body_sphere() {
    Body_object::set_body_type("sphere");
    Radius = 1.0; 
}

Body_sphere::Body_sphere(const Body_sphere& orig) {
}

Body_sphere::~Body_sphere() {
}

// Get methods
double Body_sphere::radius() { return Radius; }

// Set methods
void Body_sphere::setRadius(double r) { Radius = r; }

void Body_sphere::draw() { 

    glPushMatrix();	
        glTranslated(u().at(0), u().at(1), u().at(2));
        glutSolidSphere(Radius, 20, 20);   // Sphere.
    glPopMatrix();  
}



