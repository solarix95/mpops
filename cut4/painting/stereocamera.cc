#include "stereocamera.h"

//-----------------------------------------------------------------------------
Stereocamera::Stereocamera(float x, float y, float z, float eyeSep)
    : Camera(x,y,z), mEyeSeparation(eyeSep)
{
}

//-----------------------------------------------------------------------------
void Stereocamera::move(float *p)
{
    p[0] += mEyeSeparation;
    p[2] += mZeroDist;
}
