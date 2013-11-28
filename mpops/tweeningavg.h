#ifndef TWEENINGAVG_H
#define TWEENINGAVG_H

#include <QImage>
#include "job.h"
#include "shared/image.h"

class TweeningAvg : public Job
{
public:
    TweeningAvg(ImagePtr first, ImagePtr second, ImagePtr output);

    virtual void run();

private:
    ImagePtr mImgFirst;
    ImagePtr mImgSecond;
    ImagePtr mImgOutput;
};

#endif // TWEENINGAVG_H
