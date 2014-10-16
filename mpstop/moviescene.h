#ifndef MOVIESCENE_H
#define MOVIESCENE_H

#include <QGraphicsScene>
#include <QList>
#include "thumbnail.h"
#include "selectionmodel.h"
/* ---------------------------------------------------------*\
              MovieScene: Thumbnail-Management
\* ---------------------------------------------------------*/
class Movie;
class MovieScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit MovieScene(QObject *parent, Movie *movie, SelectionModel *selections);

private slots:
    void createMovieThumbnail(int index);
    void removeMovieThumbnail(int index);

private:
    Movie               *mMovie;
    SelectionModel      *mSelections;
    QList<ThumbnailPtr>  mThumbs;
};

#endif // MOVIESCENE_H
