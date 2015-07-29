#ifndef CINELERRATOC_H
#define CINELERRATOC_H

#include <QObject>
#include <QString>
#include <QSize>
#include <QList>

class CinelerraToc : public QObject
{
    Q_OBJECT

public:
    CinelerraToc();

    void setFilename(const QString &tocFileName);
    QString filename() const;
    void setTocType(const QString &tocType);
    QString tocType() const;

    void appendImage(const QString &imageFileName);
    QList<QString> images() const;
    void setFps(int fps);
    int  fps() const;
    void setSize(const QSize &size);
    QSize size() const;

    bool save();
    bool load(const QString &filename);

signals:
    // signals during loading:
    void parsedTocType(const QString &tocType);
    void parsedSize(QSize s);
    void parsedFps(int fps);
    void parsedComment(const QString &comment);
    void parsedFile(const QString &fileName);


private:
    QString          mTocFileName;
    QList<QString>   mImages;
    int              mFps;
    QSize            mSize;
    QString          mTocType;
};

#endif // CINELERRATOC_H
