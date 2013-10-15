#include "image.h"
#include <QDebug>

Image::Image(QImage *img)
{
  Q_ASSERT(img);
  mImage = img;
  mAllJobsDone = false;
}


Image::~Image()
{
  delete mImage;
}


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
