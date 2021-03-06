#ifndef C4SCENE_H
#define C4SCENE_H

#include <QList>
#include <QVariant>
#include <QImage>

class C4FrameBuffer;
class C4Shape;
class C4OutDevice;

class C4Scene : public QObject
{
    Q_OBJECT

public:
    C4Scene();

    void init(QSize size);

    void render(int fromFrame, int toFrame);
    void singleShot(QImage *img = NULL, int frameIndex = -1);

    C4Shape     *createMediaShape(const QString &mediaName);
    C4Shape     *shape(int index) const;
    C4Shape     *shapeById(int id) const;
    C4OutDevice *device() const;

    enum Attribut {
        StereoEffect = 1,
        StereoEyeSep = 2
    };

    void setAttribut(Attribut a, const QVariant &v);

signals:
    void renderingStarted();
    void preRender(int frameIndex);
    void rendered(int frameIndex, QImage &img);
    void postRender();
    void renderingEnded();

private:
    void applyRightFrustum(C4FrameBuffer *buffer);
    void applyLeftFrustum(C4FrameBuffer *buffer);
    void setOutDevice(C4OutDevice *dev);
    void singleShot2d(QImage *img, int frameIndex);
    void singleShot3d(QImage *img, int frameIndex);

    C4FrameBuffer   *mPrimaryBuffer; // 2D: Primary-Buffer, 3D-Stereo: left Buffer
    C4FrameBuffer   *mSecondBuffer;  // 3D-Stereo: right
    C4OutDevice     *mOutDevice;
    QList<C4Shape*>  mShapes;

    // Render-Settings
    int   mStartFrame;
    int   mEndFrame;

    // 3D-Settings
    float mStereoEffect;
    float mStereoEyeSeparation;
};

#endif // C4SCENE_H
