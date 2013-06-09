#ifndef BLUEBOXJOB_H
#define BLUEBOXJOB_H

#include <QImage>
#include "job.h"

class BlueboxJob : public Job
{
  public:
    BlueboxJob(QImage *img, int hue, int tolerance);

    virtual void run();

  private:
    QImage *mImg;
    int     mHue;
    int     mTolerance;
};

#endif // BLUEBOXJOB_H