#include "cropjob.h"
#include <QDebug>

//---------------------------------------------------------------
CropJob::CropJob(QImage *img, const QRect &rect)
{
  mImg  = img;
  mRect = rect;
}

//---------------------------------------------------------------
void CropJob::run()
{
  *mImg = mImg->copy(mRect);
}
