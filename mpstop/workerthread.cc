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
    for (int i=0; i<mMovie->frameCount(); i++) {
        qint64  id;
        bool    hasThumb;
        bool    isRendered;
        QString source;
        if (!mMovie->frame(i,id,hasThumb,isRendered,source))
            break;
        if (!hasThumb) {
            createThumb(i,id,source);
            continue;
        }
        if (!isRendered) {
            render(i,id,source);
            continue;
        }
    }
}

void WorkerThread::createThumb(int frameIndex, qint64 frameId, const QString &source)
{
    QImage img;
    img.load(source);
    img = img.scaled(Movie::thumbSize(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    emit thumbCreated(frameIndex,frameId,img);
}

void WorkerThread::render(int frameIndex, qint64 frameId, const QString &source)
{
    QImage img;
    img.load(source);
    img = img.scaled(Movie::renderSize(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    emit rendered(frameIndex,frameId,img);
}
