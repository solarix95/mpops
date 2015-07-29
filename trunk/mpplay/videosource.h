#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <QThread>
#include "shared/imagearray.h"
#include <QPair>
#include <QList>

class VideoSource : public QThread
{
    Q_OBJECT
public:
    explicit VideoSource(QObject *parent = 0);
    virtual ~VideoSource();
    
    void load(const QString &source);
    void requestFrame(int index);
    void setMaxWidth(int maxWidth);
    int  frameCount() const;
    bool atEnd() const;

signals:
    void sourceReady();
    void frameLoaded(int index, ImagePtr frame); // Anwser to "requestFrame"

    void initRequest(const QString &source);     // internal use only
    void loadFrame(int index);                   // internal use only

private slots:
    void threadedInit(const QString &source);
    void threadedLoad(int index);
    void threadedLoaderStarted();
    void threadedLoadDone(int index, ImagePtr frame);

private:
    void threadedFrameLoader(int index, ImagePtr frame);


    // Public Members
    int          mFrameCount;
    bool         mAtEnd;

    // Thread-Members:
    ImageArray                   *mArray;
    int                          mCurrentFrame;
    int                          mMaxWidth;

    QList<QThread*>              mStartingThreads;
    QList<QThread*>              mLoaders;
    QList<QPair<int, ImagePtr> > mLoadingImages;
};

#endif // VIDEOSOURCE_H
