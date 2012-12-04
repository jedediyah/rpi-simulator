/* 
 * File:   CollisionDetection.h
 * Author: Jedediyah Williams
 *
 * Created on November 10, 2012, 11:52 PM
 */

#ifndef COLLISIONDETECTION_H
#define	COLLISIONDETECTION_H

#include "Body_sphere.h"
#include "Body_trimesh.h"
#include "Contact.h"
#include <vector>

class CollisionDetection {
public:
    CollisionDetection();
    CollisionDetection(const CollisionDetection& orig);
    virtual ~CollisionDetection();
    
    void findCollisions(Contact *Contacts, int &num_bodies, int &num_contacts, 
                int &num_subcontacts, Body_sphere *spheres, int &num_spheres,
                                      Body_trimesh *meshes,   int &num_meshes);
    
private:
    

};

#endif	/* COLLISIONDETECTION_H */

