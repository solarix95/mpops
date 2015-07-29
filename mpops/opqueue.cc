#include "opqueue.h"
#include <QDebug>
#include <QCoreApplication>

#define THREAD_CONTEXT Q_ASSERT(QThread::currentThread() == this)
#define MAIN_CONTEXT   Q_ASSERT(QThread::currentThread() == qApp->thread())
//---------------------------------------------------------------
OpQueue::OpQueue(QObject *parent) :
    QThread(parent)
{
    MAIN_CONTEXT;
    moveToThread(this);
    connect(this, SIGNAL(queueChanged(OpQueue*)), this, SLOT(runJobs()), Qt::QueuedConnection);
}

//---------------------------------------------------------------
int OpQueue::jobCount() const
{
    MAIN_CONTEXT;
    ncThis()->mJobMutex.lock();
    int ret = mJobQueue.count();
    ncThis()->mJobMutex.unlock();
    return ret;
}

//---------------------------------------------------------------
void OpQueue::addJob(Job *job)
{
    MAIN_CONTEXT;
    ncThis()->mJobMutex.lock();
    mJobQueue << job;
    ncThis()->mJobMutex.unlock();
    emit queueChanged(this);
}

//---------------------------------------------------------------
void OpQueue::runJobs()
{
    THREAD_CONTEXT;

    ncThis()->mJobMutex.lock();
    Job *job = NULL;
    if (!mJobQueue.isEmpty())
        job = mJobQueue[0];
    ncThis()->mJobMutex.unlock();

    if (job) {
        job->run();

        ncThis()->mJobMutex.lock();
        delete mJobQueue.takeFirst();
        ncThis()->mJobMutex.unlock();
        emit queueChanged(this);
    }
}

//---------------------------------------------------------------
OpQueue  *OpQueue::ncThis() const
{
    return const_cast<OpQueue*>(this);
}
