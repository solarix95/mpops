#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QDragEnterEvent>
#include <QDropEvent>
#include "cinema.h"
#include "movie.h"
#include "utils.h"

#define INIT_ANIM_TIME 5000

// -----------------------------------------------------------
Cinema::Cinema(QWidget *parent) :
    QWidget(parent)
{
    mCurrentFrame  = 0;
    mPlayDirection = 0;
    mLoop          = false;
    mStartTime.start();
    connect(&mPlayTimer,SIGNAL(timeout()), this, SLOT(showNextFrame()));
    mPlayTimer.start(1000/12);
    setAcceptDrops(true);

    QTimer *t = new QTimer();
    connect(t, SIGNAL(timeout()), this, SLOT(initAnimation()));
    t->start(25);
}

// -----------------------------------------------------------
void Cinema::setMovie(Movie *movie)
{
    mMovie = movie;
    connect(mMovie, SIGNAL(frameChanged(int)), this, SLOT(frameChanged(int)));
    connect(mMovie, SIGNAL(fpsChanged(int)), this, SLOT(fpsChanged(int)));
}

// -----------------------------------------------------------
void Cinema::setFrame(int index)
{
    if (index != mCurrentFrame) {
        mCurrentFrame = index;
        emit currentFrame(index);
        update();
    }
}

// -----------------------------------------------------------
void Cinema::playFwd()
{
    mPlayDirection = +1;
}

// -----------------------------------------------------------
void Cinema::playBack()
{
    mPlayDirection = -1;
}

// -----------------------------------------------------------
void Cinema::pause()
{
    mPlayDirection = 0;
}

// -----------------------------------------------------------
void Cinema::playLoop(bool doLoop)
{
    mLoop = doLoop;
}

// -----------------------------------------------------------
void Cinema::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    p.drawRect(rect());


    if (!mMovie || (mCurrentFrame < 0) || (mCurrentFrame >= mMovie->frameCount())) {
        if (mStartTime.elapsed() < INIT_ANIM_TIME) {
            int d = mStartTime.elapsed()/(float)INIT_ANIM_TIME * 255.0;
            p.setPen(QColor(255-d,255-d,255-d));
            p.drawText(rect(),Qt::AlignCenter,"Welcome back, director.");
        }
        return;
    }

    if (!mMovie->rendered(mCurrentFrame) || mMovie->rendered(mCurrentFrame)->isNull())
        return;

    QImage img = mMovie->rendered(mCurrentFrame)->scaled(width()-10,height()-70,Qt::KeepAspectRatio);
    p.drawImage((width()-img.width())/2,(height()-img.height())/2,img);

    int msec = mCurrentFrame/(double)mMovie->fps() * 1000;
    int h    = msec/3600000;
    int m    = (msec-h*3600000)/60000;
    int s    = (msec-h*3600000-m*60000)/1000;
    int ms   = msec % 1000;
    QString infoText = QString("frame %1 / %2")
                            .arg(mCurrentFrame)
                            .arg(QTime(h,m,s,ms).toString("hh:mm:ss.zzz"));
    p.setPen(Qt::white);
    p.drawText(QRect(0,10,width(),height()),Qt::AlignTop | Qt::AlignHCenter, infoText);
    p.drawText(QRect(0,10,width(),height()),Qt::AlignTop | Qt::AlignHCenter, infoText);
    p.drawText(QRect(0,height()-30,width(),height()),Qt::AlignTop | Qt::AlignHCenter, mMovie->source(mCurrentFrame));
}

// -----------------------------------------------------------
void Cinema::dropEvent(QDropEvent *e)
{
    if (!mMovie)
        return;

    QList<QByteArray> fileUrls = Darksuit::filesFromMimeData(e->mimeData());
    foreach(QByteArray nextFile, fileUrls) {
            mMovie->addFrame(nextFile);
    }
}

// -----------------------------------------------------------
void Cinema::dragEnterEvent(QDragEnterEvent *e)
{
    if (!mMovie)
        return;

    if (e->mimeData()->hasFormat("text/uri-list"))
             e->acceptProposedAction();
}

// -----------------------------------------------------------
void Cinema::frameChanged(int frameIndex)
{
    if (frameIndex == mCurrentFrame)
        update();
}

// -----------------------------------------------------------
void Cinema::showNextFrame()
{
    if (!mPlayDirection)
        return;

    int nextFrame = mCurrentFrame + mPlayDirection;
    if (nextFrame < 0) {
        if (mLoop) {
            nextFrame = mMovie->frameCount() - 1;
        } else {
            nextFrame = 0;
            pause();
        }
    }
    if (mMovie && nextFrame >= mMovie->frameCount()) {
        if (mLoop) {
            nextFrame = 0;
        } else {
            nextFrame = mMovie->frameCount() - 1;
            pause();
        }
    }
    if (nextFrame != mCurrentFrame) {
        setFrame(nextFrame);
    }

}

// -----------------------------------------------------------
void Cinema::fpsChanged(int newFps)
{
    mPlayTimer.start(1000/newFps);
}

// -----------------------------------------------------------
void Cinema::initAnimation()
{
    Q_ASSERT(dynamic_cast<QTimer*>(sender()));
    if (mStartTime.elapsed() > INIT_ANIM_TIME) {
        dynamic_cast<QTimer*>(sender())->deleteLater();
    }
    update();

}
