#include "lutjob.h"
#include "lut/lutcorrection.h"

//---------------------------------------------------------------
LutJob::LutJob(ImagePtr img,LutCorrection *lut)
    : mImg(img), mLut(lut)
{
    Q_ASSERT(mLut);
}

//---------------------------------------------------------------
void LutJob::run()
{
    mImg->lock();

    mLut->correct(*mImg->img());

    mImg->unlock();
}
