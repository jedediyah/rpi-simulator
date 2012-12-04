/* 
 * File:   Body_Edge.h
 * Author: Jedediyah Williams
 *
 * Created on September 29, 2012, 8:58 PM
 */

#ifndef BODY_EDGE_H
#define	BODY_EDGE_H

class Body_Edge {
public:
    Body_Edge();
    Body_Edge(const Body_Edge& orig);
    virtual ~Body_Edge();

    int num_verts(); 
    int num_faces();
    int* vert_indecies();
    int* face_indecies();
    
private:
    int Num_Verts;      // Number of verts edge is in
    int Num_Faces;      // Number of faces edge is in
    int *Vert_Indecies; // int array of vert indecies edge is in
    int *Face_Indecies; // int array of face indecies edge is in

};

#endif	/* BODY_EDGE_H */

