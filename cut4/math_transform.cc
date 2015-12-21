#include <math.h>
#include "math_transform.h"

#define X      (a[0]-c[0])
#define Y      (a[1]-c[1])
#define Z      (a[2]-c[2])

#define Cx     cos(o[0]*0.01745)
#define Cy     cos(o[1]*0.01745)
#define Cz     cos(o[2]*0.01745)

#define Sx     sin(o[0]*0.01745)
#define Sy     sin(o[1]*0.01745)
#define Sz     sin(o[2]*0.01745)


namespace math {


bool transform1(float *c, float *o, float *e, float *a, float *b)
{

    float dx = Cy * (Sz*Y + Cz*X) - Sy*Z;
    float dy = Sx * (Cy*Z + (Sz*Y + Cz*X))    + Cx*(Cz*Y - Sz*X);
    float dz = Cx * (Cy*Z + Sy*(Sz*Y + Cz*X)) - Sx*(Cz*Y  - Sz*X);

    b[0] = (e[2]/dz)*dx - e[0];
    b[1] = (e[2]/dz)*dy - e[1];
    return true; // dz > c[2];
}

bool transform2(float d, float *a, float *b) {
    /*
    float lambda = a[2]/(a[2]+d);
    b[0] = a[0]*lambda;
    b[1] = a[1]*lambda;
    */

    /*
    float lambda = 1 + a[2]/d;
    b[0] = a[0]/lambda;
    b[1] = a[1]/lambda;
    */
    b[0] = a[0]*d/a[2];
    b[1] = a[1]*d/a[2];

    return a[2] >=0;
}

}
