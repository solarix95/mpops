#include <QPainter>
#include <QTime>
#include "cinema.h"
#include "movie.h"

// -----------------------------------------------------------
Cinema::Cinema(QWidget *parent) :
    QWidget(parent)
{
    mCurrentFrame  = 0;
    mPlayDirection = 0;
    mLoop          = false;
    connect(&mPlayTimer,SIGNAL(timeout()), this, SLOT(showNextFrame()));
    mPlayTimer.start(1000/12);
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

    if (!mMovie || (mCurrentFrame < 0) || (mCurrentFrame >= mMovie->frameCount()))
        return;

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
