#include <QVariant>
#include <QDebug>
#include "painting/framebuffer.h"
#include "render/c4pnganaglyphdevice.h"

//-----------------------------------------------------------------------------
C4PngAnaglyphDevice::C4PngAnaglyphDevice()
{
    mFrameCount = 0;
}

//-----------------------------------------------------------------------------
bool C4PngAnaglyphDevice::isStereo() const
{
    return true;
}

//-----------------------------------------------------------------------------
void C4PngAnaglyphDevice::setAttribute(const QString &name, const QVariant &value)
{
    if (name.toLower() == "path")
        mOutDir = value.toString();
    else
        qWarning() << "C4PngAnaglyphDevice::setAttribute: invalid:" << name;
}

//-----------------------------------------------------------------------------
void C4PngAnaglyphDevice::init()
{
    mFrameCount = 0;
}

//-----------------------------------------------------------------------------
QImage C4PngAnaglyphDevice::appendFrame(const C4FrameBuffer *leftFrame, const C4FrameBuffer *rightFrame)
{
    Q_ASSERT(leftFrame);
    Q_ASSERT(leftFrame->size() == rightFrame->size());

    QImage out(leftFrame->size(),QImage::Format_ARGB32);

    int w = leftFrame->size().width();
    int h = leftFrame->size().height();

    for (int y = 0; y<h; y++) {
        for (int x=0; x<w; x++) {
            QRgb left  = leftFrame->pixel(x,y).rgb();
            QRgb right = rightFrame->pixel(x,y).rgb();
            out.setPixel(x,y,mixer(left,right));
        }
    }
    emit playerFrame(out);

    if (!mOutDir.isEmpty())
        out.save(QString("%1/%2_%3.png").arg(mOutDir).arg("frame").arg(mFrameCount));

    mFrameCount++;
    return out;
}

//-----------------------------------------------------------------------------
QRgb C4PngAnaglyphDevice::mixer(const QRgb &r, const QRgb &l) const
{
    return qRgb(0.299*qRed(r) + 0.587*qGreen(r) + 0.114*qBlue(r),
                qGreen(l),
                qBlue(l));
}
