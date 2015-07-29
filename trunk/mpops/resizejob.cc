#include "resizejob.h"
#include <QDebug>

//---------------------------------------------------------------
ResizeJob::ResizeJob(ImagePtr img, const QSize &size)
{
  mImg  = img;
  mSize = size;
}

//---------------------------------------------------------------
void ResizeJob::run()
{
  mImg->lock();

  /*
  if (mImg->img()->size() != mSize)
    *(mImg->img()) = mImg->img()->scaled(mSize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

  */
  mImg->unlock();
}
