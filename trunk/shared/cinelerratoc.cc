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

enum TocParseStage {
    ParseTocType,
    ParseFps,
    ParseWidth,
    ParseHeight,
    ParseFiles
};

//---------------------------------------------------------------
CinelerraToc::CinelerraToc()
    : QObject()
{
    mTocFileName  = "content.toc";
    mFps          = 24;
    mTocType      = "PNGLIST";
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
void CinelerraToc::setTocType(const QString &tocType)
{
    Q_ASSERT(!tocType.isEmpty());
    mTocType = tocType;
}

//---------------------------------------------------------------
QString CinelerraToc::tocType() const
{
    return mTocType;
}

//---------------------------------------------------------------
void CinelerraToc::appendImage(const QString &imageFileName)
{
    mImages << imageFileName;
}

//---------------------------------------------------------------
QList<QString> CinelerraToc::images() const
{
    return mImages;
}

//---------------------------------------------------------------
void CinelerraToc::setFps(int fps)
{
    mFps = fps;
}

//---------------------------------------------------------------
int CinelerraToc::fps() const
{
    return mFps;
}

//---------------------------------------------------------------
void CinelerraToc::setSize(const QSize &size)
{
    mSize = size;
}

//---------------------------------------------------------------
QSize CinelerraToc::size() const
{
    return mSize;
}

//---------------------------------------------------------------
bool CinelerraToc::save()
{
    QFile file(mTocFileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << "PNGLIST"                                                          << "\n";
    out << "#Image sequence format to be loaded into Cinelerra/Broadcast2000 and Darksuit" << "\n";
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

//---------------------------------------------------------------
bool CinelerraToc::load(const QString &filename)
{
    mImages.clear();

    QFile inToc(filename);
    if (!inToc.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    TocParseStage stage = ParseTocType;
    QTextStream stream(&inToc);
    QString line;
    do {
        line = stream.readLine().trimmed();
        if (line.isEmpty())
            continue;
        if (line.startsWith("#"))
            emit parsedComment(line);
        else {
            if (line.contains("#")) { // inline-comment: "24 #FPS"
                int commentStart = line.indexOf("#");
                line.remove(commentStart,line.length()-commentStart);
                line = line.trimmed();
            }
            Q_ASSERT(!line.isEmpty());
            switch (stage) {
              case ParseTocType:
                mTocType = line;
                emit parsedTocType(mTocType);
                stage = ParseFps;
                break;
              case  ParseFps:
                mFps = line.toInt();
                emit parsedFps(mFps);
                stage = ParseWidth;
                break;
             case  ParseWidth:
                mSize.setWidth(line.toInt());
                stage = ParseHeight;
                break;
             case  ParseHeight:
                mSize.setHeight(line.toInt());
                emit parsedSize(mSize);
                stage = ParseFiles;
                break;
             case  ParseFiles:
                mImages << line;
                emit parsedFile(line);
                break;
            }
        }
    } while (!line.isNull());

    return true;
}

