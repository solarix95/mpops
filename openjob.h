#ifndef OPENJOB_H
#define OPENJOB_H

#include "job.h"
#include <QImage>

class OpenJob : public Job
{
public:
    OpenJob(ImagePtr img, const QString &fromFilename, const QPoint &colorPicker);

    virtual void run();

private:
    ImagePtr  mImg;
    QString   mFromFilename;
    QPoint    mColorPickerPos;
};

#endif // OPENJOB_H
