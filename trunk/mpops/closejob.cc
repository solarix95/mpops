#include "closejob.h"
#include <QDebug>

//---------------------------------------------------------------
CloseJob::CloseJob(ImagePtr img)
{
  mImg        = img;
}

//---------------------------------------------------------------
void CloseJob::run()
{
  mImg->lock();
  mImg->setAllJobsDone();
  mImg->unlock();
}
