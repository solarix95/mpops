#ifndef STEREOCAMERA_H
#define STEREOCAMERA_H

#include "camera.h"

class Stereocamera : public Camera
{
public:
    Stereocamera(float x, float y, float z, float eyeSep);

    virtual void move(float *p);

protected:
    float mEyeSeparation;
};

#endif // STEREOCAMERA_H
