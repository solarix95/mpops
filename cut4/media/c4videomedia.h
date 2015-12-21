#ifndef C4VIDEOMEDIA_H
#define C4VIDEOMEDIA_H

#include <QString>
#include "c4media.h"
#include "shared/imagearray.h"

class C4VideoMedia : public C4Media
{
public:
    C4VideoMedia(const QString &sourceName);
    virtual ~C4VideoMedia();

    virtual int   width() const;
    virtual int   height() const;
    virtual void  setFrameIndex(int frameIndex);
    virtual QRgb  pixel(int x, int y) const;

private:

    ImageArray mVideo;
    ImagePtr   mCurrentFrame;
};

#endif // C4VIDEOMEDIA_H
