/* 
 * File:   Body_polymesh.h
 * Author: carser
 *
 * Created on September 29, 2012, 3:18 PM
 */

#include "Body_object.h"

#ifndef BODY_POLYMESH_H
#define	BODY_POLYMESH_H

class Body_polymesh : public Body_object {
public:
    Body_polymesh();
    Body_polymesh(const Body_polymesh& orig);
    virtual ~Body_polymesh();
    
    void draw(); 
    void setVertices(double v[]); 
    
private:
    int num_verts;
    int num_edges; 
    int num_faces;
    double verts[];  // 3*n vector of vertices: [x0 y0 z0 x1 y1 z1 ... xn yn zn]
    
    
    

};

#endif	/* BODY_POLYMESH_H */

