/* 
 * File:   Body_object.cpp
 * Author: Jedediyah Williams
 * 
 * Created on September 28, 2012, 7:37 PM
 */

#include "Body_object.h"
#include <armadillo>
#include <cmath>
#include <iostream>

using namespace std; 
using namespace arma; 

Body_object::Body_object() {
    Name = "Body_Object";
    Mass = 1.0;
    Mass_Inertia_Matrix = eye<mat>(6,6); 
    U = zeros<vec>(3); 
    Quat = zeros<vec>(4); Quat(0) = 1.0; 
    Nu = zeros<vec>(6);
    Fext = zeros<vec>(6); 
    Aext = zeros<vec>(6); 
    Mu = 0.5; 
    IsStaticBody = 0; 
    Visible = true; 
}

Body_object::Body_object(const Body_object& orig) {
}

Body_object::~Body_object() {
}

// Get methods
string Body_object::name()               { return Name; }
double Body_object::mass()               { return Mass; }
mat Body_object::mass_inertia_matrix()   { return Mass_Inertia_Matrix; }
vec Body_object::u()                     { return U; }
vec Body_object::quat()                  { return Quat; }
vec Body_object::rot()                   { return R; }
vec Body_object::nu()                    { return Nu; }
vec Body_object::fext()                  { return Fext; }
vec Body_object::aext()                  { return Aext; }
double Body_object::mu()                 {return Mu; }
bool Body_object::isStaticBody()         { return IsStaticBody; }
bool Body_object::visible()              { return Visible; }

// Set methods
void Body_object::setName(string name) { Name = name; }
void Body_object::setMass(double mass) {
    Mass = mass;
    // TODO: update inertia tensor 
}
void Body_object::setPosition(double x, double y, double z) {
    U(0) = x;  U(1) = y;  U(2) = z; 
}
void Body_object::setQuaternion(double a, double i, double j, double k) {
    Quat(0)=a;  Quat(1)=i;  Quat(2)=j;  Quat(3)=k;
}
void Body_object::setR(mat r) { R = r; }
void Body_object::setVelocity(double x, double y, double z) {
    Nu(0)=x;  Nu(1)=y;  Nu(2)=z; 
}
void Body_object::setVelocity(double x, double y, double z, double wx, double wy, double wz) {
    Nu(0)=x; Nu(1)=y; Nu(2)=z; Nu(3)=wx; Nu(4)=wy; Nu(5)=wz;
}
void Body_object::setFext(double Fx, double Fy, double Fz, double Tx, double Ty, double Tz) {
    Fext(0)=Fx; Fext(1)=Fy; Fext(2)=Fz; Fext(3)=Tx; Fext(4)=Ty; Fext(5)=Tz; 
    Aext = Fext/Mass; 
}
void Body_object::setAext(double Ax, double Ay, double Az, double ax, double ay, double az) {
    Aext(0)=Ax; Aext(1)=Ay; Aext(2)=Az; Aext(3)=ax; Aext(4)=ay; Aext(5)=az;
    Fext = Mass*Aext; 
}
void Body_object::setMu(double mu) { Mu = mu; }
void Body_object::setStatic(bool isStatic) { IsStaticBody = isStatic; }
void Body_object::setVisible(bool visible) { Visible = visible; }

// Uses current velocities to update position vector U and quaternion Quat. 
void Body_object::stepDynamics(double dt) {
    // Update position
    U = U + Nu.subvec(0,2) * dt; 
    
    // Update rotation 
    vec w = Nu.subvec(3,5) * dt;
    double wn = norm(w,2);              // Normalized
    if (wn != 0) {
        // Quat multiply current Quat
        vec q = Quat; 
        vec r = vec(4);
        r(0) = -cos(wn/2);
        r(1) = (w(0)/wn)*sin(wn/2);
        r(2) = (w(1)/wn)*sin(wn/2);
        r(3) = (w(2)/wn)*sin(wn/2);
        
        vec v = vec(4);
        v(0) = q(0)*r(0) - q(1)*r(1) - q(2)*r(2) - q(3)*r(3); 
        v(1) = q(0)*r(1) + r(0)*q(1) + q(2)*r(3) - q(3)*r(2);
        v(2) = q(0)*r(2) + r(0)*q(2) + q(3)*r(1) - q(1)*r(3);
        v(3) = q(0)*r(3) + r(0)*q(3) + q(1)*r(2) - q(2)*r(1); 
        
        Quat = v/norm(v,2); // Remember to normalize. 
    }
}

// Updates Nu only, based on current external accelerations. 
void Body_object::applyAext(double dt) {
    Nu = Nu*dt + 0.5 * Aext * dt*dt;  
}

void Body_object::print() {
    cout << "------------------------\n"; 
    cout << ":: Body: " << Name << endl;
    cout << ":: Mass: " << Mass << endl; 
    cout << ":: U: [" << U(0) << ", " << U(1) << ", " << U(2) << "]" << endl;
    cout << ":: Quat: [" << Quat(0) << ", " << Quat(1)<<", "<<Quat(2)<<", "<<Quat(3)<<"]"<<endl;
    cout << ":: Nu: [" << Nu(0)<<", "<<Nu(1)<<", "<<Nu(2)<<", "<<Nu(3)<<", "<<Nu(4)<<", "<<Nu(5)<<"]"<<endl;
    cout << ":: Fext: [" << Fext(0)<<", "<<Fext(1)<<", "<<Fext(2)<<", "<<Fext(3)<<", "<<Fext(4)<<", "<<Fext(5)<<"]" << endl;
    cout << ":: Aext: [" << Aext(0)<<", "<<Aext(1)<<", "<<Aext(2)<<", "<<Aext(3)<<", "<<Aext(4)<<", "<<Aext(5)<<"]" << endl;
    cout << ":: Mu: " << Mu << endl; 
    cout << ":: IsStatic: " << IsStaticBody << endl;
    cout << ":: Visible: " << Visible << endl; 
}

