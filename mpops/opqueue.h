#ifndef OPQUEUE_H
#define OPQUEUE_H

#include <QThread>
#include <QMutex>
#include "job.h"

class OpQueue : public QThread
{
Q_OBJECT
public:
    explicit OpQueue(QObject *parent = 0);

    int jobCount() const;
    void addJob(Job *job);

signals:
    void queueChanged(OpQueue*);

private slots:
    void runJobs();

private:
    OpQueue  *ncThis() const;

    QMutex       mJobMutex;
    QList<Job*>  mJobQueue;
};

#endif // OPQUEUE_H
