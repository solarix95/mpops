#include "cropjob.h"
#include <QDebug>

//---------------------------------------------------------------
CropJob::CropJob(ImagePtr img, const QRect &rect)
{
  mImg  = img;
  mRect = rect;
}

//---------------------------------------------------------------
void CropJob::run()
{
  mImg->lock();
  *(mImg->img()) = mImg->img()->copy(mRect);
  mImg->unlock();
}
