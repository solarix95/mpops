#include "blueboxjob.h"
#include <QColor>

//---------------------------------------------------------------
BlueboxJob::BlueboxJob(ImagePtr img, int hue, int tolerance)
{
  mImg         = img;
  mHue         = hue;
  mTolerance   = tolerance;
}

//---------------------------------------------------------------
void BlueboxJob::run()
{
  mImg->lock();
  QImage *img = mImg->img();

  if (img->isNull()) {
    mImg->unlock();
    return;
  }

  // QImage alphaMask(mImg->width(), mImg->height(), );
  if (img->format() != QImage::Format_ARGB32)
    *img = mImg->img()->convertToFormat(QImage::Format_ARGB32);
  Q_ASSERT(mImg->img()->hasAlphaChannel());

  for (int x=0; x<img->width(); x++) {
    for (int y=0; y<img->height(); y++) {
      QColor pixelColor = img->pixel(x,y);
      int distance = abs(pixelColor.hue() - mHue);
      if (distance <= mTolerance) {
        QColor newColor = img->pixel(x,y);
        newColor.setAlpha(255*(distance/(double)mTolerance));
        img->setPixel(x,y,newColor.rgba());
      }
    }
  }

  mImg->unlock();
}

