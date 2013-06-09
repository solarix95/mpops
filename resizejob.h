#ifndef RESIZEJOB_H
#define RESIZEJOB_H

#include <QImage>
#include "job.h"

class ResizeJob : public Job
{
public:
    ResizeJob(ImagePtr img, const QSize &size);

    virtual void run();

private:
    ImagePtr  mImg;
    QSize     mSize;
};

#endif // RESIZEJOB_H
