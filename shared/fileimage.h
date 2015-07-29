#ifndef FILEIMAGE_H
#define FILEIMAGE_H

#include <QString>
#include "shared/image.h"

class FileImage : public Image
{
public:
    FileImage(QImage *img, const QString &filename, int maxWidthHint = -1);
    ~FileImage();

protected:
    virtual void loadImage(QImage *img);

private:
    int mMaxWidthHint;
};

#endif // FILEIMAGE_H
