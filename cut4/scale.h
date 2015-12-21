#ifndef SCALE_H
#define SCALE_H

#include "transformation.h"

class Scale : public Transformation
{
public:
    Scale(float *f);
    Scale(float fx, float fy, float fz);

    virtual void transform(float *p) const;

private:
    float mFx;
    float mFy;
    float mFz;
};

#endif // SCALE_H
