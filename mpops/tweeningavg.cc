#include "tweeningavg.h"
#include <QDebug>

TweeningAvg::TweeningAvg(ImagePtr first, ImagePtr second, ImagePtr output)
{
  mImgFirst  = first;
  mImgSecond = second;
  mImgOutput = output;
}

void TweeningAvg::run()
{
  mImgFirst->waitForFinished();
  mImgSecond->waitForFinished();

  if (mImgFirst->img()->width() != mImgSecond->img()->width() ||
      mImgFirst->img()->height() != mImgSecond->img()->height())
  {
    mImgFirst->unlock();
    mImgSecond->unlock();
    return;
  }

  *(mImgOutput->img()) = QImage(mImgFirst->img()->width(),mImgFirst->img()->height(),QImage::Format_RGB32);


  for (int x=0; x<mImgOutput->img()->width(); x++) {
    for (int y=0; y<mImgOutput->img()->height(); y++) {
      QRgb fCol = mImgFirst->img()->pixel(x,y);
      QRgb sCol = mImgSecond->img()->pixel(x,y);
      mImgOutput->img()->setPixel(x,y,qRgb((qRed(fCol)+qRed(sCol))/2,(qGreen(fCol)+qGreen(sCol))/2,(qBlue(fCol)+qBlue(sCol))/2));
    }
  }

  mImgFirst->unlock();
  mImgSecond->unlock();

}
