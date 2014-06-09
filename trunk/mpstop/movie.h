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
    explicit Movie(QObject *parent = 0);
    
    int  frameCount() const;

    void     addFrames(const QStringList &fileList);

    QImage  *thumbNail(int frame) const;
    QImage  *rendered(int frame) const;

    bool     frame(int index, qint64 &id, bool &hasThumb, bool &isRendered, QString &source);

    static QSize thumbSize();
    static QSize renderSize();

signals:
    void frameAppended(int index);
    void frameChanged(int index);
    
public slots:
    void jobThumb(int index, qint64 id, QImage thumb);
    void jobRender(int index, qint64 id, QImage result);

private:
    void lock() const;
    void unlock() const;

    typedef QSharedPointer<QImage> ImagePtr;

    struct Frame {
        qint64   ident;
        ImagePtr thumbnail;
        ImagePtr rendered;
        QString  source;
    };

    QList<Frame*>  mFrames;
    QMutex         mMutex;
    qint64         mTopFrameIdent;
};

#endif // MOVIE_H
