#include "openjob.h"
#include <QColor>
#include <QString>
#include <iostream>
//---------------------------------------------------------------
OpenJob::OpenJob(QImage *img, const QString &fromFilename, const QPoint &colorPicker)
{
  mImg            = img;
  mFromFilename   = fromFilename;
  mColorPickerPos = colorPicker;
}

//---------------------------------------------------------------
void OpenJob::run()
{
  mImg->load(mFromFilename);

  // Do the Color-Picker-Job:
  if (!mImg->isNull() && (!mColorPickerPos.isNull())) {
    QColor pixelColor = mImg->pixel(mColorPickerPos);
    std::cout << "Color Picker: " << mFromFilename.toAscii().data() << std::endl;
    std::cout << " R-G-B-A: " <<  pixelColor.red()   << "-" <<
                                  pixelColor.green() << "-" <<
                                  pixelColor.blue()  << "-" <<
                                  pixelColor.alpha() << std::endl;
    std::cout << " H-S-V  : " <<  pixelColor.hue()   << "-" <<
                                  pixelColor.value() << "-" <<
                                  pixelColor.saturation() << std::endl << std::endl;
  }
}
