#include "media/c4videomedia.h"

//-----------------------------------------------------------------------------
C4VideoMedia::C4VideoMedia(const QString &sourceName)
    : C4Media(), mVideo(sourceName)
{
    mCurrentFrame = mVideo.isValid() ? mVideo.nextFrame() : ImagePtr();
    if (!mCurrentFrame.isNull())
        mCurrentFrame->load();

}

//-----------------------------------------------------------------------------
C4VideoMedia::~C4VideoMedia()
{
}

//-----------------------------------------------------------------------------
int C4VideoMedia::width() const
{
    return mCurrentFrame.isNull() ? -1 : mCurrentFrame->img()->width();
}

//-----------------------------------------------------------------------------
int C4VideoMedia::height() const
{
    return mCurrentFrame.isNull() ? -1 : mCurrentFrame->img()->height();
}

//-----------------------------------------------------------------------------
void C4VideoMedia::setFrameIndex(int frameIndex)
{
    ImagePtr nextFrame = mCurrentFrame;
    while (mVideo.isValid() && mVideo.nextIndex() < frameIndex && !nextFrame.isNull())
        nextFrame = mVideo.nextFrame();

    if (!nextFrame.isNull()) {
        mCurrentFrame = nextFrame;
        mCurrentFrame->load();
    }
}

//-----------------------------------------------------------------------------
QRgb C4VideoMedia::pixel(int x, int y) const
{
    Q_ASSERT(!mCurrentFrame.isNull());
    Q_ASSERT(mCurrentFrame->img()->width()  > x);
    Q_ASSERT(mCurrentFrame->img()->height() > y);
    return mCurrentFrame->img()->pixel(x,y);
}
