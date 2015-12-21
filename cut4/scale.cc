#include "scale.h"

Scale::Scale(float *f)
    : mFx(f[0]), mFy(f[1]), mFz(f[2])
{
}

Scale::Scale(float fx, float fy, float fz)
    : mFx(fx), mFy(fy), mFz(fz)
{
}

void Scale::transform(float *p) const
{
    p[0] *= mFx;
    p[1] *= mFy;
    p[2] *= mFz;
}

