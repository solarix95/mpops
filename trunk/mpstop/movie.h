#ifndef MOVIE_H
#define MOVIE_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QSharedPointer>
#include <QMutex>

class Movie : public QObject
{
    Q_OBJECT
public:
    enum FrameType {
        FileSource,
        FreezeFrame,
        TweenFrame
    };

    explicit Movie(QObject *parent = 0);
    
    int  frameCount() const;
    int  fps() const;



    void     addFrames(const QStringList &fileList);

    FrameType type(int frame) const;
    QImage  *thumbNail(int frame) const;
    QImage  *rendered(int frame) const;

    // Thread-Interface:
    bool     frame(int index, qint64 &id, qint32 &type, bool &hasThumb, bool &isRendered, QString &source);
    bool     relativeImage(int fromIndex, qint64 fromId, int relIndex, qint64 &relIdent, QImage &renderedImage);


    static QSize thumbSize();
    QSize renderSize();

signals:
    void frameAppended(int index);
    void frameInserted(int index);
    void frameDeleted(int index);
    void frameChanged(int index);
    void isComplete(bool complete);
    void fpsChanged(int newFps);
    
public slots:
    // Thread-Anwsers:
    void jobThumb(int index, qint64 id, QImage thumb);
    void jobRender(int index, qint64 id, QImage result);
    void setMovieIsComplete();

    // GUI-Request:
    void addFreezeFrame(int startIndex);
    void removeFrame(int startIndex);
    void setFps(int fps);
    void setVideoWidth(int w);
    void setVideoHeight(int h);

private:
    void lock() const;
    void unlock() const;
    void resetRendering();

    typedef QSharedPointer<QImage> ImagePtr;



    struct Frame {
        qint64     ident;
        ImagePtr   thumbnail;
        ImagePtr   rendered;
        QString    source;
        FrameType  type;
    };

    QList<Frame*>  mFrames;
    QMutex         mMutex;
    qint64         mTopFrameIdent;
    int            mFps;
    QSize          mRenderSize;
};

#endif // MOVIE_H
