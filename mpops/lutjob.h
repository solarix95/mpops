#ifndef LUTJOB_H
#define LUTJOB_H

#include <QImage>
#include "job.h"
#include "shared/image.h"

class LutCorrection;

class LutJob : public Job
{
public:
    LutJob(ImagePtr img,LutCorrection *lut);

    virtual void run();

private:
    ImagePtr        mImg;
    LutCorrection  *mLut;
};
#endif // LUTJOB_H
