#include <QDebug>
#include <QThread>
#include "imageloaderjob.h"

//-----------------------------------------------------------------------------
ImageLoaderJob::ImageLoaderJob(int index, ImagePtr img) :
    QObject()
{
    mIndex = index;
    mImage = img;
    Q_ASSERT(mIndex >= 0);
    Q_ASSERT(!mImage.isNull());
    connect(this, SIGNAL(startJob()), this, SLOT(loadImage()), Qt::QueuedConnection);
}

//-----------------------------------------------------------------------------
ImageLoaderJob::~ImageLoaderJob()
{
    // qDebug() << "ImageLoaderJob::~ImageLoaderJob()"; // SW-Test only
}

//-----------------------------------------------------------------------------
void ImageLoaderJob::start()
{
    emit startJob();
}

//-----------------------------------------------------------------------------
void ImageLoaderJob::loadImage()
{
    // qDebug() << "ImageLoaderJob::loadImage()" << QThread::currentThread();
    mImage->load();
    emit loaded(mIndex,mImage);
    deleteLater();
}
