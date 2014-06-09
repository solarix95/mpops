#include "movie.h"
#include <QSize>
#include <QStringList>
#include <QThread>
#include <QCoreApplication>
#include <QDebug>

Movie::Movie(QObject *parent) :
    QObject(parent)
{
    mTopFrameIdent = 1;
}

int Movie::frameCount() const
{
    lock();
    int ret = mFrames.count();
    unlock();
    return ret;
}

QSize Movie::thumbSize()
{
    return QSize(70,70);
}

QSize Movie::renderSize()
{
    return QSize(1280,720);
}

void Movie::addFrames(const QStringList &fileList)
{
    lock();
    foreach(QString nextImage, fileList) {
        mFrames << new Frame();
        mFrames.last()->ident     = mTopFrameIdent++;
        mFrames.last()->source    = nextImage;
        mFrames.last()->rendered  = ImagePtr(new QImage());
        mFrames.last()->thumbnail = ImagePtr(new QImage());
        // mFrames.last()->thumbnail->load(nextImage);
        // *(mFrames.last()->thumbnail) = mFrames.last()->thumbnail->scaled(thumbSize(),Qt::KeepAspectRatio,Qt::SmoothTransformation);

        emit frameAppended(mFrames.count()-1);
    }
    unlock();
}

QImage *Movie::thumbNail(int frame) const
{
    Q_ASSERT(frame >= 0 && frame < mFrames.count());
    return mFrames.at(frame)->thumbnail.data();
}

QImage *Movie::rendered(int frame) const
{
    QImage *ret = NULL;
    lock();
    Q_ASSERT(frame >= 0 && frame < mFrames.count());
    if (!mFrames[frame]->rendered->isNull()) {
        ret = mFrames[frame]->rendered.data();
    } else if (!mFrames[frame]->thumbnail->isNull()) {
        ret = mFrames[frame]->thumbnail.data();
    }

    unlock();

    return ret;
}

bool Movie::frame(int index, qint64 &id, bool &hasThumb, bool &isRendered, QString &source)
{
    lock();

    bool ret = index >= 0 && index < mFrames.count();
    if (ret) {
        id          = mFrames[index]->ident;
        hasThumb    = !mFrames[index]->thumbnail->isNull();
        isRendered  = !mFrames[index]->rendered->isNull();
        source      = mFrames[index]->source;
    }
    unlock();

    return ret;
}

void Movie::lock() const
{
    const_cast<Movie*>(this)->mMutex.lock();
}


void Movie::unlock() const
{
    const_cast<Movie*>(this)->mMutex.unlock();
}

void Movie::jobThumb(int index, qint64 id, QImage thumb)
{
    lock();
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    Q_ASSERT(index >= 0);

    bool changed = false;
    if (index < mFrames.count() && mFrames[index]->ident == id) {
        *(mFrames[index]->thumbnail) = thumb;
        changed = true;
    }
    unlock();

    if (changed)
        emit frameChanged(index);
}

void Movie::jobRender(int index, qint64 id, QImage result)
{
    lock();
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    Q_ASSERT(index >= 0);

    bool changed = false;
    if (index < mFrames.count() && mFrames[index]->ident == id) {
        *(mFrames[index]->rendered) = result;
        changed = true;
    }
    unlock();

    if (changed)
        emit frameChanged(index);

}
