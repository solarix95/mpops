#ifndef CINELERRATOC_H
#define CINELERRATOC_H

#include <QString>
#include <QSize>
#include <QList>

class CinelerraToc
{
public:
    CinelerraToc();


    void setFilename(const QString &tocFileName);
    QString filename() const;

    void appendImage(const QString &imageFileName);
    void setFps(int fps);
    void setSize(const QSize &size);

    bool save();

private:
    QString          mTocFileName;
    QList<QString>   mImages;
    int              mFps;
    QSize            mSize;
};

#endif // CINELERRATOC_H
