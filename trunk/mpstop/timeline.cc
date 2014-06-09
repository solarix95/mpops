#include <QWheelEvent>
#include <QDebug>
#include "timeline.h"

TimeLine::TimeLine(QWidget *parent) :
    QGraphicsView(NULL,parent)
{
    qDebug() << "CREATED";
}

void TimeLine::wheelEvent(QWheelEvent *event)
{

    scale(event->delta() > 0 ?  1/1.1 : 1.1 ,1);
}
