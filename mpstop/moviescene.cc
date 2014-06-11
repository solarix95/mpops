#include <QDebug>
#include <QTimer>
#include "moviescene.h"
#include "movie.h"

// -----------------------------------------------------------
MovieScene::MovieScene(QObject *parent, Movie *movie, SelectionModel *selections)
    :  QGraphicsScene(parent)
{
    mMovie      = movie;
    mSelections = selections;
    connect(mMovie, SIGNAL(frameAppended(int)), this, SLOT(createMovieThumbnail(int)));
    connect(mMovie, SIGNAL(frameInserted(int)), this, SLOT(createMovieThumbnail(int)));
    connect(mMovie, SIGNAL(frameDeleted(int)), this, SLOT(removeMovieThumbnail(int)));
}

// -----------------------------------------------------------
void MovieScene::createMovieThumbnail(int index)
{
    mThumbs     << ThumbnailPtr(new Thumbnail(mMovie,mThumbs.count(),mSelections));
    addItem(mThumbs.last().data());
    connect(mThumbs.last().data(), SIGNAL(freezeFrameRequest(int)), mMovie, SLOT(addFreezeFrame(int)));
    connect(mThumbs.last().data(), SIGNAL(deleteFrameRequest(int)), mMovie, SLOT(removeFrame(int)));
}

// -----------------------------------------------------------
void MovieScene::removeMovieThumbnail(int index)
{
    mThumbs.takeLast();
}
