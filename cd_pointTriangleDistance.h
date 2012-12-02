/* 
 * File:   cd_pointTriangleDistance.h
 * Author: carser
 *
 * Created on December 1, 2012, 4:53 PM
 * 
 * 
 * 
 */

#ifndef CD_POINTTRIANGLEDISTANCE_H
#define	CD_POINTTRIANGLEDISTANCE_H

#include <armadillo>

using namespace arma; 

// 
// Assumes p1 has length 3
void pointTriangleDistance(mat TRI, vec P, double &dist, double PP0[]) {
    mat::fixed<1,3>B = TRI.row(0);   
    mat E0 = TRI.row(1)-B; 
    mat E1 = TRI.row(2)-B;
    
    mat D = B - P.t();
    double a = dot(E0,E0);
    double b = dot(E0,E1);
    double c = dot(E1,E1);
    double d = dot(E0,D);
    double e = dot(E1,D);
    double f = dot(D,D);

    double det = a*c - b*b; // do we have to use abs here?
    double s   = b*e - c*d;
    double t   = b*d - a*e;

    double sqrDistance;
    double invDet; 
    double tmp0, tmp1, numer, denom; 
    // Terible tree of conditionals
    if ( (s+t) <= det ) {
      if ( s<0 ) {
        if ( t<0 ) {
          // region4
          if (d < 0) {
            t = 0;
            if (-d >= a) {
              s = 1;
              sqrDistance = a + 2*d + f;
            }
            else {
              s = -d/a;
              sqrDistance = d*s + f;
            }
          }
          else {
            s = 0;
            if (e >= 0) {
              t = 0;
              sqrDistance = f;
            }
            else {
              if (-e >= c) {
                t = 1;
                sqrDistance = c + 2*e + f;
              }
              else {
                t = -e/c;
                sqrDistance = e*t + f;
              }
            }
          }  // of region 4
        }
        else {
          // region 3
          s = 0;
          if ( e >= 0 ) {
            t = 0;
            sqrDistance = f;
          }
          else {
            if (-e >= c) {
              t = 1;
              sqrDistance = c + 2*e +f;
            }
            else {
              t = -e/c;
              sqrDistance = e*t + f;
            }
          }
        }  // of region 3 
      }
      else {
        if ( t < 0 ) {
          // region 5
          t = 0;
          if ( d >= 0 ) {
            s = 0;
            sqrDistance = f;
          }
          else {
            if ( -d >= a ) {
              s = 1;
              sqrDistance = a + 2*d + f; 
            }
            else {
              s = -d/a;
              sqrDistance = d*s + f;
            }
          }
        }
        else {
          // region 0
          invDet = 1/det;
          s = s*invDet;
          t = t*invDet;
          sqrDistance = s*(a*s + b*t + 2*d) + t*(b*s + c*t + 2*e) + f;
        }
      }
    }
    else {
      if ( s < 0 ) {
        // region 2
        tmp0 = b + d;
        tmp1 = c + e;
        if ( tmp1 > tmp0 ) { // minimum on edge s+t=1 ) {
          numer = tmp1 - tmp0;
          denom = a - 2*b + c;
          if ( numer >= denom ) {
            s = 1;
            t = 0;
            sqrDistance = a + 2*d + f; 
          }
          else {
            s = numer/denom;
            t = 1-s;
            sqrDistance = s*(a*s + b*t + 2*d) + t*(b*s + c*t + 2*e) + f;
          }
        }
        else {          // minimum on edge s=0
          s = 0;
          if ( tmp1 <= 0 ) {
            t = 1;
            sqrDistance = c + 2*e + f;
          }
          else {
            if ( e >= 0 ) {
              t = 0;
              sqrDistance = f;
            }
            else {
              t = -e/c;
              sqrDistance = e*t + f;
            }
          }
        }
      }
    
       // of region 2
      else {
        if ( t < 0 ) {
          // region6 
          tmp0 = b + e;
          tmp1 = a + d;
          if (tmp1 > tmp0) {
            numer = tmp1 - tmp0;
            denom = a-2*b+c;
            if (numer >= denom) {
              t = 1;
              s = 0;
              sqrDistance = c + 2*e + f;
            }
            else {
              t = numer/denom;
              s = 1 - t;
              sqrDistance = s*(a*s + b*t + 2*d) + t*(b*s + c*t + 2*e) + f;
            }
          }
          else {
            t = 0;
            if (tmp1 <= 0) {
                s = 1;
                sqrDistance = a + 2*d + f;
            }
            else {
              if (d >= 0) {
                  s = 0;
                  sqrDistance = f;
              }
              else {
                  s = -d/a;
                  sqrDistance = d*s + f;
              }
            }
          }
        }
          // end region 6
        else {
          // region 1
          numer = c + e - b - d;
          if ( numer <= 0 ) {
            s = 0;
            t = 1;
            sqrDistance = c + 2*e + f;
          }
          else {
            denom = a - 2*b + c;
            if ( numer >= denom ) {
              s = 1;
              t = 0;
              sqrDistance = a + 2*d + f;
            }
            else {
              s = numer/denom;
              t = 1-s;
              sqrDistance = s*(a*s + b*t + 2*d) + t*(b*s + c*t + 2*e) + f;
            }
          } //of region 1
        }
      }
    }

    // account for numerical round-off error
    if (sqrDistance < 0)
      sqrDistance = 0;

    dist = sqrt(sqrDistance);
    mat RESULT = B + s*E0 + t*E1; 
    PP0[0] = RESULT[0];
    PP0[1] = RESULT[1];
    PP0[2] = RESULT[2]; 

}

#endif	/* CD_POINTTRIANGLEDISTANCE_H */

