#include "longexposurejob.h"
#include <QDebug>

// ----------------------------------------------------------------
typedef struct sFloatRGB
{
  float r;
  float g;
  float b;
  sFloatRGB(): r(0.0), g(0.0), b(0.0) {}
} FloatRGB;

//---------------------------------------------------------------
LongExposureJob::LongExposureJob(ImagePtrs inImgs, ImagePtr outImg, 
                                 const QString &fname, int maxFrames)
{
  mFileName  = fname;
  mInImgs    = inImgs;
  mOutImg    = outImg;
  mMaxFrames = maxFrames;
  Q_ASSERT(mMaxFrames > 0);
}

//---------------------------------------------------------------
void LongExposureJob::run()
{
  Q_ASSERT(mInImgs.count() > 0);

  foreach(ImagePtr nextFrame, mInImgs)
    nextFrame->waitForFinished();
  mOutImg->lock();

  int width  = mInImgs.first()->img()->width();
  int height = mInImgs.first()->img()->height();
  Q_ASSERT((width*height) > 0);

  *(mOutImg->img()) = QImage(width,height,QImage::Format_RGB32);

  int frameCount = qMin(mInImgs.count(),mMaxFrames);

  FloatRGB *pixmap = new FloatRGB[width*height];
  float     factor = 1/(float)frameCount;

  int currentFrame = 1;
  foreach(ImagePtr nextFrame, mInImgs) {
    for (int x=0; x<mOutImg->img()->width(); x++) {
      for (int y=0; y<mOutImg->img()->height(); y++) {
        QRgb col = nextFrame->img()->pixel(x,y);
        pixmap[y*width + x].r += factor * qRed(col);
        pixmap[y*width + x].g += factor * qGreen(col);
        pixmap[y*width + x].b += factor * qBlue(col);
      }
    }
    if (currentFrame >= frameCount)
      break;
    currentFrame++;
  }

  for (int x=0; x<mOutImg->img()->width(); x++) {
    for (int y=0; y<mOutImg->img()->height(); y++) {
      mOutImg->img()->setPixel(x,y,qRgb(pixmap[y*width+x].r, pixmap[y*width + x].g, pixmap[y*width + x].b));
    }
  }

  delete [] pixmap;
  foreach(ImagePtr nextFrame, mInImgs)
    nextFrame->unlock();

  mOutImg->img()->save(mFileName); 
  mOutImg->unlock();
}
