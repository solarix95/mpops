#include "translate.h"

Translate::Translate(float *p)
    : mDx(p[0]), mDy(p[1]), mDz(p[2])
{
}

void Translate::transform(float *p) const
{
    p[0] += mDx;
    p[1] += mDy;
    p[2] += mDz;
}
