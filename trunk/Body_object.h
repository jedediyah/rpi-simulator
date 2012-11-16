/* 
 * File:   Body_object.h
 * Author: carser
 *
 * Created on September 28, 2012, 7:37 PM
 */

#include <armadillo>

#ifndef BODY_OBJECT_H
#define	BODY_OBJECT_H

using namespace std;
using namespace arma;

class Body_object {
public:
    Body_object();
    Body_object(const Body_object& orig);
    virtual ~Body_object();
    
    // Get methods
    string name();              // Name of body
    void set_body_type(string bt); 
    string body_type();         // Type of body, e.g. trimesh, sphere, cylinder, etc.
    double mass ();             // Mass of body
    mat mass_inertia_matrix (); // diag( 3x3 mass matrix, 3x3 inertia tensor )
    vec u ();                   // Position [x y z]
    vec quat ();                // Quaternion representation of rotation
    vec rot ();
    vec nu ();                  // Velocity as [Vx Vy Vz Wx Wy Wz]
    vec fext ();                // External forces including torques [Fx Fy Fz Tx Ty Tz]
    vec aext (); 
    double mu ();               // Friction "factor"
    bool isStaticBody ();       // Static or dynamics body
    bool visible();             // Whether to draw or not 
    double bounding_radius();

    // Set methods
    void setName(string Name);  // Set name
    void setMass(double mass);
    void setPosition(double x, double y, double z);
    void setQuaternion(double a, double i, double j, double k);
    void setR(mat r); 
    void setVelocity(double x, double y, double z);
    void setVelocity(double x, double y, double z, double wx, double wy, double wz);
    void setFext(double Fx, double Fy, double Fz, double Tx, double Ty, double Tz);
    void setAext(double Ax, double Ay, double Az, double ax, double ay, double az);
    void setMu(double mu); 
    void setStatic(bool s); 
    void setVisible(bool visible);      // Set visibility 
    void setBoundingRadius(double r);
    
    // Dynamics 
    void stepDynamics(double dt);
    void applyImpulse(vec &impulse, double dt); // Apply impulse over time dt
    void applyAext(double dt); 
    
    // Auxiliary methods
    void print();
    
    int bodyIndex(); 
    int contactCount();
    
    int BodyIndex; 
    int ContactCount;
    
protected:
    // Dynamics variables 
    string Name; // Name of object
    string Body_Type; 
    double Mass;
    mat::fixed<3,3> Mass_Matrix;          // 3x3 mass matrix = mass * eye(3)
    mat::fixed<3,3> Mass_Inverse_Matrix;  // 3x3 inverse of mass matrix used for LCP formulation
    mat::fixed<3,3> Inertia_Matrix;       // 3x3 inertia tensor 
    mat::fixed<6,6> Mass_Inertia_Matrix;  // 6x6 mass-inertia matrix
    vec::fixed<3>   U;      // Position [x y z]
    vec::fixed<4>   Quat;   // Quaternion [a i j k]
    mat::fixed<3,3> R;      // Rotation matrix
    vec::fixed<6>   Nu;     // Velocity as [Vx Vy Vz Wx Wy Wz]
    vec::fixed<6>   Fext;   // External force, including torques [Fx Fy Fz Tx Ty Tz]
    vec::fixed<6>   Aext;   // External acceleration [Ax Ay Az ax ay az]  
    double Mu;  // Friction "factor"
    bool IsStaticBody; // Dynamic or static body
    double Bounding_Radius; 
    
    // Graphics variables
    bool Visible;  

};

#endif	/* BODY_OBJECT_H */

