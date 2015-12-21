#include "render/c4png2ddevice.h"

//-----------------------------------------------------------------------------
C4Png2dDevice::C4Png2dDevice()
{
}

//-----------------------------------------------------------------------------
bool C4Png2dDevice::isStereo() const
{
    return false;
}

//-----------------------------------------------------------------------------
void C4Png2dDevice::appendFrame(const QImage &left, const QImage &right)
{
    Q_ASSERT(right.isNull());
    Q_ASSERT(!left.isNull());

}
