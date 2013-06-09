#ifndef CROPJOB_H
#define CROPJOB_H

#include <QImage>
#include "job.h"

class CropJob : public Job
{
public:
    CropJob(ImagePtr img, const QRect &rect);

    virtual void run();

private:
    ImagePtr  mImg;
    QRect     mRect;
};

#endif // CROPJOB_H
