#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QMutex>
#include <QSharedPointer>

class Image
{
public:
    Image(QImage *img);
    ~Image();

    void waitForFinished();

    inline   void    lock()      { mMutex.lock();   };
    inline   QImage *img() const { return mImage;   };
    inline   void    unlock()    { mMutex.unlock(); };

    inline   void    setAllJobsDone()    { mAllJobsDone = true; }
    inline   bool    allJobsDone() const { return mAllJobsDone; };

private:
    QImage *mImage;
    QMutex  mMutex;
    bool    mAllJobsDone;
};

typedef QSharedPointer<Image> ImagePtr;

#endif // IMAGE_H
