#include <math.h>
#include "math_aux.h"

namespace math {

    float abs3(const float *p1, const float *p2)
    {
        return sqrt((p1[0]-p2[0])*(p1[0]-p2[0]) + (p1[1]-p2[1])*(p1[1]-p2[1]) + (p1[2]-p2[2])*(p1[2]-p2[2]));
    }
}
