#include "opencvimage.h"
#include <QDebug>

//-----------------------------------------------------------------------------
OpencvImage::OpencvImage(QImage *img, IplImage *opencvImage, const QString &sourceName, int maxWidthHint)
    : Image(img,sourceName)
{
    mFrame = opencvImage;
    mMaxWidthHint= maxWidthHint;
    Q_ASSERT(mMaxWidthHint);
    Q_ASSERT(mFrame);
}

//-----------------------------------------------------------------------------
OpencvImage::~OpencvImage()
{
    if (mFrame)
        cvReleaseImage(&mFrame);
}

//-----------------------------------------------------------------------------
void OpencvImage::loadImage(QImage *img)
{
    Q_ASSERT(mFrame);
    Q_ASSERT(img);

    if (mFrame->nChannels != 3) {
        qWarning() << "OpencvImage::loadImage: unsupported video format (channels)";
        return;
    }
    if (mFrame->depth != IPL_DEPTH_8U) {
        qWarning() << "OpencvImage::loadImage: unsupported video format (color depth)";
        return;
    }

    int nc   = mFrame->nChannels;
    int wstep= mFrame->widthStep;

    unsigned char *data= reinterpret_cast<unsigned char *>
            (mFrame->imageData);

    // Step: don't return Images in a higher resolution than requested..
    int step = mFrame->width / mMaxWidthHint;
    if (step <= 0)
        step = 1;
    else
        step -= step % 2;

    if (!step || (mFrame->width % step) || (mFrame->height % step))
        step = 1;

    *img = QImage(mFrame->width/step,mFrame->height/step,QImage::Format_RGB32);

    for (int y=0; y<mFrame->height; y += step) {
        Q_ASSERT(y < mFrame->height);
        QRgb *rawRow = (QRgb*)img->scanLine(y/step);
        for (int x=0; x<mFrame->width; x += step) {
            Q_ASSERT(x < mFrame->width);
            int firstIndex = y*wstep + x*nc;
            // BGR, not RGB:
            uchar blue  = data[firstIndex + 0];
            uchar green = data[firstIndex + 1];
            uchar red   = data[firstIndex + 2];
            // Slow:
            // img->img()->setPixel(x,y,qRgb(red,green,blue));
            rawRow[x/step] = qRgb(red,green,blue);
        }
    }

    cvReleaseImage(&mFrame);
    mFrame = NULL;

}
