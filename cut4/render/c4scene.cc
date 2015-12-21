#include "math.h"
#include "render/c4scene.h"
#include "render/c4dummydevice.h"
#include "render/c4png2ddevice.h"
#include "render/c4pnganaglyphdevice.h"
#include "painting/simpleframebuffer.h"
#include "shape/c4mediashape.h"
#include "media/c4imagemedia.h"
#include "media/c4videomedia.h"
#include "painting/stereocamera.h"
#include "painting/painter.h"

//-----------------------------------------------------------------------------
C4Scene::C4Scene()
{
    mPrimaryBuffer  = new C4SimpleFrameBuffer(QSize(800,600));
    mSecondBuffer   = new C4SimpleFrameBuffer(QSize(800,600));
    mOutDevice      = NULL;
    // setOutDevice(new C4DummyDevice());
    setOutDevice(new C4PngAnaglyphDevice());

    mShapes << new C4MediaShape(new C4VideoMedia("black-background_600.jpg"));
    mShapes << new C4MediaShape(new C4VideoMedia("playmate_640.jpg"));
    // mShapes << new C4MediaShape(new C4ImageMedia("anaglyph.png"));

    mShapes.first()->setPos(0,0,700);
    mStereoEffect        = 1;
    mStereoEyeSeparation = 2;

    mStartFrame = 0;
    mEndFrame   = 50*24;

    applyLeftFrustum(mPrimaryBuffer);
    applyRightFrustum(mSecondBuffer);
}

//-----------------------------------------------------------------------------
void C4Scene::init(QSize size)
{
    qDeleteAll(mShapes);
    mShapes.clear();
    if (mPrimaryBuffer)
        delete mPrimaryBuffer;
    if (mSecondBuffer)
        delete mSecondBuffer;

    mPrimaryBuffer  = new C4SimpleFrameBuffer(size);
    mSecondBuffer   = new C4SimpleFrameBuffer(size);
    applyLeftFrustum(mPrimaryBuffer);
    applyRightFrustum(mSecondBuffer);
}

//-----------------------------------------------------------------------------
void C4Scene::render(int fromFrame, int toFrame)
{
    // emit renderingStarted();
    mOutDevice->init();
    for (int i=fromFrame; i<=toFrame; i++)
        singleShot(NULL,i);
    mOutDevice->save();
    // emit renderingEnded();
}

//-----------------------------------------------------------------------------
void C4Scene::singleShot(QImage *img, int frameIndex)
{
    if (mOutDevice->isStereo())
        singleShot3d(img,frameIndex);
    else
        singleShot2d(img,frameIndex);
}

//-----------------------------------------------------------------------------
C4Shape *C4Scene::createMediaShape(const QString &mediaName)
{
    mShapes << new C4MediaShape(new C4VideoMedia(mediaName));
    return mShapes.last();
}

//-----------------------------------------------------------------------------
C4Shape *C4Scene::shape(int index) const
{
    return mShapes[index];
}

//-----------------------------------------------------------------------------
C4Shape *C4Scene::shapeById(int id) const
{
    // TODO: make it faster :)
    for(int i=0; i<mShapes.count(); i++) {
        if (mShapes.at(i)->id() == id)
            return mShapes.at(i);
    }
    return NULL;
}

//-----------------------------------------------------------------------------
C4OutDevice *C4Scene::device() const
{
    return mOutDevice;
}

//-----------------------------------------------------------------------------
void C4Scene::setAttribut(C4Scene::Attribut a, const QVariant &v)
{
    switch (a) {
      case StereoEffect: mStereoEffect        = v.toFloat(); break;
      case StereoEyeSep: mStereoEyeSeparation = v.toFloat(); break;
    }
}

//-----------------------------------------------------------------------------
void C4Scene::applyLeftFrustum(C4FrameBuffer *buffer)
{
    float top, bottom, left, right;

    float nearClippingDistance = 75;
    float fov                  = 45*3.1415/180.0f;
    float aspect               = buffer->size().width()/(float)buffer->size().height();
    float convergence          = 120;
    float eyeSeparation        = convergence/30.0 * mStereoEffect;

    top     = nearClippingDistance * tan(fov/2);
    bottom  = -top;
    float a = aspect * tan(fov/2) * convergence;
    float b = a - eyeSeparation/2;
    float c = a + eyeSeparation/2;

    left    = -b * nearClippingDistance/convergence;
    right   =  c * nearClippingDistance/convergence;

    buffer->setFrustum(left, right, bottom, top, nearClippingDistance, 10000);
}

