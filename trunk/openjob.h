#ifndef OPENJOB_H
#define OPENJOB_H

#include "job.h"
#include <QImage>

class OpenJob : public Job
{
public:
    OpenJob(QImage *img, const QString &fromFilename, const QPoint &colorPicker);

    virtual void run();

private:
    QImage *mImg;
    QString mFromFilename;
    QPoint  mColorPickerPos;
};

#endif // OPENJOB_H
