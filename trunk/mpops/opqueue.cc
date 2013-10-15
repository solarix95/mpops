#include "opqueue.h"
#include <QDebug>

//---------------------------------------------------------------
OpQueue::OpQueue(QObject *parent) :
    QThread(parent)
{
  moveToThread(this);
  connect(this, SIGNAL(queueChanged(OpQueue*)), this, SLOT(runJobs()), Qt::QueuedConnection);
}

//---------------------------------------------------------------
int OpQueue::jobCount() const
{
  ncThis()->mJobMutex.lock();
  int ret = mJobQueue.count();
  ncThis()->mJobMutex.unlock();
  return ret;
}

//---------------------------------------------------------------
void OpQueue::addJob(Job *job)
{
  ncThis()->mJobMutex.lock();
  mJobQueue << job;
  ncThis()->mJobMutex.unlock();
  emit queueChanged(this);
}

//---------------------------------------------------------------
void OpQueue::runJobs()
{

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
