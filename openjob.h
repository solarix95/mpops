#ifndef OPENJOB_H
#define OPENJOB_H

#include "job.h"
#include <QImage>

class OpenJob : public Job
{
public:
    OpenJob(QImage *img, const QString &fromFilename);

    virtual void run();

private:
    QImage *mImg;
    QString mFromFilename;
};

#endif // OPENJOB_H
