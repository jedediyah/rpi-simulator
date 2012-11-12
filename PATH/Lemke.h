/* 
 * File:   Lemke.h
 * Author: carser
 *
 * Created on November 11, 2012, 7:52 PM
 */

#ifndef LEMKE_H
#define	LEMKE_H

#include <armadillo> 

using namespace arma; 

// Given LCP represented by matrix M, vector q, and initial guess vector z0, 
// solves Mz+q >=0, z>=0, z'(Mz+q)=0.  
mat lemke(mat M, vec q, vec z0) {
    int n = q.size();  // q SHOULD be a vector.  size() returns total # of elements
    double zer_tol = 1e-5;  
    double piv_tol = 1e-8;
    int maxiter = min(10000,25*n);
    int err = 0;
    
    // Trivial solution exists
    bool allZero = true;
    for (int i=0; i<n; i++) {
        if (q.at(i) != 0.0) {
            allZero = false;
            break;
        }
    }
    if (allZero) 
        return zeros<mat>(n,1);  
    
    // Non-trivial solution...
    mat z = zeros(2*n,1); 
    mat j = zeros(n,1); 
    
    // Determine initial basis (assumes that z0 was included)
    vec bas = find(z0 > 0);
    
    for (int i=0; i<n; i++) {
        if ( z0.at(i) <= 0 )
            bas = join_cols(bas,n+i*ones(1)); 
    }
    mat B = join_rows( M, -eye(n,n) );
    B = B.cols(bas);
    
    // Determine initial values
    vec x = -(solve(B,q));
    
    // Check if initial basis provides solution
    allZero = true;
    for (int i=0; i<x.size(); i++) {
        if (x.at(i) != 0.0) {
            allZero = false;
            break;
        }
    }
    if (allZero) {
        z.cols(bas) = x;     // I don't know if this works...
        z = z(span(0,n-1));
        return;
    }
    
    double t = 2*n+1; 
    double entering = t;
    
    // Determine initial leaving variable
    uword lvindex;
    vec nx = -x;
    double tval = nx.max(lvindex); 
    double leaving = bas(lvindex);
    
    // Pivot in the artificial variable 
    bas.at(lvindex) = t; 
    x = x+tval;
    x(lvindex) = tval;
    B.cols(lvindex) = -B*ones(n,1); 
    
    // Loop variables
    mat Be; 
    vec d; 
    vec lvindex_vec;
    // Main iterations begin here
    for (int iter=1; iter < maxiter; iter++) {
        if (leaving == t) 
            break;
        else if (leaving <= n) {
            entering = n+leaving;
            //Be =  ............
        }
        else {
            entering = leaving-n;
            Be = M.cols(entering);  
        }
        d = solve(B,Be); 
        
        // Find new leaving variable
        j = find(d > piv_tol);  // Indicies of d>0
        if ( j.empty() ) {      // No new pivots - ray termination
            err = 2;
            break;
        }
        vec ratios = (x.at(j)+zer_tol)/d.at(j); 
        vec theta = ratios.min();   // Minimal ratios, d>0
        j = j( find((x.at(j)/d.at(j)) <= theta) );
        lvindex_vec = find(bas.at(j)==t); 
        if ( !lvindex_vec.empty() ) {
            lvindex = j.at(lvindex); 
        }
        else {
            theta = d.
        }
            
    }
    
}


#endif	/* LEMKE_H */

