#include "fileimage.h"


FileImage::FileImage(QImage *img, const QString &filename)
    : Image(img,filename)
{
}

FileImage::~FileImage()
{
}

void FileImage::loadImage(QImage *img)
{
    Q_ASSERT(img);
    img->load(frameName());
}
