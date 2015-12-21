#include <QDebug>
#include "media/c4imagemedia.h"
#include "shape/c4mediashape.h"
#include "painting/painter.h"

//-----------------------------------------------------------------------------
C4MediaShape::C4MediaShape(C4Media *media)
    : C4Shape(), mMedia(media)
{
    Q_ASSERT(mMedia);
}

//-----------------------------------------------------------------------------
C4MediaShape::~C4MediaShape()
{
}

//-----------------------------------------------------------------------------
float C4MediaShape::radius() const
{
    return qMax(mMedia->width()/2, mMedia->height()/2);
}

//-----------------------------------------------------------------------------
void C4MediaShape::shapeRender(C4Painter *painter,int frame)
{
    float a[3];
    mMedia->setFrameIndex(frame);

    float ppp = pixelPerPoint(painter);

    for (float x=0; x<mMedia->width(); x+=ppp) {
        for (float y = 0; y<mMedia->height(); y+=ppp) {
            a[0] = x-mMedia->width()/2;
            a[1] = mMedia->height()/2 - y;
            a[2] = 0;
            QRgb c = mMedia->pixel(x,y);

            if (qAlpha(c) > 127) {
                painter->setColor(c);
                painter->drawPoint(a);
            }
        }
    }
}

//-----------------------------------------------------------------------------
float C4MediaShape::pixelPerPoint(C4Painter *painter) const
{
    float x = -mMedia->width()/2;
    float y = mMedia->height()/2;
    float h = mMedia->height();
    float w = mMedia->width();
    float pppTopLeft    = painter->pixelPerPoint(x,  y,0,x+1,y+1,0);
    float pppBottomLeft = painter->pixelPerPoint(x,y+h,0, x+1, y+h-1,0);
    float pppTopRight   = painter->pixelPerPoint(x+w,y,0, x+w-1, y+1,0);
    float pppBottomRight= painter->pixelPerPoint(x+w,y+h,0, x+w-1, y+h-1,0);

    return qMin(qMax(pppTopLeft,pppBottomLeft),qMax(pppTopRight,pppBottomRight));
}


