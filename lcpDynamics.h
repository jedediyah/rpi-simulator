/* 
 * File:   lcpDynamics.h
 * Author: carser
 *
 * Created on November 12, 2012, 11:17 AM
 */

#ifndef LCPDYNAMICS_H
#define	LCPDYNAMICS_H

#include "Contact.h"
#include "Body_object.h"
#include <cmath>

// functions
vec lcpDynamics(Contact *Contacts, Body_sphere *spheres, int &num_spheres, 
                 Body_trimesh *trimeshes, int &num_trimeshes, int &nb, int &nc); 
mat rot(vec k, double theta); 
mat hat(vec k); 


/////////////////////////////////////
// lcpDynamics 
vec lcpDynamics(Contact *Contacts, Body_sphere *spheres, int &num_spheres, 
                 Body_trimesh *trimeshes, int &num_trimeshes, 
                 int &nb, int &nc, double &h) {
    
    int nd = 3;  // Number of friction directions in discretized friction cone
    
    /////////////////////////////////////
    // Initialize submatrices 
    mat M = zeros(6*nb, 6*nb); 
    mat Gn = zeros(6*nb,nc);
    mat Gf = zeros(6*nb,nd*nc); 
    mat U = eye(nc,nc);
    mat E = zeros(nd*nc, nc);
    mat NU = zeros(6*nb,1);  // Velocity, including angular
    mat FX = zeros(6*nb, 1); // Extern IMPULSE (not force)
    mat PSI = zeros(nc, 1);
    
    
    /////////////////////////////////////
    // Formulate dynamics 
    
    // M - mass matrix
    // SPHERES
    int bid;  
    for (int s=0; s<num_spheres; s++) {
        if ( spheres[s].ContactCount > 0 ) { // If sphere has active contacts
            bid = spheres[s].bodyIndex(); 
            M.submat(span(6*bid-6,6*bid-1),span(6*bid-6,6*bid-1)) = spheres[s].mass_inertia_matrix();
        }
    }
    // TODO: trimeshes
    
    // E
    if (nd > 0)
        for (int i=1; i<=nc; i++) {
            E.submat(span( nd*i-nd, nd*i-1 ),span(i-1,i-1)) = ones(nd,1);
        }
    
    // Gn, U, and b
    int cID, body1id, body2id;  
    mat r1;
    mat Gn_i1, Gf_i1;  
    mat d;
    mat r2, Gn_i2, Gf_i2;  
    for (int i=0; i<nc; i++) {
        
        if (Contacts[i].body1 == -3) { // GROUND VERSION
            cID = Contacts[i].contact_ID;  
            r2 = Contacts[i].r2; 
            body2id = spheres[Contacts[i].body2].bodyIndex(); 

            // Gn
            Gn_i2 = join_cols(Contacts[i].normal, cross(r2, Contacts[i].normal));
            if ( !spheres[Contacts[i].body2].isStaticBody() )
                Gn.submat( span(6*body2id-6,6*body2id-1),span(cID,cID) ) = Gn_i2; 

            // Gf
            if (nd > 0) {
                Gf_i2 = zeros(6,nd); 
                for (double j=0; j < nd; j++) {    // For each friction direction
                    
//                    mat R = rot(Contacts[i].normal, ((j)/(double)nd)*(2.*M_PI));
//                    cout << "R:" << endl; R.print(); 
//                    cout << "a:" << ((j)/(double)nd)*(2.*M_PI) << endl; 
                    d = rot(Contacts[i].normal, ((j)/(double)nd)*(2.*M_PI)) * Contacts[i].tangent; 
//                    
//                    cout << "t:" << endl; Contacts[i].tangent.print(); 
//                    cout << "d:" << endl; d.print();
                    Gf_i2.submat( span(0,5), span(j,j) ) = join_cols( d , cross(r2,d));
                }
//                Gf_i2.print();
                if ( !spheres[Contacts[i].body2].isStaticBody() )
                    Gf.submat( span(6*body2id-6,6*body2id-1), span( nd*(cID+1)-nd, nd*(cID+1)-1 ) ) = Gf_i2; 
//                Gf.print();
            }

            // With the MCP, this is where we would fill in a portion of b.  
            // The following will make redundant assignments to NU & FX, but it's 
            // for the best right now!  (Since we need PSI, and we're already looping)
            if ( !spheres[Contacts[i].body2].isStaticBody() ) {
                NU.submat( span(6*body2id-6,6*body2id-1), span(0,0) ) = spheres[Contacts[i].body2].nu(); 
                FX.submat( span(6*body2id-6,6*body2id-1), span(0,0) ) = spheres[Contacts[i].body2].fext();
            }

            // U
            U.at(cID,cID) = 0.5 * (0.5 * spheres[Contacts[i].body2].mu());
            PSI.at(cID) = Contacts[i].psi.at(0);
        }
        
        else {
            cID = Contacts[i].contact_ID;  
            r1 = Contacts[i].r1;
            r2 = Contacts[i].r2; 
            body1id = spheres[Contacts[i].body1].bodyIndex();
            body2id = spheres[Contacts[i].body2].bodyIndex(); 

            // Gn
            Gn_i1 = join_cols(-Contacts[i].normal, cross(r1,-Contacts[i].normal));
            Gn_i2 = join_cols(Contacts[i].normal, cross(r2, Contacts[i].normal));

            if ( !spheres[Contacts[i].body1].isStaticBody() )
                Gn.submat( span(6*body1id-6,6*body1id-1),span(cID,cID) ) = Gn_i1; 
            if ( !spheres[Contacts[i].body2].isStaticBody() )
                Gn.submat( span(6*body2id-6,6*body2id-1),span(cID,cID) ) = Gn_i2; 

            // Gf
            if (nd > 0) {
                Gf_i1 = zeros(6,nd);
                Gf_i2 = zeros(6,nd); 
                for (double j=0; j < nd; j++) {    // For each friction direction
                    d = rot(Contacts[i].normal, ((j)/(double)nd)*(2.*M_PI)) * Contacts[i].tangent;  
                    Gf_i1.submat( span(0,5), span(j,j) ) = join_cols( d , cross(r1,d));
                    Gf_i2.submat( span(0,5), span(j,j) ) = join_cols( d , cross(r2,d));
                }
//                Gf_i1.print();
//                Gf_i2.print();
                if ( !spheres[Contacts[i].body1].isStaticBody() )
                    Gf.submat( span(6*body1id-6,6*body1id-1), span( nd*(cID+1)-nd, nd*(cID+1)-1 ) ) = Gf_i1;  // Need to double check these indices 
                if ( !spheres[Contacts[i].body2].isStaticBody() )
                    Gf.submat( span(6*body2id-6,6*body2id-1), span( nd*(cID+1)-nd, nd*(cID+1)-1 ) ) = Gf_i2; 
//                Gf.print();
            }

            // With the MCP, this is where we would fill in a portion of b.  
            // The following will make redundant assignments to NU & FX, but it's 
            // for the best right now!  (Since we need PSI, and we're already looping)
            if ( !spheres[Contacts[i].body1].isStaticBody() ) {
                NU.submat( span(6*body1id-6,6*body1id-1), span(0,0) ) = spheres[Contacts[i].body1].nu(); 
                FX.submat( span(6*body1id-6,6*body1id-1), span(0,0) ) = spheres[Contacts[i].body1].fext(); 
            }

            if ( !spheres[Contacts[i].body2].isStaticBody() ) {
                NU.submat( span(6*body2id-6,6*body2id-1), span(0,0) ) = spheres[Contacts[i].body2].nu(); 
                FX.submat( span(6*body2id-6,6*body2id-1), span(0,0) ) = spheres[Contacts[i].body2].fext();
            }

            // U
                 U.at(cID,cID) = 0.5 * (spheres[Contacts[i].body1].mu() * spheres[Contacts[i].body2].mu());
            PSI.at(cID) = Contacts[i].psi.at(0);
        }
        
        
        
    } // Done with submatrices
    
    // Construct A, b, and solve LCP 
    mat Minv = inv(M);  
    mat MinvGn = Minv*Gn; //solve(M,Gn); 
    mat MinvGf = Minv*Gf; //solve(M,Gf);  
    mat MinvPext = Minv*FX*h; //solve(M, FX*h);  
    
    mat A1 = join_rows( join_rows( Gn.t()*MinvGn, Gn.t()*MinvGf ), zeros(nc,nc) );
    mat A2 = join_rows( join_rows( Gf.t()*MinvGn, Gf.t()*MinvGf ), E ); 
    mat A3 = join_rows( join_rows( U, -E.t() ), zeros(nc,nc) ); 
    mat A = join_cols( join_cols(A1,A2), A3 );           
    
    vec b = join_cols( join_cols(Gn.t()*(NU+MinvPext)+PSI/h, 
                       Gf.t()*(NU + MinvPext) ), 
                       zeros(nc,1) );    
//    cout << "MinvGn" << endl; MinvGn.print();
//    cout << "MinvGf" << endl; MinvGf.print(); 
//    cout << "MinvPext" << endl; MinvPext.print(); 
    
    
    // Prepare data for LCP call
    int nn = A.n_rows; 
    double AA[A.n_elem]; 
    for (int e=0; e<A.n_elem; e++) 
        AA[e] = A.at(e);
    
    double bb[b.n_elem];
    for (int e=0; e<b.n_elem; e++) 
        bb[e] = b.at(e); 
    
    double z0[nn];  
    double w[nn];
    int info;
    int iparamLCP;
    double dparamLCP[1] = {10e-5}; 
    
    lcp_nsqp(&nn,AA,bb,z0,w,&info,&iparamLCP,dparamLCP);        // Solve LCP 
    switch(info) {
        case 0:
            cout << "LCP: Converged" << endl; 
            break;
        case 1: 
            cout << "LCP: Too many iterations" << endl;
            break;
        case 2: 
            cout << "LCP: Accuracy insufficient to satisfy convergence criterion" << endl;
            break;
        case 5: 
            cout << "LCP: Length of working array insufficient" << endl;
            break;
        default:
            cout << "LCP: The constraints are inconsistent" << endl; 
            
    }
// *                0 : convergence  / minimization sucessfull\n
// *                1 : Too Many iterations\n
// * 	            2 : Accuracy insuficient to satisfy convergence criterion\n
// *                5 : Length of working array insufficient\n
// *                Other : The constraints are inconstent\n
    
    // Compute the result, to pass back to simulation
    vec Pn = zeros(nc);
    vec Pf = zeros(nd*nc);
    for (int i=0; i<nc; i++)
        Pn.at(i) = z0[i]; 
    for (int i=nc; i<nc+nd*nc; i++)
        Pf.at(i) = z0[i]; 
    
    vec RESULT =  NU + MinvGn*Pn + MinvGf*Pf + MinvPext;  // RESULT
    
    Gf.print();
    RESULT.print();

    return RESULT; 
    
}

/////////////////////////////////////
// rot
// Returns a 3x3 rotation matrix representing rotation of theta about vec k.  
mat rot(vec k, double theta) {
    // Assume k is already normalized
    // k = k/norm(k,2);  
    
    // From Peter Corke, replace "hat(k)*hat(k)" with "(k*k'-eye(3))"
    return eye(3,3)+sin(theta)*hat(k)+(1-cos(theta))*(k*k.t()-eye(3,3));
}

/////////////////////////////////////
// hat
// Returns the skew-symmetric matrix form of vector k
mat hat(vec k) {
    mat khat = zeros(3,3); 
    khat.at(0,1) = -k.at(2);
    khat.at(0,2) =  k.at(1);
    khat.at(1,0) =  k.at(2); 
    khat.at(2,2) = -k.at(0);
    khat.at(2,0) = -k.at(1);
    khat.at(2,1) =  k.at(0); 
    return khat;  
}


#endif	/* LCPDYNAMICS_H */

