#ifndef CROPJOB_H
#define CROPJOB_H

#include <QImage>
#include "job.h"

class CropJob : public Job
{
public:
    CropJob(QImage *img, const QRect &rect);

    virtual void run();

private:
    QImage *mImg;
    QRect   mRect;
};

#endif // CROPJOB_H
