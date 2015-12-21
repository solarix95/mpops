#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "transformation.h"

class Translate : public Transformation
{
public:
    Translate(float *p);
    virtual void transform(float *p) const;

private:
    float mDx;
    float mDy;
    float mDz;
};

#endif // TRANSLATE_H
