#ifndef CLOSEJOB_H
#define CLOSEJOB_H

#include <QImage>
#include "job.h"
#include "shared/image.h"

class CloseJob : public Job
{
public:
    CloseJob(ImagePtr img);

    virtual void run();

private:
    ImagePtr  mImg;
};

#endif // CLOSEJOB_H
