#ifndef VIDEOBUFFER_H
#define VIDEOBUFFER_H

#include <QObject>
#include <QMap>

#include "shared/image.h"
#include "videosource.h"

/*
     VideoBuffer:

     Video  :  |----------------------------------|
     Buffer :           |------|------|----|
                      Start  Current min   max
*/

class VideoBuffer : public QObject
{
    Q_OBJECT
public:
    enum State {
        Undef,
        Init,
        Buffering,  // 0   .. size
        Idle
    };

    explicit VideoBuffer(QObject *parent = 0);
    virtual ~VideoBuffer();

    void     setSource(VideoSource *source);
    void     setMaxWidth(int width);

    ImagePtr takeFrame(bool *atEnd = NULL);
    State    state() const;
    int      size() const;
    void     setCurrentIndex(int index);
    int      currentIndex() const;
    int      lastIndex() const;
    int      requestingIndex() const;
    int      sourceLength() const;
    qint64   memoryUsage() const;

signals:
    void bufferFull();
    void bufferEmpty();
    void bufferSize(int size);
    void atEnd();
    void requesting();
    void stateChanged(int newState);

public slots:

private slots:
    void appendFrame(int index, ImagePtr img);
    void initBuffer();

private:
    void setState(State st);
    void destroy();
    void request(int index);

    VideoSource        *mSource;
    QMap<int,ImagePtr>  mBuffer;

    int        mStartFrame;
    int        mCurrentIndex;
    int        mLastIndex;
    int        mBufferSize;
    QList<int> mRequests;

    int    mMaxImageWidth;
    qint64 mMemoryUsage;

    State mState;

};

#endif // VIDEOBUFFER_H
