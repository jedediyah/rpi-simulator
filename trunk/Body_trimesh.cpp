/* 
 * File:   Body_Trimesh.cpp
 * Author: carser
 * 
 * Created on September 29, 2012, 8:25 PM
 */

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <GL/gl.h>
#include "Body_trimesh.h"
#include "Body_object.h"
#include <armadillo>

using namespace std; 
using namespace arma; 

string processString(string s); 
double strGetN(string s, int n);
vec::fixed<3> vert0;
vec::fixed<3> vert1; 
vec::fixed<3> edge0;
vec::fixed<3> edge1; 
vec::fixed<3> face_norm; 

Body_trimesh::Body_trimesh() {
}

// Create a Body_trimesh object from a polygon file e.g. cube.poly
// Assumes the file is formated as:
//      # of verts
//      List of verts
//      # of faces
//      List of faces           (See cube.poly for example)
Body_trimesh::Body_trimesh(string filename) {
    bool VertsZeroIndexed = false; 
    
    string line; 
    ifstream infile(filename.c_str());
    if (infile.is_open()) {
        // Get number of Vertices
        while (infile.good()) {
            getline (infile, line);
            line = processString(line); 
            if (line.length() > 0) {
                Num_Verts = atoi(line.c_str()); // Set Num_Verts
                Local_Verts = new double[3*Num_Verts];  // Allocate Vertex Space
                World_Verts = new double[3*Num_Verts]; 
                Verts = new Body_Vertex[Num_Verts]; 
                break;
            } 
        }
        // Read in all vertices
        int vert_counter = 0;
        while (infile.good() && Num_Verts > vert_counter) {
            getline (infile, line);
            line = processString(line); 
            if (line.length() > 0) {
                if (vert_counter == 0) 
                    if (strGetN(line,0)==0)
                        VertsZeroIndexed = true;
                
                //Verts[vert_counter] = new Body_Vertex(); 
                Local_Verts[3*vert_counter+0] = strGetN(line,1); 
                Local_Verts[3*vert_counter+1] = strGetN(line,2);
                Local_Verts[3*vert_counter+2] = strGetN(line,3);
            }
            vert_counter++; 
        }
        if (vert_counter != Num_Verts) 
            cout << "WARNING: Number of vertices did not match!!!" << endl; 
        
        // Get number of Faces
        while (infile.good()) {
            getline (infile, line);
            line = processString(line); 
            if (line.length() > 0) {
                Num_Faces = atoi(line.c_str()); // Set Num_Faces
                Faces = new Body_Face[Num_Faces]; 
                break;
            } 
        }
        // Read in faces
        int face_counter = 0;
        while (infile.good() && Num_Faces > face_counter) {
            getline (infile, line);
            line = processString(line); 
            int temp_verts[3];
            if (line.length() > 0) { 
                if (VertsZeroIndexed) {
                    temp_verts[0] = (int)strGetN(line,0);  // Assumes not zero indexed
                    temp_verts[1] = (int)strGetN(line,1);
                    temp_verts[2] = (int)strGetN(line,2);
                }
                else {
                    temp_verts[0] = (int)strGetN(line,0)-1;  // Assumes not zero indexed
                    temp_verts[1] = (int)strGetN(line,1)-1;
                    temp_verts[2] = (int)strGetN(line,2)-1;
                }
                Faces[face_counter].setVerts(temp_verts, 3);
                // Add face to vertex list
                for (int v=0; v<Num_Verts; v++) {
                    if (v==temp_verts[0] || v==temp_verts[1] || v==temp_verts[2])
                        Verts[v].add_FaceIndex(face_counter); 
                }
                face_counter++; 
            }
        }
    }
    
    // Generate all edges
    // Complete populating Verts, Edges, and Faces ...
    this->updateWorld_Verts(); 
    
//    // Make sure world coordinates are initialized 
//    for (int v=0; v<3*Num_Verts; v++)
//        World_Verts[v] = Local_Verts[v]; 
//    // Copy edges into trianagleIndecies
//    triangleIndecies = new unsigned char [3*Num_Faces];
//    for (int f=0; f<Num_Faces; f++) {
//        triangleIndecies[3*f+0] = Faces[f].vert_indicies()[0];
//        triangleIndecies[3*f+1] = Faces[f].vert_indicies()[1];
//        triangleIndecies[3*f+2] = Faces[f].vert_indicies()[2];
//    }
    
    Body_object::set_body_type("trimesh"); 
}

// Remove comments from string
string processString(string s) {
    char c; 
    int n=-1;
    while(++n < s.length()) { 
        c = s.at(n);
        if (c >= 48 && c <= 57)
            continue;
        if (c!=43 && c!=45 && c!=46 && c!=32 && c!=9) // + - . [tab]
            break;
    }
    return s.substr(0,n); 
}

bool validChar(char c) {
    if (c>=48 && c<=57) return true; // 0 through 9
    if (c==43 || c==45 || c==46) return true; // + - . 
    return false; 
}

