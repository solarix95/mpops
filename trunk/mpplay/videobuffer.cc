#include <QDebug>
#include "videobuffer.h"

#define DEFAULT_SIZE      (10*24)
#define IMG_SIZE(imgPtr)  (imgPtr->img()->size().width() * imgPtr->img()->size().height() * 3)

//-----------------------------------------------------------------------------
VideoBuffer::VideoBuffer(QObject *parent) :
    QObject(parent)
{
    mSource     = NULL;
    mBufferSize = 0;
    mMemoryUsage= 0;
    mState      = Undef;
    mLastIndex      = -1;
    mMaxImageWidth  = -1;
}

//-----------------------------------------------------------------------------
VideoBuffer::~VideoBuffer()
{
    destroy();
}

//-----------------------------------------------------------------------------
void VideoBuffer::setSource(VideoSource *source)
{
    destroy();
    mSource = source;
    if (mSource) {
        mSource->setMaxWidth(mMaxImageWidth);
        connect(mSource,SIGNAL(frameLoaded(int,ImagePtr)), this, SLOT(appendFrame(int,ImagePtr)));
        connect(mSource,SIGNAL(sourceReady()), this, SLOT(initBuffer()));
    }
}

//-----------------------------------------------------------------------------
void VideoBuffer::setMaxWidth(int width)
{
    mMaxImageWidth = width;
    if (mSource)
        mSource->setMaxWidth(mMaxImageWidth);
}

//-----------------------------------------------------------------------------
ImagePtr VideoBuffer::takeFrame(bool *atEnd)
{
    // qDebug() << "TAKE, Requests:" << mRequests.count();
    ImagePtr ret;
    mCurrentIndex++;
    if (atEnd)
        *atEnd = false;

    if (mBuffer.contains(mCurrentIndex)) {
        ret = mBuffer.value(mCurrentIndex);
        mBuffer.remove(mCurrentIndex);
        mMemoryUsage -=  IMG_SIZE(ret);

        if (mBuffer.isEmpty()) {
            emit bufferEmpty();
        }
        emit bufferSize(mBuffer.size());
    } else {
        if (atEnd && (!mSource || mSource->atEnd()))
            *atEnd = true;
    }

    if (mSource && !mSource->atEnd()) {
        setState(Buffering);
        request(mLastIndex+1);
    }
    return ret;
}

//-----------------------------------------------------------------------------
VideoBuffer::State VideoBuffer::state() const
{
    return mState;
}

//-----------------------------------------------------------------------------
int VideoBuffer::size() const
{
    return mBuffer.size();
}

//-----------------------------------------------------------------------------
void VideoBuffer::setCurrentIndex(int index)
{
    mRequests.clear();
    mCurrentIndex   = index-1;

    mBuffer.clear();
    mMemoryUsage = 0;

    /*
    while (mBuffer.size() > 0 && (mBuffer.begin().key() < index)) {
        mMemoryUsage -=  IMG_SIZE(mBuffer[mBuffer.begin().key()]);
        mBuffer.remove(mBuffer.begin().key());
    }
    */

    // qDebug() << "Jump to" << index;
    request(index);
}

//-----------------------------------------------------------------------------
int VideoBuffer::currentIndex() const
{
    return mCurrentIndex;
}

//-----------------------------------------------------------------------------
int VideoBuffer::lastIndex() const
{
    return mLastIndex;
}

//-----------------------------------------------------------------------------
int VideoBuffer::requestingIndex() const
{
    if (mRequests.isEmpty())
        return -1;
    return mRequests.first();
}

//-----------------------------------------------------------------------------
int VideoBuffer::sourceLength() const
{
    return mSource ? mSource->frameCount() : 0;
}

//-----------------------------------------------------------------------------
qint64 VideoBuffer::memoryUsage() const
{
    return mMemoryUsage;
}

//-----------------------------------------------------------------------------
void VideoBuffer::appendFrame(int index, ImagePtr img)
{
    // qDebug() << " VideoBuffer::appendFrame" << index;
    if ((sender() != mSource) || (requestingIndex() != index))
        return;

    if (img.isNull()) { // end reached
        emit bufferFull();
        mRequests.clear();
        setState(Idle);
        return;
    }

    if (!mBuffer.contains(index)) {
        mBuffer[index]  = img;
        mMemoryUsage   += IMG_SIZE(img);
    }

    // qDebug() << img->img()->size();
    Q_ASSERT(!mRequests.isEmpty() && mRequests.first() == index);
    mRequests.takeFirst();
    mLastIndex      = index;
    emit bufferSize(mBuffer.size());

    if (mBuffer.size() >= mBufferSize) {
        emit bufferFull();
        setState(Idle);
    } else
        request(mLastIndex + 1);
}

//-----------------------------------------------------------------------------
void VideoBuffer::initBuffer()
{
    Q_ASSERT(mSource);
    mBufferSize   = qMin(DEFAULT_SIZE,(int)(mSource->frameCount()*0.2));
    mStartFrame   = 0;
    mCurrentIndex = 0;
    mMemoryUsage  = 0;
    mBuffer.clear();

    setState(Buffering);
}

//-----------------------------------------------------------------------------
void VideoBuffer::setState(VideoBuffer::State st)
{
    if (mState == st)
        return;
    mState = st;
    switch(mState) {
      case Buffering:
        request(mLastIndex+1);
        break;
      case Idle:      break;
    }

    // Debug() << "VideoBuffer::setState" << mState;
    emit stateChanged(mState);
}

//-----------------------------------------------------------------------------
void VideoBuffer::destroy()
{
    mBuffer.clear();
    mStartFrame   = -1;
    mCurrentIndex = -1;
}

//-----------------------------------------------------------------------------
void VideoBuffer::request(int index)
{
    if (!mSource || mRequests.contains(index))
        return;

    if (index <= mCurrentIndex) { // Random-Access -> Reset!
        mBuffer.clear();
        mRequests.clear();
        mStartFrame   = mCurrentIndex = index;
        mMemoryUsage  = 0;
        emit bufferSize(0);
    }
    mRequests << index;
    mSource->requestFrame(index);
    emit requesting();

    while (mRequests.count() < QThread::idealThreadCount()*4)
        request(index+1);
}
