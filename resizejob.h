#ifndef RESIZEJOB_H
#define RESIZEJOB_H

#include <QImage>
#include "job.h"

class ResizeJob : public Job
{
public:
    ResizeJob(QImage *img, const QSize &size);

    virtual void run();

private:
    QImage *mImg;
    QSize   mSize;
};

#endif // RESIZEJOB_H
