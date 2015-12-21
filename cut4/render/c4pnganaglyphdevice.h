#ifndef C4PNGANAGLYPHDEVICE_H
#define C4PNGANAGLYPHDEVICE_H

#include "render/c4outdevice.h"

class C4PngAnaglyphDevice : public C4OutDevice
{
    Q_OBJECT
public:
    C4PngAnaglyphDevice();

    virtual bool isStereo() const;
    virtual void setAttribute(const QString &name, const QVariant &value);

public slots:
    virtual void   init();
    virtual QImage appendFrame(const C4FrameBuffer *left, const C4FrameBuffer *right);

private:
    QRgb mixer(const QRgb &right, const QRgb &left) const;

    QString mOutDir;
    int     mFrameCount;
};

#endif // C4PNGANAGLYPHDEVICE_H
