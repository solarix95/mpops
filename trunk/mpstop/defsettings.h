#ifndef DEFSETTINGS_H
#define DEFSETTINGS_H

#include <QObject>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QRect>

class DefSettings : public QObject
{
    Q_OBJECT
public:
    explicit DefSettings(QObject *parent = 0);
    virtual ~DefSettings();

    void setLastInDir(const QString &inDir);
    QString lastInDir() const;
     void setLastOutDir(const QString &inDir);
    QString lastOutDir() const;

    int defaultWidth() const;
    int defaultHeight() const;
    int defaultFps() const;

    void   setLastMainPos(QPoint p);
    QPoint lastMainPos() const;
    void   setLastMainSize(QSize s);
    QSize  lastMainSize() const;

    void   setLastGeometry(QRect geo);
    QRect  lastGeometry() const;

signals:
    
public slots:
    void save();
    void load();

private:
    QString   mLastInDir;
    QString   mLastOutDir;
    int       mDefaultWidth;
    int       mDefaultHeight;
    int       mDefaultFps;

    QPoint    mLastMainPos;
    QSize     mLastMainSize;
    QRect     mLastMainGeometry;
};

#endif // DEFSETTINGS_H
