#ifndef OPENCVIMAGE_H
#define OPENCVIMAGE_H

#include <QString>
#include "shared/image.h"

#include <opencv2/opencv.hpp>

class OpencvImage : public Image
{
public:
    OpencvImage(QImage *img, IplImage *opencvImage, const QString &sourceName, int maxWidthHint = -1);
    ~OpencvImage();

protected:
    virtual void loadImage(QImage *img);

private:
    int       mMaxWidthHint;
    IplImage *mFrame;
};

#endif // OPENCVIMAGE_H
