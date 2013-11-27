#include "image.h"
#include <QDebug>

//---------------------------------------------------------------
Image::Image(QImage *img, const QString &frameName)
{
  Q_ASSERT(img);
  mImage = img;
  mFrameName = frameName;
  mAllJobsDone = false;
}

//---------------------------------------------------------------
Image::~Image()
{
  delete mImage;
}

bool Image::load()
{
    if (mImage->isNull())
        loadImage(mImage);
    return !mImage->isNull();
}

//---------------------------------------------------------------
void Image::waitForFinished()
{
  do {
    mMutex.lock();
    if (mAllJobsDone)
      return;
    mMutex.unlock();
    usleep(1000);
  } while (true);
}

//---------------------------------------------------------------
void Image::loadImage(QImage*)
{
}
