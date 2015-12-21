#include <QDebug>
#include "painter.h"
#include "framebuffer.h"
#include "rotate.h"
#include "translate.h"
#include "scale.h"
#include "camera.h"
#include "math_transform.h"
#include "math_aux.h"

//-----------------------------------------------------------------------------
C4Painter::C4Painter(C4FrameBuffer *buffer, Camera *camera)
{
    mBuffer = buffer;
    mCamera = camera;
    Q_ASSERT(mBuffer);
}

//-----------------------------------------------------------------------------
C4Painter::~C4Painter()
{
    if (mCamera)
        delete mCamera;
}

//-----------------------------------------------------------------------------
void C4Painter::rotate(float *r)
{
    mTransforms << new Rotate(r);
}

//-----------------------------------------------------------------------------
void C4Painter::rotate(float ax, float ay, float az)
{
    mTransforms << new Rotate(ax,ay,az);
}

//-----------------------------------------------------------------------------
void C4Painter::translate(float *to)
{
    mTransforms << new Translate(to);
}

//-----------------------------------------------------------------------------
void C4Painter::translate(float dx, float dy, float dz)
{
    float p[3];
    p[0] = dx;
    p[1] = dy;
    p[2] = dz;
    translate(p);
}

//-----------------------------------------------------------------------------
void C4Painter::scale(float fx, float fy, float fz)
{
     mTransforms << new Scale(fx,fy,fz);
}

//-----------------------------------------------------------------------------
void C4Painter::scale(float *f)
{
     mTransforms << new Scale(f);
}

//-----------------------------------------------------------------------------
void C4Painter::setColor(const QColor &c)
{
    mState.color = c;
}

//-----------------------------------------------------------------------------
void C4Painter::setColor(QRgb c)
{
    mState.color = QColor(qRed(c),qGreen(c),qBlue(c),qAlpha(c)); // QColor(QRgb color) -> ignores Alpha!
}

//-----------------------------------------------------------------------------
void C4Painter::setColorMask(bool red, bool green, bool blue)
{
    mState.colorMatrix[0] = red;
    mState.colorMatrix[1] = 0;
    mState.colorMatrix[2] = 0;

    mState.colorMatrix[3] = 0;
    mState.colorMatrix[4] = green;
    mState.colorMatrix[5] = 0;

    mState.colorMatrix[6] = 0;
    mState.colorMatrix[7] = 0;
    mState.colorMatrix[8] = blue;
}

//-----------------------------------------------------------------------------
void C4Painter::setColorMatrix(float red1, float red2, float red3, float green1, float green2, float green3, float blue1, float blue2, float blue3)
{
    mState.colorMatrix[0] = red1;
    mState.colorMatrix[1] = red2;
    mState.colorMatrix[2] = red3;

    mState.colorMatrix[3] = green1;
    mState.colorMatrix[4] = green2;
    mState.colorMatrix[5] = green3;

    mState.colorMatrix[6] = blue1;
    mState.colorMatrix[7] = blue2;
    mState.colorMatrix[8] = blue3;
}

//-----------------------------------------------------------------------------
void C4Painter::mapPoint(const float *from, float *to)
{
    to[0] = from[0];
    to[1] = from[1];
    to[2] = from[2];

    foreach(Transformation *t, mTransforms)
        t->transform(to);
}

//-----------------------------------------------------------------------------
void C4Painter::mapPoint(float x, float y, float z, float *to)
{
    float p[] = { x,y,z };
    mapPoint(p,to);
}

//-----------------------------------------------------------------------------
float C4Painter::pixelPerPoint(const float *p1, const float *p2) const
{
    float p1t[] = { p1[0], p1[1], p1[2]};
    float p2t[] = { p2[0], p2[1], p2[2]};

    foreach(Transformation *t, mTransforms) {
        t->transform(p1t);
        t->transform(p2t);
    }

    if (mCamera) {
        mCamera->move(p1t);
        mCamera->move(p2t);
    }
    float rp1[2];
    float rp2[2];

    math::transform2(120,p1t,rp1);
    math::transform2(120,p2t,rp2);

    float rabs  = math::abs3(p1,p2);
    float rdist = mBuffer->realDistance(rp1,rp2);
    return rdist > 0 ? rabs/rdist : 0;
}

//-----------------------------------------------------------------------------
float C4Painter::pixelPerPoint(float x1, float y1, float z1, float x2, float y2, float z2) const
{
    float p1[] = { x1, y1, z1 };
    float p2[] = { x2, y2, z2 };

    return pixelPerPoint(p1,p2);
}


//-----------------------------------------------------------------------------
void C4Painter::drawPoint(float *p)
{
    float pTo[3];
    pTo[0] = p[0];
    pTo[1] = p[1];
    pTo[2] = p[2];

    foreach(Transformation *t, mTransforms)
        t->transform(pTo);
    float b[3];
    if (mCamera)
        mCamera->move(pTo);
    math::transform2(120,pTo,b);
    mBuffer->paintPixel(b[0],b[1],pTo[2],mState.getColor());
}

//-----------------------------------------------------------------------------
void C4Painter::drawPoint(float x, float y, float z)
{
    float p[3];
    p[0] = x; p[1] = y; p[2] = z;
    drawPoint(p);
}

//-----------------------------------------------------------------------------
void C4Painter::drawImage(const QImage &img)
{
    float a[3];
    for (float x=0; x<img.width(); x+=0.2)
        for (float y = 0; y<img.height(); y+=0.2) {
            a[0] = x-img.width()/2;
            a[1] = img.height()/2 - y;
            a[2] = 0;
            QRgb c = img.pixel(x,y);
            /*
            setColor(c);
            drawPoint(a);
         */
            if (qAlpha(c) > 127) {
                setColor(c);
                drawPoint(a);
            }

        }
}

//-----------------------------------------------------------------------------
void C4Painter::drawLine(float *p1, float *p2)
{

    drawPoint(p1);
    drawPoint(p2);

    float len = math::abs3(p1,p2);
    if (!len)
        return;

    float dx  = (p2[0] - p1[0])/len;
    float dy  = (p2[1] - p1[1])/len;
    float dz  = (p2[2] - p1[2])/len;

    float p[3];
    for (float i=0; i<len; i++) {
        p[0] = p1[0] + i*dx;
        p[1] = p1[1] + i*dy;
        p[2] = p1[2] + i*dz;
        drawPoint(p);
    }
}

//-----------------------------------------------------------------------------
void C4Painter::drawLine(float p1x, float p1y, float p1z, float p2x, float p2y, float p2z)
{
    float p1[] = { p1x, p1y, p1z };
    float p2[] = { p2x, p2y, p2z };
    drawLine(p1,p2);
}

//-----------------------------------------------------------------------------
void C4Painter::push()
{
    mState.stackDepth = mTransforms.count();
    mStack << mState;
}

//-----------------------------------------------------------------------------
void C4Painter::pop()
{
    Q_ASSERT(mStack.size() > 0);
    mState = mStack.takeLast();
    while (mTransforms.count() > mState.stackDepth)
        delete mTransforms.takeLast();
}
