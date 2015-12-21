#ifndef C4MEDIASHAPE_H
#define C4MEDIASHAPE_H

#include "shape/c4shape.h"

class C4Media;

class C4MediaShape : public C4Shape
{
public:
    C4MediaShape(C4Media *media);
    virtual ~C4MediaShape();

    virtual float radius() const;

protected:
    virtual void shapeRender(C4Painter *painter, int frame);

private:
    float pixelPerPoint(C4Painter *painter) const;
    C4Media  *mMedia;
};

#endif // C4MEDIASHAPE_H
