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

public slots:
    
protected:
    virtual void run();

private slots:
    void workRun();

private:
    void createThumb(int frameIndex, qint64 frameId, const QString &source);
    void render(int frameIndex, qint64 frameId, const QString &source);

    Movie *mMovie;

};

#endif // WORKERTHREAD_H
