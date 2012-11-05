/* 
 * File:   Body_Edge.cpp
 * Author: carser
 * 
 * Created on September 29, 2012, 8:58 PM
 */

#include "Body_Edge.h"

Body_Edge::Body_Edge() {
}

Body_Edge::Body_Edge(const Body_Edge& orig) {
}

Body_Edge::~Body_Edge() {
}

int Body_Edge::num_verts() { return Num_Verts; } 
int Body_Edge::num_faces() { return Num_Faces; }
int* Body_Edge::vert_indecies() { return Vert_Indecies; }
int* Body_Edge::face_indecies() { return Face_Indecies; }


