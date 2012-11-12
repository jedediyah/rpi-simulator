/* 
 * File:   Body_Vertex.h
 * Author: Jedediyah Williams
 *
 * Created on September 29, 2012, 8:51 PM
 */

#ifndef BODY_VERTEX_H
#define	BODY_VERTEX_H

class Body_Vertex {
public:
    Body_Vertex();
    Body_Vertex(const Body_Vertex& orig);
    virtual ~Body_Vertex();
    
    int num_edges(); 
    int num_faces();
    int* edge_indicies();
    int* face_indicies();
    
    void add_FaceIndex(int new_f); 
    
private:
    int Num_Edges;      // Number of edges vertex is in
    int Num_Faces;      // Number of faces vertex is in
    int *Edge_Indicies; // int array of edge indecies vertex is in
    int *Face_Indicies; // int array of face indecies vertex is in

};

#endif	/* BODY_VERTEX_H */

