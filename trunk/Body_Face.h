/* 
 * File:   Body_Face.h
 * Author: carser
 *
 * Created on September 29, 2012, 8:59 PM
 */

#ifndef BODY_FACE_H
#define	BODY_FACE_H

class Body_Face {
public:
    Body_Face();
    Body_Face(int v[], int num_v);
    Body_Face(const Body_Face& orig);
    virtual ~Body_Face();

    int num_verts(); 
    int num_edges();
    int* verts();
    int* edges();
    
    void setVerts(int v[], int num_v); 
    
private:
    int Num_Verts;      // Number of edges face is in
    int Num_Edges;      // Number of faces face is in
    int *Vert_Indicies; // int array of vert indecies face is in
    int *Edge_Indicies; // int array of edge indecies face is in

};

#endif	/* BODY_FACE_H */

