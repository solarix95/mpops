#ifndef OPENJOB_H
#define OPENJOB_H

#include <QImage>
#include "job.h"
#include "shared/image.h"

class OpenJob : public Job
{
public:
    OpenJob(ImagePtr img,const QPoint &colorPicker);

    virtual void run();

private:
    ImagePtr  mImg;
    QString   mFromFilename;
    QPoint    mColorPickerPos;
};

#endif // OPENJOB_H
