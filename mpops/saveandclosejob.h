#ifndef SAVEANDCLOSEJOB_H
#define SAVEANDCLOSEJOB_H

#include <QImage>
#include "job.h"

class SaveAndCloseJob : public Job
{
public:
    SaveAndCloseJob(ImagePtr img, const QString &toFileName);

    virtual void run();

private:
    ImagePtr  mImg;
    QString   mToFilename;
};

#endif // SAVEANDCLOSEJOB_H
