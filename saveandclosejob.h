#ifndef SAVEANDCLOSEJOB_H
#define SAVEANDCLOSEJOB_H

#include <QImage>
#include "job.h"

class SaveAndCloseJob : public Job
{
public:
    SaveAndCloseJob(QImage *img, const QString &toFileName);

    virtual void run();

private:
    QImage *mImg;
    QString mToFilename;
};

#endif // SAVEANDCLOSEJOB_H
