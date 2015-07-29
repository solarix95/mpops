#include <QPainter>
#include "timeline.h"

//-----------------------------------------------------------------------------
Timeline::Timeline(QWidget *parent) :
    QWidget(parent)
{
    mBuffer = NULL;
}

//-----------------------------------------------------------------------------
void Timeline::setBuffer(VideoBuffer *buffer)
{
    mBuffer = buffer;
    if (mBuffer) {
        connect(mBuffer, SIGNAL(bufferSize(int)), this, SLOT(update()));
        connect(mBuffer, SIGNAL(requesting()), this, SLOT(update()));
    }

    update();
}

//-----------------------------------------------------------------------------
void Timeline::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setPen(Qt::black);
    p.setBrush(QColor(230,230,230));
    p.drawRect(rect());

    if (mBuffer && mBuffer->size() > 0) {
        double frameLength = width()/(double)mBuffer->sourceLength();
        int    frameWidth  = frameLength < 0 ? 1 : (int)frameLength;
        int    currentPos  = frameLength*mBuffer->currentIndex();

        p.setPen(Qt::black);
        p.setBrush(Qt::lightGray);
        QRect bufferRect(currentPos,0,frameLength*mBuffer->size(),height()-1);
        p.drawRect(bufferRect);

        QString infoText = QString("%1MB")
                                .arg(mBuffer->memoryUsage()/1024/1024);
        p.setPen(Qt::white);
        p.drawText(bufferRect,Qt::AlignCenter, infoText);

        p.setPen(Qt::blue);
        p.setBrush(Qt::blue);
        p.drawRect(QRect(currentPos,1,4,height()-2));

    }
}

//-----------------------------------------------------------------------------
void Timeline::mousePressEvent(QMouseEvent *me)
{
    if (mBuffer) {
        int nextFrame = mBuffer->sourceLength() * me->pos().x()/(double)width();
        mBuffer->setCurrentIndex(nextFrame);
    }
}
