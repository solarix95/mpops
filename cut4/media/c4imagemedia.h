#ifndef C4IMAGEMEDIA_H
#define C4IMAGEMEDIA_H

#include <QString>
#include <QImage>
#include "media/c4media.h"

class C4ImageMedia : public C4Media
{
public:
    C4ImageMedia(const QString &file);

    virtual int   width() const;
    virtual int   height() const;
    virtual QRgb  pixel(int x, int y) const;

private:
    QImage mImage;
};


#endif // C4IMAGEMEDIA_H
