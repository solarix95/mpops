#include <math.h>
#include "math_turn.h"


#define X   p[0]
#define Y   p[1]
#define Z   p[2]

#define n1  n[0]
#define n2  n[1]
#define n3  n[2]

namespace math {

    void turn(const float *p, float *n, float a, float *r)
/*

        / n1*n1*(1-cos(a)) +    cos(a)     n1*n2*(1-cos(a)) - n3*sin(a)     n1*n3*(1-cos(a)) + n2*sin(a) \
        | n2*n1*(1-cos(a)) + n3*sin(a)     n2*n2*(1-cos(a)) +    cos(a)     n2*n3*(1-cos(a)) - n1*sin(a) |
        \ n3*n1*(1-cos(a)) - n2*sin(a)     n3*n2*(1-cos(a)) + n1*sin(a)     n3*n3*(1-cos(a)) +    cos(a) /

*/
    {
        a = a*0.01745; // grad -> rad
        float cosa = cos(a);
        float sina = sin(a);
        r[0] = X*(n1*n1*(1-cosa) +    cosa)  +   Y*(n1*n2*(1-cosa) - n3*sina)   +  Z*(n1*n3*(1-cosa) + n2*sina);
        r[1] = X*(n2*n1*(1-cosa) + n3*sina)  +   Y*(n2*n2*(1-cosa) +    cosa)   +  Z*(n2*n3*(1-cosa) - n1*sina);
        r[2] = X*(n3*n1*(1-cosa) - n2*sina)  +   Y*(n3*n2*(1-cosa) + n1*sina)   +  Z*(n3*n3*(1-cosa) +    cosa);
    }
}

