#include "blueboxjob.h"
#include <QColor>

//---------------------------------------------------------------
BlueboxJob::BlueboxJob(QImage *img, int hue, int tolerance)
{
  mImg         = img;
  mHue         = hue;
  mTolerance   = tolerance;
}

//---------------------------------------------------------------
void BlueboxJob::run()
{
  if (mImg->isNull()) {
    return;
  }

  // QImage alphaMask(mImg->width(), mImg->height(), );
  if (mImg->format() != QImage::Format_ARGB32)
    *mImg = mImg->convertToFormat(QImage::Format_ARGB32);
  Q_ASSERT(mImg->hasAlphaChannel());

  for (int x=0; x<mImg->width(); x++) {
    for (int y=0; y<mImg->height(); y++) {
      QColor pixelColor = mImg->pixel(x,y);
      int distance = abs(pixelColor.hue() - mHue);
      if (distance <= mTolerance) {
        QColor newColor = mImg->pixel(x,y);
        newColor.setAlpha(255*(distance/(double)mTolerance));
        mImg->setPixel(x,y,newColor.rgba());
      }
    }
  }


}

