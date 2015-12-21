#include "camera.h"
#include "math.h"

//-----------------------------------------------------------------------------
Camera::Camera(float x, float y, float z)
{
    mPos[0]   = x;
    mPos[1]   = y;
    mPos[2]   = z;

    mZeroDist = sqrt(x*x + y*y + z*z);
}
