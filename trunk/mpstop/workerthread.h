#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
#include <QImage>

class Movie;

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThread(QObject *parent = 0);
    
    void start(Movie *movie);

signals:
    void thumbCreated(int frameIndex, qint64 frameId, QImage thumb);
    void rendered(int frameIndex, qint64 frameId, QImage result);
    void movieCompleteRendered();

public slots:
    
protected:
    virtual void run();

private slots:
    void workRun();

private:
    void createThumb(int frameIndex, qint64 frameId, qint32 type, const QString &source);
    bool render(int frameIndex, qint64 frameId, qint32 type, const QString &source);

    Movie *mMovie;

};

#endif // WORKERTHREAD_H
