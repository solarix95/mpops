#include "openjob.h"
#include <QColor>
#include <QString>
#include <iostream>

//---------------------------------------------------------------
OpenJob::OpenJob(ImagePtr img, const QPoint &colorPicker)
{
  mImg            = img;
  mColorPickerPos = colorPicker;
}

//---------------------------------------------------------------
void OpenJob::run()
{
  mImg->lock();
  mImg->load();

  // Do the Color-Picker-Job:
  if (!mImg->img()->isNull() && (!mColorPickerPos.isNull())) {
    QColor pixelColor = mImg->img()->pixel(mColorPickerPos);
    std::cout << "Color Picker: " << mFromFilename.toAscii().data() << std::endl;
    std::cout << " R-G-B-A: " <<  pixelColor.red()   << "-" <<
                                  pixelColor.green() << "-" <<
                                  pixelColor.blue()  << "-" <<
                                  pixelColor.alpha() << std::endl;
    std::cout << " H-S-V  : " <<  pixelColor.hue()   << "-" <<
                                  pixelColor.value() << "-" <<
                                  pixelColor.saturation() << std::endl << std::endl;
  }

  mImg->unlock();
}
