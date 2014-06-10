#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QAction>

#include "thumbnail.h"
#include "movie.h"
#include "selectionmodel.h"

/* ---------------------------------------------------------*\
                Thumbnail: Single-Frame-View
\* ---------------------------------------------------------*/
Thumbnail::Thumbnail(Movie *movie, int frameIndex, SelectionModel *selections) :
    QGraphicsItem()
{
    Q_ASSERT(movie);

    mMovie      = movie;
    mSelections = selections;
    mId         = frameIndex;
    connect(mSelections,SIGNAL(changed(int)), this, SLOT(redraw(int)));
    connect(mMovie,SIGNAL(frameChanged(int)), this, SLOT(redraw(int)));
    connect(mMovie, SIGNAL(frameDeleted(int)), this, SLOT(redraw()));
}

// -----------------------------------------------------------
QRectF Thumbnail::boundingRect() const
{
    QSize mySize = Movie::thumbSize();
    return QRect(mId * (10 + mySize.width()),0,mySize.width()+1,mySize.height());
}

// -----------------------------------------------------------
void Thumbnail::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_ASSERT(painter);
    painter->save();
    QRectF r = boundingRect();
    painter->translate(r.x(),r.y());
    paintThumb(painter);

    switch(mMovie->type(mId)) {
    case Movie::FreezeFrame: {
        painter->setBrush(Qt::red);
        painter->drawEllipse(10,10,7,7);
    } break;
    }

    painter->restore();
}

// -----------------------------------------------------------
void Thumbnail::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_ASSERT(mSelections);
    mSelections->select(mId, !(event->modifiers() & Qt::ControlModifier));
}

// -----------------------------------------------------------
void Thumbnail::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *removeAction  = menu.addAction("Remove");
    QAction *freezeAction  = menu.addAction("FreezeFrame");
    // QAction *freezesAction = menu.addAction("FreezeFrames");
    mSelections->select(mId,true);
    QAction *selectedAction = menu.exec(event->screenPos());

    if (selectedAction == freezeAction)
        emit freezeFrameRequest(mId);
    if (selectedAction == removeAction)
        emit deleteFrameRequest(mId);
}

// -----------------------------------------------------------
void Thumbnail::redraw(int index)
{
    if (index < 0 || index == mId)
        update();
}

// -----------------------------------------------------------
void Thumbnail::paintThumb(QPainter *painter)
{
    Q_ASSERT(mSelections);


    QRectF r = boundingRect();
    QImage *thumb = mMovie->thumbNail(mId);

    if (!thumb) {
        painter->drawLine(0,0,r.width(),r.height());
        painter->drawLine(r.width(),0,0,r.height());
        painter->drawText(QRect(0,0,r.width(),r.height()),Qt::AlignCenter,QString::number(mId));
    } else {
        painter->drawImage(1,(r.height()-thumb->height())/2,*thumb);
    }

    painter->setPen(QPen(mSelections->isSelected(mId) ? Qt::blue : Qt::black,
                         mSelections->isSelected(mId) ? 2 : 1));
    painter->drawRect(0,0,r.width(),r.height());
}

