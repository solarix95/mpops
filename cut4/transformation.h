#ifndef TRANSFORM_H
#define TRANSFORM_H

class Transformation
{
public:
    virtual void transform(float *p) const = 0;

protected:
    Transformation();
};

#endif // TRANSFORM_H
