#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QMutex>
#include <QSharedPointer>

class Image
{
public:
    Image(QImage *img, const QString &frameName);
    virtual ~Image();

    bool load();
    void waitForFinished();

    inline   void    lock()       { mMutex.lock();   }
    inline   QImage *img()  const { return mImage;   }
    inline   void    unlock()     { mMutex.unlock(); }

    inline   void    setAllJobsDone()    { mAllJobsDone = true; }
    inline   bool    allJobsDone() const { return mAllJobsDone; }
    inline  QString  frameName() const   { return mFrameName;   }
protected:
    virtual void loadImage(QImage *img);

private:
    QImage *mImage;
    QMutex  mMutex;
    bool    mAllJobsDone;
    QString mFrameName;
};

typedef QSharedPointer<Image> ImagePtr;
typedef QList<ImagePtr>       ImagePtrs;

#endif // IMAGE_H
