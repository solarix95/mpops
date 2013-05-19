#include "saveandclosejob.h"
#include <QDebug>

//---------------------------------------------------------------
SaveAndCloseJob::SaveAndCloseJob(QImage *img, const QString &toFilename)
{
  mImg        = img;
  mToFilename = toFilename;
}

//---------------------------------------------------------------
void SaveAndCloseJob::run()
{
  if (mImg->isNull()) {
    delete mImg;
    return;
  }

  bool done = mImg->save(mToFilename, 0, 99);
  Q_ASSERT(done);
  delete mImg;
}
