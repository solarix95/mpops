#include <QDebug>
#include <QTime>
#include <QFile>
#include "fileimage.h"


FileImage::FileImage(QImage *img, const QString &filename, int maxWidthHint)
    : Image(img,filename), mMaxWidthHint(maxWidthHint)
{
}

FileImage::~FileImage()
{
}

void FileImage::loadImage(QImage *img)
{
    Q_ASSERT(img);

    img->load(frameName());

    if (mMaxWidthHint > 0 && img->size().width() > mMaxWidthHint) {
        double scale = mMaxWidthHint/(double)img->size().width();
        *img = img->scaled(img->width()*scale,img->height()*scale,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
}
