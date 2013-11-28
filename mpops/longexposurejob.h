#ifndef LONGEXPOSUREJOB_H
#define LONGEXPOSUREJOB_H

#include <QImage>
#include "job.h"
#include "shared/image.h"

class LongExposureJob : public Job
{
public:
    LongExposureJob(ImagePtrs inImgs, ImagePtr outImg, const QString &fname
                    , int maxFrames);

    virtual void run();

private:
    QString    mFileName;
    ImagePtr   mOutImg;
    ImagePtrs  mInImgs;
    int        mMaxFrames;
};

#endif // CROPJOB_H
