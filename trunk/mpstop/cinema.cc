#include <QPainter>
#include "cinema.h"
#include "movie.h"

Cinema::Cinema(QWidget *parent) :
    QWidget(parent)
{
    mIsPlaying    = false;
    mCurrentFrame = -1;
}

void Cinema::setMovie(Movie *movie)
{
    mMovie = movie;
    connect(mMovie, SIGNAL(frameChanged(int)), this, SLOT(frameChanged(int)));
}

void Cinema::setFrame(int index)
{
    if (!mIsPlaying) {
        mCurrentFrame = index;
        update();
    }
}

void Cinema::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setBrush(Qt::black);
    p.drawRect(rect());

    if (!mMovie || mCurrentFrame < 0)
        return;

    if (!mMovie->rendered(mCurrentFrame) || mMovie->rendered(mCurrentFrame)->isNull())
        return;
    QImage img = mMovie->rendered(mCurrentFrame)->scaled(width(),height(),Qt::KeepAspectRatio);
    p.drawImage(0,(height()-img.height())/2,img);
}


void Cinema::frameChanged(int frameIndex)
{
    if (frameIndex == mCurrentFrame)
        update();
}
