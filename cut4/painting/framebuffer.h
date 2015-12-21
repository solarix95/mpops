#ifndef BUFFER_H
#define BUFFER_H

#include <QColor>
#include <QSize>
#include <QImage>

class C4FrameBuffer
{
public:
    virtual void   setSize(const QSize &size);
    virtual QSize  size() const;
    virtual void   setBackgroundColor(const QColor &c);
    virtual void   setFrustum(float left, float right, float bottom, float top, float nearVal, float farVal);
    virtual void   setFrustum(float left, float right, float aspect, float nearVal, float farVal);


    void   paintPixel(float x, float y, float z, const QColor &c);
    float realDistance(const float *p1, const float *p2) const;
    virtual QColor pixel(int x, int y) const = 0;

    virtual void   reset() = 0;
    virtual void   paintBegin() = 0;
    virtual void   paintEnd()   = 0;

    virtual QImage toImage() const = 0;

    C4FrameBuffer *operator+(const C4FrameBuffer &other);

protected:
    C4FrameBuffer();
    virtual void   setPixel(int x, int y, const QColor &c) = 0;
    virtual void   add(const C4FrameBuffer &other) = 0;

    QColor mBackgroundColor;
    QSize  mSize;

private:
    float  mFrustumLeft;
    float  mFrustumRight;
    float  mFrustumBottom;
    float  mFrustumTop;
    float  mFrustumNear;
    float  mFrustumFar;
};

#endif // BUFFER_H
