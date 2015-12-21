#ifndef C4PNG2DDEVICE_H
#define C4PNG2DDEVICE_H

#include "c4outdevice.h"

//-----------------------------------------------------------------------------
class C4Png2dDevice : public C4OutDevice
{
public:
    C4Png2dDevice();

    virtual bool isStereo() const;

public slots:
    virtual void appendFrame(const QImage &left, const QImage &right = QImage());

};

#endif // C4PNG2DDEVICE_H
