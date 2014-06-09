#include "defsettings.h"
#include <QSettings>

DefSettings::DefSettings(QObject *parent) :
    QObject(parent)
{
    load();
}

DefSettings::~DefSettings()
{
}

void DefSettings::setLastInDir(const QString &inDir)
{
    mLastInDir = inDir;
    save();
}

QString DefSettings::lastInDir() const
{
    return mLastInDir;
}

void DefSettings::setLastOutDir(const QString &outDir)
{
    mLastOutDir = outDir;
    save();
}

QString DefSettings::lastOutDir() const
{
    return mLastOutDir;
}

int DefSettings::defaultWidth() const
{
    return mDefaultWidth;
}

int DefSettings::defaultHeight() const
{
    return mDefaultHeight;
}

int DefSettings::defaultFps() const
{
    return mDefaultFps;
}

void DefSettings::setLastMainPos(QPoint p)
{
    mLastMainPos = p;
    save();
}

QPoint DefSettings::lastMainPos() const
{
    return mLastMainPos;
}

void DefSettings::setLastMainSize(QSize s)
{
    mLastMainSize = s;
    save();
}

QSize DefSettings::lastMainSize() const
{
    return mLastMainSize;
}

void DefSettings::setLastGeometry(QRect geo)
{
    mLastMainGeometry = geo;
    save();
}

QRect DefSettings::lastGeometry() const
{
    return mLastMainGeometry;
}

void DefSettings::save()
{
     QSettings settings("MPOps", "mpstop");

     settings.setValue("video/width",mDefaultWidth);
     settings.setValue("video/height",mDefaultHeight);
     settings.setValue("video/fps",mDefaultFps);

     settings.setValue("main/lastInDir",mLastInDir);
     settings.setValue("main/lastOutDir",mLastOutDir);
     settings.setValue("main/lastpos",mLastMainPos);
     settings.setValue("main/lastsize",mLastMainSize);
     settings.setValue("main/lastgeometry",mLastMainGeometry);
}


void DefSettings::load()
{
     QSettings settings("MPOps", "mpstop");

     mDefaultWidth  = settings.value("video/width",1920).toInt();
     mDefaultHeight = settings.value("video/height",1080).toInt();
     mDefaultFps    = settings.value("video/fps",12).toInt();

     mLastInDir     = settings.value("main/lastInDir").toString();
     mLastOutDir    = settings.value("main/lastOutDir").toString();

     mLastMainPos   = settings.value("main/lastpos",QPoint(10,10)).toPoint();
     mLastMainSize  = settings.value("main/lastsize",QSize(800,600)).toSize();

     mLastMainGeometry = settings.value("main/lastgeometry",QRect(10,10,800,600)).toRect();
}
