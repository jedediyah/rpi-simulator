/* 
 * File:   Body_Vertex.cpp
 * Author: Jedediyah Williams
 * 
 * Created on September 29, 2012, 8:51 PM
 */

#include <stdlib.h>
#include "Body_Vertex.h"
#include "Body_Face.h"

Body_Vertex::Body_Vertex() {
    Num_Edges = 0;
    Num_Faces = 0; 
}

Body_Vertex::Body_Vertex(const Body_Vertex& orig) {
}

Body_Vertex::~Body_Vertex() {
}

int Body_Vertex::num_edges() { return Num_Edges; } 
int Body_Vertex::num_faces() { return Num_Faces; }
int* Body_Vertex::edge_indicies() { return Edge_Indicies; }
int* Body_Vertex::face_indicies() { return Face_Indicies; }

void Body_Vertex::add_FaceIndex(int new_f) {
    Num_Faces++;
    int New_Faces[Num_Faces]; 
    
    for (int f=0; f<Num_Faces-1; f++)
        New_Faces[f] = Face_Indicies[f];
    New_Faces[Num_Faces-1] = new_f;
    
    //delete[] Face_Indicies;             
    Face_Indicies = new int[Num_Faces];
    
    for (int f=0; f<Num_Faces; f++)
        Face_Indicies[f] = New_Faces[f]; 
    
}
