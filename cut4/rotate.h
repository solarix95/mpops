#ifndef TRANSROTATE_H
#define TRANSROTATE_H

#include "transformation.h"

class Rotate : public Transformation
{
public:
    Rotate(float *r);
    Rotate(float ax, float ay, float az);

    virtual void transform(float *p) const;

private:
    float mAx;
    float mAy;
    float mAz;
};

#endif // TRANSROTATE_H
