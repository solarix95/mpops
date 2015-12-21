#ifndef C3OUTDEVICE_H
#define C3OUTDEVICE_H

#include <QObject>
#include <QImage>

class C4FrameBuffer;
class C4OutDevice : public QObject
{
    Q_OBJECT
public:
    explicit C4OutDevice(QObject *parent = 0);

    virtual bool isStereo() const = 0;
    virtual void setAttribute(const QString &name, const QVariant &value);

signals:
    void playerFrame(const QImage &img);
    
public slots:
    virtual void   init();
    virtual QImage appendFrame(const C4FrameBuffer *left, const C4FrameBuffer *right = NULL) = 0;
    virtual void   save();
};

#endif // C3OUTDEVICE_H
