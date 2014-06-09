#include <QTimer>
#include <QDebug>
#include "workerthread.h"
#include "movie.h"

WorkerThread::WorkerThread(QObject *parent) :
    QThread(parent)
{
    mMovie = NULL;
    moveToThread(this);
}

void WorkerThread::start(Movie *movie)
{
    mMovie = movie;
    QThread::start();
}

void WorkerThread::run()
{
    Q_ASSERT(mMovie);
    QTimer t;
    connect(&t, SIGNAL(timeout()), this, SLOT(workRun()), Qt::DirectConnection);
    t.start(200);
    exec();
}

void WorkerThread::workRun()
{
    Q_ASSERT(QThread::currentThread() == this);
    bool isComplete = true;
    for (int i=0; i<mMovie->frameCount(); i++) {
        qint64  id;
        qint32  type;
        bool    hasThumb;
        bool    isRendered;
        QString source;
        if (!mMovie->frame(i,id,type,hasThumb,isRendered,source))
            break;
        if (!hasThumb) {
            createThumb(i,id,type,source);
            continue;
        }
        if (!isRendered) {
            isComplete = render(i,id,type,source) && isComplete;
            continue;
        }
    }

    if (isComplete)
        emit movieCompleteRendered();
}

void WorkerThread::createThumb(int frameIndex,qint64 frameId,  qint32 type, const QString &source)
{
    QImage img;

    switch (type) {
    case Movie::FileSource : {
        img.load(source);
    } break;
    case Movie::FreezeFrame : {
        qint64 relIdent;
        QImage fullFrame;
        if (mMovie->relativeImage(frameIndex,frameId,-1,relIdent,fullFrame)) {
            img = fullFrame;
        }
    }
    }

    if (!img.isNull()) {
        img = img.scaled(Movie::thumbSize(),Qt::KeepAspectRatio,Qt::FastTransformation);
        emit thumbCreated(frameIndex,frameId,img);
    }
}

bool WorkerThread::render(int frameIndex, qint64 frameId, qint32 type, const QString &source)
{
    QImage img;

    switch (type) {
    case Movie::FileSource : {
        img.load(source);
    } break;
    case Movie::FreezeFrame : {
        qint64 relIdent;
        QImage fullFrame;
        if (mMovie->relativeImage(frameIndex,frameId,-1,relIdent,fullFrame)) {
            img = fullFrame;
        }
    }
    }

    if (!img.isNull()) {
        img = img.scaled(Movie::renderSize(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        emit rendered(frameIndex,frameId,img);
    }

    return !img.isNull();
}
