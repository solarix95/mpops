#ifndef TIMELINE_H
#define TIMELINE_H

#include <QWidget>
#include <QMouseEvent>
#include "videobuffer.h"

class Timeline : public QWidget
{
    Q_OBJECT
public:
    explicit Timeline(QWidget *parent = 0);
    
    void setBuffer(VideoBuffer *buffer);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);

private:
    VideoBuffer *mBuffer;
};

#endif // TIMELINE_H
