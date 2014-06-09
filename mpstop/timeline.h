#ifndef TIMELINE_H
#define TIMELINE_H

#include <QGraphicsView>

class TimeLine : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TimeLine(QWidget *parent = 0);
    

signals:
    
public slots:
    
protected:
    virtual void wheelEvent(QWheelEvent *event);
};

#endif // TIMELINE_H
