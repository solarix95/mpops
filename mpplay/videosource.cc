#include <QDebug>
#include <QTime>
#include <QCoreApplication>
#include "videosource.h"
#include "imageloaderjob.h"

#define THREAD_CONTEXT     Q_ASSERT(QThread::currentThread() == this)
#define MAIN_CONTEXT       Q_ASSERT(QThread::currentThread() == qApp->thread())

//-----------------------------------------------------------------------------
VideoSource::VideoSource(QObject *parent) :
    QThread(parent)
{
    MAIN_CONTEXT;

    mArray = NULL;
    mCurrentFrame = -1;
    mFrameCount   =  0;
    mMaxWidth     = -1;
    mAtEnd        = false;

    moveToThread(this);
    connect(this, SIGNAL(initRequest(QString)), this, SLOT(threadedInit(QString)));
    connect(this, SIGNAL(loadFrame(int)), this, SLOT(threadedLoad(int)));

    for (int i=0; i < (QThread::idealThreadCount()*2); i++) {
        mStartingThreads << new QThread();
        connect(mStartingThreads.last(), SIGNAL(started()), this, SLOT(threadedLoaderStarted()));
        mStartingThreads.last()->start();
    }
}

//-----------------------------------------------------------------------------
VideoSource::~VideoSource()
{
    QList<QThread*> workers;
    workers << mStartingThreads << mLoaders;
    foreach(QThread *next, workers) {
        next->quit();
        next->wait();
        delete next;
    }
}

//-----------------------------------------------------------------------------
void VideoSource::load(const QString &source)
{
    mFrameCount = 0;
    mAtEnd      = false;
    emit initRequest(source);
}

//-----------------------------------------------------------------------------
void VideoSource::requestFrame(int index)
{
    emit loadFrame(index);
}

//-----------------------------------------------------------------------------
void VideoSource::setMaxWidth(int maxWidth)
{
    Q_ASSERT(maxWidth); // everything except 0 is ok..
    mMaxWidth = maxWidth;
}

//-----------------------------------------------------------------------------
int VideoSource::frameCount() const
{
    return mFrameCount;
}

//-----------------------------------------------------------------------------
bool VideoSource::atEnd() const
{
    return mAtEnd;
}

//-----------------------------------------------------------------------------
void VideoSource::threadedInit(const QString &source)
{
    THREAD_CONTEXT;
    if (mArray)
        delete mArray;

    mArray        = new ImageArray(source);
    mCurrentFrame = -1;
    mFrameCount   = mArray->frameCount();
    mLoadingImages.clear();
    emit sourceReady();

}

//-----------------------------------------------------------------------------
void VideoSource::threadedLoad(int index)
{
    // qDebug() << "VideoSource::threadedLoad" << index;
    THREAD_CONTEXT;
    if (!mArray)
        return;

    if (mArray->nextIndex() > index) {
        mArray->reset();
        mLoadingImages.clear();
        mAtEnd = false;
    }

    while (mArray->nextIndex() < index) {
        // qDebug() << "SKIP" << mArray->nextIndex();
        if (!mArray->skipFrame())
            break;
    }

    mArray->setMaxWidthHint(mMaxWidth);
    ImagePtr nextFrame = mArray->nextFrame();
    if (nextFrame.isNull()) {
        mAtEnd = true;
    } else
        threadedFrameLoader(index,nextFrame);

    // (*nextFrame->img()) = nextFrame->img()->scaled(QSize(1280,720),Qt::KeepAspectRatio);
    // emit frameLoaded(index,nextFrame);
}

//-----------------------------------------------------------------------------
void VideoSource::threadedLoaderStarted()
{
    THREAD_CONTEXT;
    QThread *thr = (QThread*)sender();
    Q_ASSERT(mStartingThreads.contains(thr));
    mLoaders << mStartingThreads.takeAt(mStartingThreads.indexOf(thr));
}

//-----------------------------------------------------------------------------
void VideoSource::threadedLoadDone(int index, ImagePtr frame)
{
    int jobIndex = -1;
    for (int i=0; i<mLoadingImages.count(); i++) {
        if (mLoadingImages.at(i).first == index) {
            mLoadingImages[i].second = frame;
            jobIndex = i;
            break;
        }
    }
    if (jobIndex < 0)
        return;

    while (!mLoadingImages.isEmpty() && !mLoadingImages.first().second.isNull()) {
        emit frameLoaded(mLoadingImages.first().first,mLoadingImages.first().second);
        mLoadingImages.takeFirst();
    }
}

//-----------------------------------------------------------------------------
void VideoSource::threadedFrameLoader(int index, ImagePtr frame)
{
    THREAD_CONTEXT;
    if (mLoaders.isEmpty() || !frame->img()->isNull()) {
        frame->load();
        emit frameLoaded(index,frame);
    } else {
        ImageLoaderJob *job = new ImageLoaderJob(index,frame);
        connect(job,SIGNAL(loaded(int,ImagePtr)), this, SLOT(threadedLoadDone(int,ImagePtr)));
        mLoadingImages << qMakePair(index,ImagePtr());
        job->moveToThread(mLoaders[index % mLoaders.count()]);
        job->start();
    }
}
