#include <QDebug>
#include "simpleframebuffer.h"

//-----------------------------------------------------------------------------
C4SimpleFrameBuffer::C4SimpleFrameBuffer(const QSize &size)
{
    mBuffer = NULL;
    setSize(size);
}

//-----------------------------------------------------------------------------
void C4SimpleFrameBuffer::setSize(const QSize &size)
{
    if (mSize == size)
        return;

    C4FrameBuffer::setSize(size);
    init();
}

//-----------------------------------------------------------------------------
void C4SimpleFrameBuffer::reset()
{
    for (int i=0; i<mSize.height()*mSize.width(); i++)
        mBuffer[i] = mBackgroundColor;
}

//-----------------------------------------------------------------------------
void C4SimpleFrameBuffer::setPixel(int x, int y, const QColor &c)
{
    Q_ASSERT(x >= 0 && x < mSize.width());
    Q_ASSERT(y >= 0 && y < mSize.height());

    Q_ASSERT(mBuffer);
    mBuffer[ y*mSize.width() + x ] = c;
}

//-----------------------------------------------------------------------------
QColor C4SimpleFrameBuffer::pixel(int x, int y) const
{
    return mBuffer[ y*mSize.width() + x ];
}

//-----------------------------------------------------------------------------
void C4SimpleFrameBuffer::paintBegin()
{
}

//-----------------------------------------------------------------------------
void C4SimpleFrameBuffer::paintEnd()
{
}

//-----------------------------------------------------------------------------
QImage C4SimpleFrameBuffer::toImage() const
{
    QImage img(mSize,QImage::Format_RGB32);
    for (int y = 0; y<mSize.height(); y++) {
        QRgb *row = (QRgb*)img.scanLine(y);
        for (int x=0; x<mSize.width(); x++) {
            row[x] = mBuffer[y*mSize.width() + x].rgb();
        }
    }
    return img;
}

//-----------------------------------------------------------------------------
void C4SimpleFrameBuffer::add(const C4FrameBuffer &other)
{
    Q_ASSERT(this->size() == other.size());
    for (int y = 0; y<mSize.height(); y++) {
        for (int x=0; x<mSize.width(); x++) {
            mBuffer[y*mSize.width() + x].setRed(mBuffer[y*mSize.width() + x].red() + other.pixel(x,y).red());
            mBuffer[y*mSize.width() + x].setGreen(mBuffer[y*mSize.width() + x].green() + other.pixel(x,y).green());
            mBuffer[y*mSize.width() + x].setBlue(mBuffer[y*mSize.width() + x].blue() + other.pixel(x,y).blue());
        }
    }
}

//-----------------------------------------------------------------------------
void C4SimpleFrameBuffer::init()
{
    if (mBuffer) {
        delete [] mBuffer;
        mBuffer = NULL;
    }
    if (mSize.width() * mSize.height() > 0)
        mBuffer = new QColor[mSize.width() * mSize.height()];
}
