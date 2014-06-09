#include <QDebug>
#include "moviescene.h"
#include "movie.h"

MovieScene::MovieScene(QObject *parent, Movie *movie, SelectionModel *selections)
    :  QGraphicsScene(parent)
{
    mMovie      = movie;
    mSelections = selections;
    connect(mMovie, SIGNAL(frameAppended(int)), this, SLOT(createMovieThumbnail(int)));
}

void MovieScene::createMovieThumbnail(int index)
{
    qDebug() << index;
    mThumbs << ThumbnailPtr(new Thumbnail(mMovie,index,mSelections));
    addItem(mThumbs.last().data());


    // setSceneRect(-100,0,mThumbs.count() * Movie::thumbSize().width(),Movie::thumbSize().height());
}
