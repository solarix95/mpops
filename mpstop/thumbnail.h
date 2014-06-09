#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsItem>

class Movie;
class SelectionModel;

class Thumbnail : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit Thumbnail(Movie *movie, int frameIndex, SelectionModel *selections);

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);



protected:
    virtual void	mousePressEvent ( QGraphicsSceneMouseEvent * event );

private slots:
    void redraw(int index = -1);

private:
    void paintThumb(QPainter *p);

    Movie            *mMovie;
    SelectionModel   *mSelections;
    int               mId;
};

typedef QSharedPointer<Thumbnail> ThumbnailPtr;

#endif // THUMBNAIL_H
