#include "painting/painter.h"
#include "shape/c4shape.h"

int C4Shape::sNextId = 1;

//-----------------------------------------------------------------------------
C4Shape::C4Shape()
{
    mPos[0]   = mPos[1]   = mPos[2]   = 0.0;
    mRot[0]   = mRot[1]   = mRot[2]   = 0.0;
    mScale[0] = mScale[1] = mScale[2] = 1.0;
    mId = sNextId++;
}

//-----------------------------------------------------------------------------
C4Shape::~C4Shape()
{
}

//-----------------------------------------------------------------------------
int C4Shape::id() const
{
    return mId;
}

//-----------------------------------------------------------------------------
const float *C4Shape::pos() const
{
    return mPos;
}

//-----------------------------------------------------------------------------
void C4Shape::scale(float dx, float dy, float dz)
{
    mScale[0] = dx;
    mScale[1] = dy;
    mScale[2] = dz;
}

//-----------------------------------------------------------------------------
void C4Shape::rotate(float rx, float ry, float rz)
{
    mRot[0] = rx;
    mRot[1] = ry;
    mRot[2] = rz;
}

//-----------------------------------------------------------------------------
void C4Shape::setPos(float x, float y, float z)
{
    mPos[0] = x;
    mPos[1] = y;
    mPos[2] = z;
}

//-----------------------------------------------------------------------------
void C4Shape::render(C4Painter *painter, int frame)
{
    painter->push();
    if (mScale[0] != 0 || mScale[1] != 0 || mScale[2] != 0)
        painter->scale(mScale);
    if (mRot[0] != 0 || mRot[1] != 0 || mRot[2] != 0)
        painter->rotate(mRot);
    if (mPos[0] != 0 || mPos[1] != 0 || mPos[2] != 0)
        painter->translate(mPos);

    shapeRender(painter,frame);

    painter->pop();
}

//-----------------------------------------------------------------------------
void C4Shape::rotate(float *r3)
{
    rotate(r3[0], r3[1], r3[2]);
}

//-----------------------------------------------------------------------------
void C4Shape::setPos(float *p3)
{
    setPos(p3[0], p3[1], p3[2]);
}

//-----------------------------------------------------------------------------
void C4Shape::scale(float *d3)
{
    scale(d3[0],d3[1], d3[2]);
}
