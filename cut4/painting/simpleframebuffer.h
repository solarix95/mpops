#ifndef SIMPLEFRAMEBUFFER_H
#define SIMPLEFRAMEBUFFER_H

#include "framebuffer.h"

class C4SimpleFrameBuffer : public C4FrameBuffer
{
public:
    C4SimpleFrameBuffer(const QSize &size = QSize());

    virtual void   setSize(const QSize &size);

    virtual void   reset();
    virtual void   paintBegin();

    virtual void   setPixel(int x, int y, const QColor &c);
    virtual QColor pixel(int x, int y) const;

    virtual void   paintEnd();

    virtual QImage toImage() const;

protected:
    virtual void   add(const C4FrameBuffer &other);

private:
    void init();

    QColor *mBuffer;
};

#endif // SIMPLEFRAMEBUFFER_H
