#include "himage.h"
#include <QColor>
#include <QDebug>

//---------------------------------------------------------------
HImage::HImage()
{
  mHeight   = 0;
  mWidth    = 0;
  mValues   = 0;
}

//---------------------------------------------------------------
HImage::HImage(const QImage &img)
{
  mHeight = img.height();
  mWidth  = img.width();

  Q_ASSERT(mHeight*mWidth > 0);
  mValues = new float[mHeight*mWidth];

  for (int y=0; y<mHeight; y++) {
    for (int x=0; x<mWidth; x++) {
      mValues[y*mWidth + x] = QColor(img.pixel(x,y)).hueF();
    }
  }
}

//---------------------------------------------------------------
HImage::HImage(const HImage &other)
{
  mHeight   = 0;
  mWidth    = 0;
  mValues   = 0;
  copy(other);
}

//---------------------------------------------------------------
HImage::~HImage()
{
  if (mValues)
    delete [] mValues;
}

//---------------------------------------------------------------
HImage & HImage::operator= (const HImage &other)
{
  if (mValues)
    delete [] mValues;

  copy(other);

  return *this;
}

//---------------------------------------------------------------
float HImage::hueAt(int x,int y) const
{
  return mValues[y*mWidth + x];
}

//---------------------------------------------------------------
void HImage::copy(const HImage &other)
{
  mValues = 0;
  mHeight = other.mHeight;
  mWidth  = other.mWidth;

  if (other.mHeight*other.mWidth == 0)
    return;

  mValues = new float[mHeight*mWidth];
  memcpy(mValues,other.mValues,mHeight*mWidth*sizeof(float));
}



