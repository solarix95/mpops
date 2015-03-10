#include <QDebug>
#include <QTimer>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
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
void MovieScene::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        Q_ASSERT(mSelections);
        QList<int> sels = mSelections->selections();
        while (sels.count() > 0) {
            mMovie->removeFrame(sels.takeLast());
        }
    }
    QGraphicsScene::keyPressEvent(event);
}

// -----------------------------------------------------------
void MovieScene::createMovieThumbnail(int /* index */)
{
    mThumbs     << ThumbnailPtr(new Thumbnail(mMovie,mThumbs.count(),mSelections));
    addItem(mThumbs.last().data());
    connect(mThumbs.last().data(), SIGNAL(freezeFrameRequest(int)), mMovie, SLOT(addFreezeFrame(int)));
    connect(mThumbs.last().data(), SIGNAL(deleteFrameRequest(int)), mMovie, SLOT(removeFrame(int)));
}

// -----------------------------------------------------------
void MovieScene::removeMovieThumbnail(int /*index*/)
{
    removeItem(mThumbs.takeLast().data());
}