// Returns a double of the nth number in the space-separated string
double strGetN(string s, int n) {
    s += "  ";  // Pad the string for the sf test below
    char c;
    int si=0, sf=1;
    for(;!validChar(s.at(si));si++); // Skip leading spaces
    while (n>0 && si < s.length()) {    // Find si
        for(;!validChar(s.at(si));si++); // Skip spaces
        for(;validChar(s.at(si));si++);  // Increment si
            n--; 
    }
    for(;!validChar(s.at(si));si++); // Skip spaces
    sf = si+1; 
    for(;validChar(s.at(sf));sf++);  // Increment sf

//    cout << "String: " << "'" << s << "'" << endl;
//    cout << "n: " << n << endl;
//    cout << "si,sf = " << si << ", " << sf << endl;
//    cout << "Returning: " << "'" << s.substr(si,sf-si).c_str() << "'" << endl;
    return atof(s.substr(si,sf-si).c_str());
}

Body_trimesh::Body_trimesh(const Body_trimesh& orig) {
}

Body_trimesh::~Body_trimesh() {
}

void Body_trimesh::setNumVerts(int nv) { Num_Verts = nv; Verts = new Body_Vertex[nv]; }
void Body_trimesh::setNumEdges(int ne) { Num_Verts = ne; Edges = new Body_Edge[ne]; }
void Body_trimesh::setNumFaces(int nf) { Num_Verts = nf; Faces = new Body_Face[nf]; }

void Body_trimesh::setVertexAt(int index, double x, double y, double z) {
    if (index >= Num_Verts) {
        cout << "ERROR - Body_trimesh::setVertexAt - Attempting to add " <<
                "vertex to uninitialized array.\n";
        return; 
    }
    Local_Verts[index*3+0] = x;
    Local_Verts[index*3+1] = y;
    Local_Verts[index*3+2] = z;
}

void Body_trimesh::scale(double s) {
    for (int v=0; v<Num_Verts; v++) {
        Local_Verts[3*v+0] = Local_Verts[3*v+0] * s;
        Local_Verts[3*v+1] = Local_Verts[3*v+1] * s;
        Local_Verts[3*v+2] = Local_Verts[3*v+2] * s;
    }
    this->updateWorld_Verts();
    // TODO: should also update inertia
}

void Body_trimesh::updateWorld_Verts() {
    mat R = zeros<mat>(3,3); 
    vec q = Body_object::quat(); 
    R(0,0) = q(0)*q(0) + q(1)*q(1) - q(2)*q(2) - q(3)*q(3);
    R(0,1) = 2*(q(1)*q(2) + q(0)*q(3));
    R(0,2) = 2*(q(1)*q(3) - q(0)*q(2));
    R(1,0) = 2*(q(1)*q(2) - q(0)*q(3));
    R(1,1) = q(0)*q(0) - q(1)*q(1) + q(2)*q(2) - q(3)*q(3);
    R(1,2) = 2*(q(2)*q(3) + q(0)*q(1));
    R(2,0) = 2*(q(1)*q(3) + q(0)*q(2));
    R(2,1) = 2*(q(2)*q(3) - q(0)*q(1));
    R(2,2) = q(0)*q(0) - q(1)*q(1) - q(2)*q(2) + q(3)*q(3);
    
    Body_object::setR(R); // Save the rotation matrix
    
    vec coord = vec(3); 
    for (int v=0; v<Num_Verts; v++) {
        coord(0)=Local_Verts[3*v+0];
        coord(1)=Local_Verts[3*v+1];
        coord(2)=Local_Verts[3*v+2];
        coord = R*coord;                  // Rotate local coord to world frame
        coord = Body_object::u() + coord; // Translate to world frame
        World_Verts[3*v+0] = coord(0);
        World_Verts[3*v+1] = coord(1);
        World_Verts[3*v+2] = coord(2);
    }

}

int Body_trimesh::num_verts() { return Num_Verts; }
int Body_trimesh::num_edges() { return Num_Edges; }
int Body_trimesh::num_faces() { return Num_Faces; }

void Body_trimesh::setVertices(double v[], int num_v) {
    Num_Verts = num_v; 
    Local_Verts = new double[3*num_v];     // Allocate memory for vertices
    for (int i=0; i<3*num_v; i++) {
        Local_Verts[i] = v[i];             // Populate vertices
    }
    cout << "Adding " << Num_Verts << " vertices." << endl;  
}

void Body_trimesh::printVertices() {
    cout << "-------------" << endl; 
    for (int i=0; i<Num_Verts; i++) {
        cout << World_Verts[3*i+0] << ", ";
        cout << World_Verts[3*i+1] << ", ";
        cout << World_Verts[3*i+2];  
        cout << endl;
    }
}

// Return a pointer to the vertices of the trimesh 
double* Body_trimesh::local_verts() { return Local_Verts; }
double* Body_trimesh::world_verts() { return World_Verts; }


