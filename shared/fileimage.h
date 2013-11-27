#ifndef FILEIMAGE_H
#define FILEIMAGE_H

#include <QString>
#include "shared/image.h"

class FileImage : public Image
{
public:
    FileImage(QImage *img, const QString &filename);
    ~FileImage();

protected:
    virtual void loadImage(QImage *img);
};

#endif // FILEIMAGE_H
