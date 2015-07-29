#include <QPainter>
#include <QColor>
#include <QImage>
#include <QRect>
#include <QKeyEvent>
#include <QDebug>
#include <QResizeEvent>

#include "videoplayer.h"

#define MIN_FPS   1
#define MAX_FPS  48
#define STD_FPS  24

// -----------------------------------------------------------
VideoPlayer::VideoPlayer(QWidget *parent) :
    QWidget(parent)
{
    mBuffer  = NULL;
    mPlayFps = STD_FPS;
    mState   = Undef;
    connect(&mTimer,SIGNAL(timeout()), this, SLOT(grabNextFrame()));
}

// -----------------------------------------------------------
void VideoPlayer::setBuffer(VideoBuffer *buffer)
{
    mBuffer       = buffer;
    mCurrentFrame = ImagePtr();
    mState        = Busy;

    mBuffer->setMaxWidth(width());
    mTimer.setInterval(1000/mPlayFps);
    connect(mBuffer,SIGNAL(stateChanged(int)), this, SLOT(bufferStateChanged(int)));
    connect(mBuffer,SIGNAL(bufferFull()), this, SLOT(startPlay()));
    connect(mBuffer, SIGNAL(bufferEmpty()), this, SLOT(stopPlay()));
    connect(mBuffer, SIGNAL(bufferSize(int)), this, SLOT(update()));
}

// -----------------------------------------------------------
void VideoPlayer::keyPressEvent(QKeyEvent *ke)
{
    if (ke->key() == Qt::Key_Right) {
        if (mState == Pause)
            setState(Playing);
        else if (mPlayFps < MAX_FPS) {
            mPlayFps *= 2;
            mTimer.setInterval(1000/mPlayFps);
            update();
        }
    }

    if (ke->key() == Qt::Key_Left && mPlayFps > MIN_FPS) {
        mPlayFps /= 2;
        mTimer.setInterval(1000/mPlayFps);
        update();
    }

     if (ke->key() == Qt::Key_Space) {
         switch (mState) {
         case Playing : setState(Pause); break;
         case Pause :
         case Ended : setState(Playing); break;
         }
     }
}

// -----------------------------------------------------------
void VideoPlayer::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    p.drawRect(rect());

    /*
    if (!mMovie || (mCurrentFrame < 0) || (mCurrentFrame >= mMovie->frameCount())) {
        if (mStartTime.elapsed() < INIT_ANIM_TIME) {
            int d = mStartTime.elapsed()/(float)INIT_ANIM_TIME * 255.0;
            p.setPen(QColor(255-d,255-d,255-d));
            p.drawText(rect(),Qt::AlignCenter,"Welcome back, director.");
        }
        return;
    }
    */

    if (!mCurrentFrame.isNull()) {
        QImage img = mCurrentFrame->img()->scaled(width()-10,height()-70,Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p.drawImage((width()-img.width())/2,(height()-img.height())/2,img);
    }

    switch (mState) {
    case Busy:
        p.setPen(Qt::white);
        p.drawText(rect(),Qt::AlignCenter,"Buffering");
        break;
    case Pause:
        p.setPen(Qt::white);
        p.drawText(rect(),Qt::AlignCenter,"Pause");
        break;
    case Ended:
        p.setPen(Qt::white);
        p.drawText(rect(),Qt::AlignCenter,"The End");
        break;
    }

    QString infoText = QString("frame:%1, fps:%2")
                            .arg(mBuffer->currentIndex())
                            .arg(mPlayFps);
    p.setPen(Qt::white);
    p.drawText(QRect(0,10,width(),height()),Qt::AlignTop | Qt::AlignHCenter, infoText);

    /*
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
    */
}

//-----------------------------------------------
void VideoPlayer::resizeEvent(QResizeEvent *re)
{
    if (mBuffer)
        mBuffer->setMaxWidth(re->size().width());
}

//-----------------------------------------------
void VideoPlayer::startPlay()
{
    if (mState == Busy)
        setState(Playing);
}

//-----------------------------------------------
void VideoPlayer::stopPlay()
{
    if (mState == Playing)
        setState(Busy);
}

//-----------------------------------------------
void VideoPlayer::bufferStateChanged(int state)
{
    update();
}

//-----------------------------------------------
void VideoPlayer::grabNextFrame()
{
    if (mState != Playing)
        return;

    Q_ASSERT(mBuffer);
    bool isAtEnd;
    mCurrentFrame = mBuffer->takeFrame(&isAtEnd);
    if (mCurrentFrame.isNull())
        setState(isAtEnd ? Ended : Busy);

    update();
}

//-----------------------------------------------
void VideoPlayer::setState(VideoPlayer::State st)
{
    if (mState == st)
        return;
    mState = st;

    switch(mState) {
      case Playing:
        if (!mTimer.isActive())
            mTimer.start();
        break;
      case Pause:
        if (mTimer.isActive())
            mTimer.stop();
        break;
    case Ended:
        if (mTimer.isActive())
            mTimer.stop();
        if (mBuffer)
            mBuffer->setCurrentIndex(0);
        break;
    }

    update();
}
