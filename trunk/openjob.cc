#include "openjob.h"

//---------------------------------------------------------------
OpenJob::OpenJob(QImage *img, const QString &fromFilename)
{
  mImg          = img;
  mFromFilename = fromFilename;
}

//---------------------------------------------------------------
void OpenJob::run()
{
  mImg->load(mFromFilename);
}
