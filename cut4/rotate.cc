#include "rotate.h"
#include "math_turn.h"

Rotate::Rotate(float *r)
    : mAx(r[0]), mAy(r[1]), mAz(r[2])
{
}

Rotate::Rotate(float ax, float ay, float az)
    : mAx(ax), mAy(ay), mAz(az)
{
}

void Rotate::transform(float *p1) const
{
    float n[3];
    float p2[3];

    if (mAx) {
        n[0] = 1;
        n[1] = 0;
        n[2] = 0;
        math::turn(p1,n,mAx,p2);
        p1[0] = p2[0];
        p1[1] = p2[1];
        p1[2] = p2[2];

    }
    if (mAy) {
        n[0] = 0;
        n[1] = 1;
        n[2] = 0;
        math::turn(p1,n,mAy,p2);
        p1[0] = p2[0];
        p1[1] = p2[1];
        p1[2] = p2[2];
    }
    if (mAz) {
        n[0] = 0;
        n[1] = 0;
        n[2] = 1;
        math::turn(p1,n,mAz,p2);
        p1[0] = p2[0];
        p1[1] = p2[1];
        p1[2] = p2[2];
    }
}