//-----------------------------------------------------------------------------
void C4Scene::setOutDevice(C4OutDevice *dev)
{
    if (mOutDevice)
        delete mOutDevice;
    mOutDevice = dev;
    Q_ASSERT(mOutDevice);
}

//-----------------------------------------------------------------------------
void C4Scene::singleShot2d(QImage *img, int frameIndex)
{
    emit renderingStarted();
    emit preRender(frameIndex);
    mPrimaryBuffer->reset();

    applyLeftFrustum(mPrimaryBuffer);

    C4Painter p(mPrimaryBuffer,new Stereocamera(0,0,120,mStereoEyeSeparation/2.0 * mStereoEffect));

    foreach(C4Shape *s, mShapes)
        s->render(&p,frameIndex);

    QImage result = mPrimaryBuffer->toImage();

    if (img)
        *img = result;
    emit rendered(frameIndex, result);
    emit renderingEnded();
}

//-----------------------------------------------------------------------------
void C4Scene::singleShot3d(QImage *img, int frameIndex)
{
    emit renderingStarted();
    emit preRender(frameIndex);
    mPrimaryBuffer->reset();
    mSecondBuffer->reset();

    // http://www.3dtv.at/knowhow/anaglyphcomparison_en.aspx
    // http://www.animesh.me/2011/05/rendering-3d-anaglyph-in-opengl.html
    // http://paulbourke.net/stereographics/stereorender/

    // parallel axis asymmetric frustum perspective projection

    applyLeftFrustum(mPrimaryBuffer);
    applyRightFrustum(mSecondBuffer);
    {
        C4Painter p(mPrimaryBuffer,new Stereocamera(0,0,120,mStereoEyeSeparation/2.0 * mStereoEffect));

        // p.setColorMatrix(1,0,0,0,0,0,0,0,0);             // Color Anaglyphs
        // p.setColorMatrix(0,0.7,0.3,0,0,0,0,0,0);         // Optimized Anaglyphs
        // p.setColorMatrix(0.299,0.587,0.114,0,0,0,0,0,0);
        // p.setColorMatrix(0.299,0.587,0.114,0,0,0,0,0,0);    // Half Color/Gray/True Anaglyphs

        foreach(C4Shape *s, mShapes)
            s->render(&p,frameIndex);
    }
    {
        C4Painter p(mSecondBuffer,new Stereocamera(0,0,120,-mStereoEyeSeparation/2.0 * mStereoEffect));

        // p.setColorMask(false,true,true);
        // p.setColorMatrix(0.0,0.0,0.0,0.299,0.587,0.114,0.299,0.587,0.114); // Gray Anaglyphs
        // p.setColorMatrix(0.0,0.0,0.0,0.0,0.0,0.0,0.299,0.587,0.114); // True Anaglyphs

        foreach(C4Shape *s, mShapes)
            s->render(&p,frameIndex);
    }

    QImage result = mOutDevice->appendFrame(mPrimaryBuffer,mSecondBuffer);

    if (img)
        *img = result;
    emit rendered(frameIndex, result);
    emit renderingEnded();
}

//-----------------------------------------------------------------------------
void C4Scene::applyRightFrustum(C4FrameBuffer *buffer)
{
    float top, bottom, left, right;

    float nearClippingDistance = 75;
    float fov                  = 45*3.1415/180.0f;
    float aspect               = buffer->size().width()/(float)buffer->size().height();
    float convergence          = 120;
    float eyeSeparation        = convergence/30.0 * mStereoEffect;

    top     = nearClippingDistance * tan(fov/2);
    bottom  = -top;
    float a = aspect * tan(fov/2) * convergence;
    float b = a - eyeSeparation/2;
    float c = a + eyeSeparation/2;

    left    = -c * nearClippingDistance/convergence;
    right   =  b * nearClippingDistance/convergence;

    buffer->setFrustum(left, right, bottom, top, nearClippingDistance, 10000);
}