void Body_trimesh::initializeGL(){
    // Make sure world coordinates are initialized 
    //this->updateWorld_Verts(); 

    // Copy edges into trianagleIndecies
    triangleIndecies = new unsigned char [3*Num_Faces];
    for (int f=0; f<Num_Faces; f++) {
        triangleIndecies[3*f+0] = Faces[f].vert_indicies()[0];  // I think I confused faces with verts!!!
        triangleIndecies[3*f+1] = Faces[f].vert_indicies()[1];
        triangleIndecies[3*f+2] = Faces[f].vert_indicies()[2];
    }
    // Point to world coordinates 
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_DOUBLE, 0, World_Verts); 
}

void Body_trimesh::draw() {
    if ( !this->Visible ) return;  
    
    // TODO: Take care of color
    
    // Material property vectors.
//    float matAmbAndDif[] = {0.7, 0.1, 0.1, 1.0};
//    float matSpec[] = {1.0, 1.0, 1.0, 1.0};
//    float matShine[] = {10.0};
//
//    // Material properties of the mesh.
//    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
//    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    
    //cout << Body_object::name() << endl; 
    //glDrawElements(GL_TRIANGLES, 3*Num_Faces, GL_UNSIGNED_BYTE, triangleIndecies);
    
    //printVertices();
    
    
    //this->updateWorld_Verts();
    
//               glBegin(GL_POLYGON);
//                glVertex3f(-5.0,-5.0,0.0);
//                glVertex3f(15.0,-5.0,0.0);
//                glVertex3f(15.0,15.0,-5.0);
//                glVertex3f(-5.0,15.0,0.0);
//               glEnd();
 
    // Material property vectors.
    float matAmbAndDif1[] = {Color[0], Color[1], Color[2], 1.0};
    float matSpec[] = {1.0, 1.0, 1.0, 1.0};
    float matShine[] = {50.0};
    // Material properties 
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matAmbAndDif1);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShine);
    
    // Don't need to transform since we have the world coordinates.
    // Draw each face
    int vdex;
    for (int f=0; f<Num_Faces; f++) {
        // Face normal
        vdex = Faces[f].vert_indicies()[0]; 
        vert0[0] = World_Verts[3*vdex+0];
        vert0[1] = World_Verts[3*vdex+1];
        vert0[2] = World_Verts[3*vdex+2];
        
        vdex = Faces[f].vert_indicies()[1];
        vert1[0] = World_Verts[3*vdex+0];
        vert1[1] = World_Verts[3*vdex+1];
        vert1[2] = World_Verts[3*vdex+2];
        edge0 = vert1-vert0;
        
        vdex = Faces[f].vert_indicies()[2];
        vert0[0] = World_Verts[3*vdex+0];
        vert0[1] = World_Verts[3*vdex+1];
        vert0[2] = World_Verts[3*vdex+2];
        edge1 = vert0-vert1;
        face_norm = cross(edge0, edge1);
       glBegin(GL_POLYGON);
          glNormal3f(face_norm[0],face_norm[1],face_norm[2]);  // Normal for lighting 
          for (int v=0; v<Faces[f].num_verts(); v++) {
            vdex = Faces[f].vert_indicies()[v]; 
            double vx = World_Verts[ 3*vdex+0 ];
            double vy = World_Verts[ 3*vdex+1 ];
            double vz = World_Verts[ 3*vdex+2 ]; // Should ALWAYS be a tri-face
            //cout << "Face [" << f << "]: "<<vx<<", "<<vy<<", "<<vz << endl;
            glVertex3f( vx,vy,vz );
          }
       glEnd();
    }
    
}

void Body_trimesh::printAllData() {
    cout << "  VERTICES:" << endl;
    for (int v=0; v < Num_Verts; v++) {
        cout << Local_Verts[3*v+0] << ", ";
        cout << Local_Verts[3*v+1] << ", ";
        cout << Local_Verts[3*v+2] << endl;
    }
    
    cout << " WORLD VERTS: " << endl;
    for (int v=0; v < Num_Verts; v++) {
        cout << World_Verts[3*v+0] << ", ";
        cout << World_Verts[3*v+1] << ", ";
        cout << World_Verts[3*v+2] << endl;
    }
    
    cout << "  FACE INDICIES of VERTICES: " << endl;
    for (int v=0; v < Num_Verts; v++) {
        cout << "v" << v << endl;
        for (int f=0; f<Verts[v].num_faces(); f++) {
            cout << "f_" << Verts[v].face_indicies()[f] << ", "; 
        }
        cout << endl; 
    }
    
    cout << "  FACES:" << endl;
    int *face_verts; 
    for (int f=0; f < Num_Faces; f++) {
        face_verts = Faces[f].vert_indicies(); 
        cout << "  F" << f << ": " << endl; 
        for (int v=0; v<Faces[f].num_verts(); v++) {
            cout << face_verts[v] << ", "; 
        }
        cout << endl;
    }
    
    cout << " triVerts: " << endl;
    for (int f=0; f<Num_Faces; f++) {
        cout << "tF_" << f << ": " << endl;
        for (int v=0; v<Faces[f].num_verts(); v++) {
            cout << triangleIndecies[v] << ", "; 
        }
    }
}


