#ifndef MOVIE_H
#define MOVIE_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QSharedPointer>
#include <QMutex>
#include <QDateTime>

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
    
    int     frameCount() const;
    int     fps() const;
    QString name() const;
    bool    isDirty() const;

    void     addFrames(const QStringList &fileList);
    void     clear();
    bool     save();
    bool     saveAs(const QString &projectName);
    bool     open(const QString &projectName);

    QString   source(int frame) const;
    FrameType type(int frame) const;
    QImage  *thumbNail(int frame) const;
    QImage  *rendered(int frame) const;

    // Thread-Interface:
    bool     frame(int index, qint64 &id, qint32 &type, bool &hasThumb, bool &isRendered, QString &source, QDateTime &renderTime);
    bool     relativeImage(int fromIndex, qint64 fromId, int relIndex, qint64 &relIdent, QImage &renderedImage);
    bool     scheduleImage(int index, qint64 id);

    static QSize thumbSize();
    QSize renderSize();

signals:
    void frameAppended(int index);
    void frameInserted(int index);
    void frameDeleted(int index);
    void frameChanged(int index);
    void isComplete(bool complete);
    void fpsChanged(int newFps);
    void sizeChanged(QSize newSize);
    void dirtyChanged();
    void requestFileName(QString *name);   // save new project
    void reloaded();                       // on "new", or "load"
    
public slots:
    // Thread-Anwsers:
    void jobThumb(int index, qint64 id, QImage thumb);
    void jobRender(int index, qint64 id, QImage result);
    void setMovieIsComplete();

    // GUI-Request:

    void addFreezeFrame(int startIndex = -1);
    void removeFrame(int startIndex);
    void setFps(int fps);
    void setVideoWidth(int w);
    void setVideoHeight(int h);

    // TOC-Parser
    void addFrame(const QString &filename);
    void handleTocComment(const QString &comment);
    void setVideoSize(QSize s);

private:
    void lock() const;
    void unlock() const;
    void resetRendering();
    void setDirty(bool d);

    typedef QSharedPointer<QImage> ImagePtr;

    struct Frame {
        qint64     ident;
        ImagePtr   thumbnail;
        ImagePtr   rendered;
        QString    source;
        FrameType  type;
        QDateTime  renderTime;
    };

    QList<Frame*>  mFrames;
    QMutex         mMutex;
    qint64         mTopFrameIdent;
    int            mFps;
    QSize          mRenderSize;

    QString        mProjectName;
    bool           mIsDirty;
};

#endif // MOVIE_H
