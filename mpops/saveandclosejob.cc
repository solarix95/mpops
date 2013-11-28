#include "saveandclosejob.h"
#include <QDebug>

//---------------------------------------------------------------
SaveAndCloseJob::SaveAndCloseJob(ImagePtr img, const QString &toFilename)
{
  mImg        = img;
  mToFilename = toFilename;
}

//---------------------------------------------------------------
void SaveAndCloseJob::run()
{
  mImg->lock();

  if (!mImg->img()->isNull()) {
    bool done = mImg->img()->save(mToFilename, 0, 99);
    if (!done)
      qWarning() << "SaveAndCloseJob::run: cannot save file:" << mToFilename;
  }

  mImg->setAllJobsDone();
  mImg->unlock();
}
