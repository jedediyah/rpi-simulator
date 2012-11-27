/* 
 * File:   Body_Trimesh.h
 * Author: carser
 *
 * Created on September 29, 2012, 8:25 PM
 */

#include <iostream>
#include "Body_object.h"
#include "Body_Vertex.h"
#include "Body_Edge.h"
#include "Body_Face.h"

#ifndef BODY_TRIMESH_H
#define	BODY_TRIMESH_H

using namespace std; 

class Body_trimesh : public Body_object {
public:
    Body_trimesh();
    Body_trimesh(string filename);
    Body_trimesh(const Body_trimesh& orig);
    virtual ~Body_trimesh();
    
    // For initializing a triangle mesh
    void setNumVerts(int nv);
    void setNumEdges(int ne);
    void setNumFaces(int nf);
    
    void setVertexAt(int index, double x, double y, double z);
    void scale(double s); 
    void updateWorld_Verts(); 
    
    // Gets
    int num_verts();
    int num_edges();
    int num_faces();
    
    // Graphics methods
    void initializeGL(); 
    void draw(); // OpenGL draw method
    
    
    void setVertices(double v[], int num_v); 
    void printVertices(); 
    double* local_verts();  
    double* world_verts();
    
    void printAllData(); 
    
protected:
    int Num_Verts;
    int Num_Edges; 
    int Num_Faces;
    
    Body_Vertex *Verts; 
    Body_Edge *Edges;
    Body_Face *Faces; 

    double *Local_Verts; // n*3 vector of local coordinates
    double *World_Verts; // n*3 vector of world coordinates 
    unsigned char *triangleIndecies; // List of triangle faces for OpenGL rendering 

};

#endif	/* BODY_TRIMESH_H */

