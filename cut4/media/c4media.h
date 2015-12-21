#ifndef C4MEDIA_H
#define C4MEDIA_H

#include <QRect>
#include <QColor>
class C4Media
{
public:
    C4Media();

    virtual int   width() const = 0;
    virtual int   height() const = 0;
    virtual void  setFrameIndex(int frameIndex);
    virtual QRgb  pixel(int x, int y) const = 0;

    // Aliases
    QSize size() const;
};

#endif // C4MEDIA_H
