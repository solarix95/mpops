#include "cinelerratoc.h"
#include <QFile>
#include <QTextStream>

/* -------------------------------------------------------------------------------------*\

PNGLIST
#Image sequence format to be loaded into Cinelerra/Broadcast2000
25 #FPS
1440 #Width
1080 #Height
#-------------------------------------
#Now the paths to the images
#-------------------------------------
/path/frame1.png
...
\*---------------------------------------------------------------------------------------*/

//---------------------------------------------------------------
CinelerraToc::CinelerraToc()
{
  mTocFileName  = "content.toc";
  mFps          = 24;
}

//---------------------------------------------------------------
void CinelerraToc::setFilename(const QString &tocFileName)
{
  mTocFileName = tocFileName;
}

//---------------------------------------------------------------
QString CinelerraToc::filename() const
{
  return mTocFileName;
}

//---------------------------------------------------------------
void CinelerraToc::appendImage(const QString &imageFileName)
{
  mImages << imageFileName;
}

//---------------------------------------------------------------
void CinelerraToc::setFps(int fps)
{
  mFps = fps;
}

//---------------------------------------------------------------
void CinelerraToc::setSize(const QSize &size)
{
  mSize = size;
}

//---------------------------------------------------------------
bool CinelerraToc::save()
{
  QFile file(mTocFileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return false;

  QTextStream out(&file);
  out << "PNGLIST"                                                          << "\n";
  out << "#Image sequence format to be loaded into Cinelerra/Broadcast2000" << "\n";
  out << mFps << " #FPS"                                                    << "\n";
  out << mSize.width()  << " #Width"                                        << "\n";
  out << mSize.height() << " #Height"                                       << "\n";
  out << "#-------------------------------------"                           << "\n";
  out << "#Now the paths to the images"                                     << "\n";
  out << "#-------------------------------------"                           << "\n";
  foreach(QString nextFile, mImages)
    out << nextFile << "\n";
  return true;
}

