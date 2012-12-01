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
    Radius = 0.5; 
    updateMassInertia(); 
    Mu = 0.5; 
}

Body_sphere::Body_sphere(const Body_sphere& orig) {
}

Body_sphere::~Body_sphere() {
}

// Get methods
double Body_sphere::radius() { return Radius; }

// Set methods
void Body_sphere::setRadius(double r) { 
    Radius = r; 
    updateMassInertia(); 
}

void Body_sphere::updateMassInertia() {
    Mass_Matrix = eye(3,3)*Mass; 
    Mass_Inverse_Matrix = inv(Mass_Matrix); 
    Inertia_Matrix = (2./5.)*Mass*Radius*Radius*eye(3,3);  
    Mass_Inertia_Matrix = zeros(6,6);  
    Mass_Inertia_Matrix(span(0,2),span(0,2)) = Mass_Matrix;
    Mass_Inertia_Matrix(span(3,5),span(3,5)) = Inertia_Matrix;   
}

void Body_sphere::drawSphere() {
//    int i, j;
//    double lats = 10;
//    double longs = 10; 
//    for (i = 0; i <= lats; i++) {
//        double lat0 = M_PI * (-Radius/2 + (double) (i - 1) / lats);
//        double z0 = sin(lat0);
//        double zr0 = cos(lat0);
//
//        double lat1 = M_PI * (-Radius/2 + (double) i / lats);
//        double z1 = sin(lat1);
//        double zr1 = cos(lat1);
//
//        glBegin(GL_QUAD_STRIP);
//            for (j = 0; j <= longs; j++) {
//                double lng = 2 * M_PI * (double) (j - 1) / longs;
//                double x = cos(lng);
//                double y = sin(lng);
//
//                glNormal3f(x * zr0, y * zr0, z0);
//                glVertex3f(x * zr0, y * zr0, z0);
//                glNormal3f(x * zr1, y * zr1, z1);
//                glVertex3f(x * zr1, y * zr1, z1);
//            }
//        glEnd();
//    }
}

// OpenGL Initialize 
void Body_sphere::initializeGL(){
    
}

// Draw()
void Body_sphere::draw(bool wireframe) { 
    if ( !this->Visible ) return;  

    glPushMatrix();
    
        // Material property vectors.
        float matAmbAndDif1[] = {Color[0], Color[1], Color[2], 1.0};
        float matSpec[] = {1.0, 1.0, 1.0, 1.0};
        float matShine[] = {50.0};
        // Material properties 
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    
        glTranslatef(Body_object::u().at(0), // Translation
            Body_object::u().at(1),
            Body_object::u().at(2));
    
        // Convert quaternion to axis-angle for glRotatef() 
        double a = 2 * acos(Body_object::quat().at(0));
        double s = sqrt(1 - Body_object::quat().at(0) * Body_object::quat().at(0));
        double x, y, z;

        if (s < 0.001) { // Avoid dividing by zero
            x = Body_object::quat().at(0);
            y = Body_object::quat().at(1);
            z = Body_object::quat().at(2);
        } else {
            x = Body_object::quat().at(1) / sqrt(1 - Body_object::quat().at(0) * Body_object::quat().at(0));
            y = Body_object::quat().at(2) / sqrt(1 - Body_object::quat().at(0) * Body_object::quat().at(0));
            z = Body_object::quat().at(3) / sqrt(1 - Body_object::quat().at(0) * Body_object::quat().at(0));
        }

        //cout << Body_object::name() << ": " << a <<","<<x<<","<<y<<","<<z<< endl; 

        glRotatef(-a * 57.2958, x, y, z); // Rotation     

        if (wireframe)
            glutWireSphere(Radius, 10, 10);
        else
            glutSolidSphere(Radius, 15, 15);

    glPopMatrix();
        
//            double zMove = 0.0; 
//            glPushMatrix();
//               glTranslatef(2.0, 0.0, zMove); // Move the sphere.
//               //glutSolidSphere(3.5, 200, 200);
//               //glutSolidCube(3.0);
//               //glutSolidOctahedron();
//               
//               glBegin(GL_POLYGON);
//                glVertex3f(-5.0,-5.0,0.0);
//                glVertex3f(15.0,-5.0,0.0);
//                glVertex3f(15.0,15.0,-5.0);
//                glVertex3f(-5.0,15.0,0.0);
//               glEnd();
//               
//               //drawSphere();
//            glPopMatrix();

            //glPushMatrix();
  
                    //drawSphere();
            //glPopMatrix();

                                                               
    
//    // Draw sphere
//    glPushMatrix(); 
//        glTranslatef( Body_object::u().at(0),   // Translation
//                      Body_object::u().at(1),
//                      Body_object::u().at(2) );
//                   
//        
//        // Convert quaternion to axis-angle for glRotatef() 
//        double a = 2 * acos(Body_object::quat().at(0));
//        double s = sqrt(1-Body_object::quat().at(0)*Body_object::quat().at(0));
//        double x, y, z;
//        
//                                             
//        if (s < 0.001) {  // Avoid dividing by zero
//            x = Body_object::quat().at(0);
//            y = Body_object::quat().at(1);
//            z = Body_object::quat().at(2); 
//        }
//        else {
//            x = Body_object::quat().at(1) / sqrt(1-Body_object::quat().at(0)*Body_object::quat().at(0));
//            y = Body_object::quat().at(2) / sqrt(1-Body_object::quat().at(0)*Body_object::quat().at(0));
//            z = Body_object::quat().at(3) / sqrt(1-Body_object::quat().at(0)*Body_object::quat().at(0));
//        }
//        
//        glRotatef(a*57.2958,x,y,z);                     // Rotation 
//        
//        int i, j;
//        double lats = 20;
//        double longs = 20; 
//        for (i = 0; i <= lats; i++) {
//            double lat0 = M_PI * (-Radius/2 + (double) (i - 1) / lats);
//            double z0 = sin(lat0);
//            double zr0 = cos(lat0);
//
//            double lat1 = M_PI * (-Radius/2 + (double) i / lats);
//            double z1 = sin(lat1);
//            double zr1 = cos(lat1);
//
//            glBegin(GL_QUAD_STRIP);
//                for (j = 0; j <= longs; j++) {
//                    double lng = 2 * M_PI * (double) (j - 1) / longs;
//                    double x = cos(lng);
//                    double y = sin(lng);
//
//                    glNormal3f(x * zr0, y * zr0, z0);
//                    glVertex3f(x * zr0, y * zr0, z0);
//                    glNormal3f(x * zr1, y * zr1, z1);
//                    glVertex3f(x * zr1, y * zr1, z1);
//                }
//            glEnd();
//        }
//    glPopMatrix();
    
    
}



