#ifndef C4DUMMYDEVICE_H
#define C4DUMMYDEVICE_H

#include "c4outdevice.h"

class C4DummyDevice : public C4OutDevice
{
public:
    C4DummyDevice();

     virtual bool isStereo() const;

public slots:
    virtual QImage appendFrame(const C4FrameBuffer *left, const C4FrameBuffer *right = NULL);
};

#endif // C4DUMMYDEVICE_H
