#include "resizejob.h"
#include <QDebug>

//---------------------------------------------------------------
ResizeJob::ResizeJob(QImage *img, const QSize &size)
{
  mImg  = img;
  mSize = size;
}

//---------------------------------------------------------------
void ResizeJob::run()
{
  if (mImg->size() == mSize)
    return;

  *mImg = mImg->scaled(mSize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
}
