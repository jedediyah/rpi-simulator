/* 
 * File:   Body_Face.cpp
 * Author: carser
 * 
 * Created on September 29, 2012, 8:59 PM
 */

#include "Body_Face.h"

Body_Face::Body_Face() {
}

Body_Face::Body_Face(int v[], int num_v) {
    Num_Verts = num_v;
    Vert_Indicies = new int[num_v];
    for (int i=0; i<num_v; i++)
        Vert_Indicies[i] = v[i]; 
}

Body_Face::Body_Face(const Body_Face& orig) {
}

Body_Face::~Body_Face() {
}

int Body_Face::num_verts() { return Num_Verts; } 
int Body_Face::num_edges() { return Num_Edges; }
int* Body_Face::verts() { return Vert_Indicies; }
int* Body_Face::edges() { return Edge_Indicies; }

void Body_Face::setVerts(int v[], int num_v) {
    Num_Verts = num_v;
    Vert_Indicies = new int[num_v];
    for (int i=0; i<num_v; i++)
        Vert_Indicies[i] = v[i]; 
}

