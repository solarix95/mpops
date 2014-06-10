#include "movie.h"
#include <QSize>
#include <QStringList>
#include <QThread>
#include <QCoreApplication>
#include <QDebug>

// -----------------------------------------------------------
Movie::Movie(QObject *parent) :
    QObject(parent)
{
    mTopFrameIdent = 1;
    mFps           = 12;
}

// -----------------------------------------------------------
int Movie::frameCount() const
{
    lock();
    int ret = mFrames.count();
    unlock();
    return ret;
}

// -----------------------------------------------------------
int Movie::fps() const
{
    return mFps;
}

// -----------------------------------------------------------
void Movie::setFps(int fps)
{
    Q_ASSERT(mFps > 0);
    if (fps == mFps)
        return;

    mFps = fps;
    emit fpsChanged(mFps);
}

// -----------------------------------------------------------
void Movie::setVideoWidth(int w)
{
    lock();
    mRenderSize.setWidth(w);
    unlock();
    resetRendering();
}

// -----------------------------------------------------------
void Movie::setVideoHeight(int h)
{
    lock();
    mRenderSize.setHeight(h);
    unlock();
    resetRendering();

}

// -----------------------------------------------------------
QSize Movie::thumbSize()
{
    return QSize(70,70);
}

// -----------------------------------------------------------
QSize Movie::renderSize()
{
    lock();
    QSize ret = mRenderSize;
    unlock();
    return ret;
}

// -----------------------------------------------------------
void Movie::addFrames(const QStringList &fileList)
{
    lock();
    int startIndex = mFrames.count();
    foreach(QString nextImage, fileList) {
        mFrames << new Frame();
        mFrames.last()->type      = FileSource;
        mFrames.last()->ident     = mTopFrameIdent++;
        mFrames.last()->source    = nextImage;
        mFrames.last()->rendered  = ImagePtr(new QImage());
        mFrames.last()->thumbnail = ImagePtr(new QImage());
    }
    unlock();

    emit isComplete(false);
    for(int i=startIndex; i<(startIndex + fileList.count()); i++)
        emit frameAppended(i);
}

Movie::FrameType Movie::type(int frame) const
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    Q_ASSERT(frame >= 0 && frame < mFrames.count());
    return mFrames.at(frame)->type;
}

// -----------------------------------------------------------
QImage *Movie::thumbNail(int frame) const
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    Q_ASSERT(frame >= 0 && frame < mFrames.count());
    return mFrames.at(frame)->thumbnail.data();
}

// -----------------------------------------------------------
QImage *Movie::rendered(int frame) const
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());

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

// -----------------------------------------------------------
bool Movie::frame(int index, qint64 &id, qint32 &type, bool &hasThumb, bool &isRendered, QString &source)
{
    lock();

    bool ret = index >= 0 && index < mFrames.count();
    if (ret) {
        id          = mFrames[index]->ident;
        type        = mFrames[index]->type;
        hasThumb    = !mFrames[index]->thumbnail->isNull();
        isRendered  = !mFrames[index]->rendered->isNull();
        source      = mFrames[index]->source;
    }
    unlock();

    return ret;
}

// -----------------------------------------------------------

bool Movie::relativeImage(int fromIndex, qint64 fromId, int relIndex, qint64 &relId, QImage &renderedImage)
{
    lock();

    bool ret = fromIndex >= 0 && (fromIndex < mFrames.count()) && mFrames[fromIndex]->ident == fromId;
    if (ret) {
        int newIndex = fromIndex + relIndex;
        if (newIndex >= 0 && newIndex < mFrames.count()) { // Neuer Index gültig?
            renderedImage = *mFrames[newIndex]->rendered;
            relId         = mFrames[newIndex]->ident;
        } else
            ret = false;
    }

    unlock();
    return ret;
}

// -----------------------------------------------------------
void Movie::setMovieIsComplete()
{
    emit isComplete(true);
}

// -----------------------------------------------------------
void Movie::lock() const
{
    const_cast<Movie*>(this)->mMutex.lock();
}

// -----------------------------------------------------------
void Movie::unlock() const
{
    const_cast<Movie*>(this)->mMutex.unlock();
}

// -----------------------------------------------------------

void Movie::resetRendering()
{
    lock();
    for (int i=0; i<mFrames.count(); i++) {
        *mFrames[i]->rendered = QImage();
    }
    unlock();
    emit isComplete(false);
}

// -----------------------------------------------------------
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

// -----------------------------------------------------------
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

// -----------------------------------------------------------
void Movie::addFreezeFrame(int startIndex)
{
    lock();
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    Q_ASSERT(startIndex >= 0);
    if (startIndex > mFrames.count())
        startIndex = mFrames.count();

    Frame *frm = new Frame();
    frm->type      = FreezeFrame;
    frm->ident     = mTopFrameIdent++;
    frm->rendered  = ImagePtr(new QImage());
    frm->thumbnail = ImagePtr(new QImage());
    mFrames.insert(startIndex,frm);

    emit isComplete(false);
    unlock();

    emit frameInserted(startIndex);
}

void Movie::removeFrame(int startIndex)
{
    qDebug() << "MOVIE: DELTE" << startIndex;
    lock();
    Q_ASSERT(QThread::currentThread() == qApp->thread());
    Q_ASSERT(startIndex >= 0);
    delete mFrames.takeAt(startIndex);
    unlock();

    emit frameDeleted(startIndex);
}
