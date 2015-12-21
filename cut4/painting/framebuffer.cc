#include <QDebug>
#include <math.h>
#include "framebuffer.h"

#define REAL_X(virtualX)  ((virtualX - mFrustumLeft)/(mFrustumRight-mFrustumLeft)*(mSize.width()-1))
#define REAL_Y(virtualY)  ((mSize.height()-1) - (virtualY - mFrustumBottom)/(mFrustumTop-mFrustumBottom)*(mSize.height()-1))

//-----------------------------------------------------------------------------
C4FrameBuffer::C4FrameBuffer()
{
    mFrustumLeft    = -100;
    mFrustumRight   =  100;
    mFrustumBottom  = -100;
    mFrustumTop     =  100;
    mFrustumNear    =    1;
    mFrustumFar     = 1000;
}

//-----------------------------------------------------------------------------
void C4FrameBuffer::setSize(const QSize &size)
{
    mSize = size;
}

//-----------------------------------------------------------------------------
QSize C4FrameBuffer::size() const
{
    return mSize;
}

//-----------------------------------------------------------------------------
void C4FrameBuffer::setBackgroundColor(const QColor &c)
{
    mBackgroundColor = c;
}

//-----------------------------------------------------------------------------
void C4FrameBuffer::setFrustum(float left, float right, float bottom, float top, float nearVal, float farVal)
{
    Q_ASSERT(left < right);
    mFrustumLeft  = left;
    mFrustumRight = right;

    Q_ASSERT(bottom < top);
    mFrustumBottom = bottom;
    mFrustumTop    = top;

    Q_ASSERT(nearVal < farVal);
    mFrustumNear = nearVal;
    mFrustumFar  = farVal;

    qDebug() << left << right << bottom << top;
}

//-----------------------------------------------------------------------------
void C4FrameBuffer::setFrustum(float left, float right, float aspect, float nearVal, float farVal)
{
    setFrustum(left,right,-(right-left)/aspect/2, +(right-left)/aspect/2, nearVal,farVal);
}

//-----------------------------------------------------------------------------
void C4FrameBuffer::paintPixel(float x, float y, float z, const QColor &c)
{
    if (z < mFrustumNear || z > mFrustumFar)
        return;
    if (x < mFrustumLeft || x > mFrustumRight)
        return;
    if (y < mFrustumBottom || y > mFrustumTop)
        return;

    /*
       +-----+ top                +-----+ height-1
       |     |             ->     |     |
       +-----+ bottom             +-----+ 0
     left   right                 0   width-1
    */
    setPixel(REAL_X(x),
             REAL_Y(y),
             c);
}

//-----------------------------------------------------------------------------
float C4FrameBuffer::realDistance(const float *p1, const float *p2) const
{
    float pr1[] = { REAL_X(p1[0]), REAL_Y(p1[1]) };
    float pr2[] = { REAL_X(p2[0]), REAL_Y(p2[1]) };

    return sqrt( (pr1[0]-pr2[0])*(pr1[0]-pr2[0]) + (pr1[1]-pr2[1])*(pr1[1]-pr2[1])) + 0.5;
}

//-----------------------------------------------------------------------------
C4FrameBuffer *C4FrameBuffer::operator +(const C4FrameBuffer &other)
{
    add(other);
    return this;
}

//-----------------------------------------------------------------------------
void C4FrameBuffer::reset()
{
}
