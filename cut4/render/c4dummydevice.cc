#include "painting/framebuffer.h"
#include "render/c4dummydevice.h"

//-----------------------------------------------------------------------------
C4DummyDevice::C4DummyDevice()
{
}

//-----------------------------------------------------------------------------
bool C4DummyDevice::isStereo() const
{
    return false;
}

//-----------------------------------------------------------------------------
QImage C4DummyDevice::appendFrame(const C4FrameBuffer *left, const C4FrameBuffer * /*right*/)
{
    Q_ASSERT(left);
    QImage ret = left->toImage();
    emit playerFrame(left->toImage());
    return ret;
}
