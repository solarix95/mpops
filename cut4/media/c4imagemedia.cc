#include <QDebug>
#include "media/c4imagemedia.h"

//-----------------------------------------------------------------------------
C4ImageMedia::C4ImageMedia(const QString &file)
    : C4Media()
{
    if (!mImage.load(file))
        qWarning() << "C4ImageMedia::C4ImageMedia: invalid file:" << file;
}

//-----------------------------------------------------------------------------
int C4ImageMedia::width() const
{
    return mImage.width();
}

//-----------------------------------------------------------------------------
int C4ImageMedia::height() const
{
    return mImage.height();
}

//-----------------------------------------------------------------------------
QRgb C4ImageMedia::pixel(int x, int y) const
{
    return mImage.pixel(x,y);
}
