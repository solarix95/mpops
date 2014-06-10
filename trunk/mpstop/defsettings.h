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


    int defaultVideoWidth() const;
    int defaultVideoHeight() const;
    int defaultVideoFps() const;


    void   setLastMainPos(QPoint p);
    QPoint lastMainPos() const;
    void   setLastMainSize(QSize s);
    QSize  lastMainSize() const;

    void   setLastGeometry(QRect geo);
    QRect  lastGeometry() const;


    bool playerLoop() const;

signals:
    
public slots:
    void setDefaultVideoWidth(int width);
    void setDefaultVideoHeight(int height);
    void setDefaultVideoFps(int fps);
    void setPlayerLoop(bool loop);

    void save();
    void load();

private:
    QString   mLastInDir;
    QString   mLastOutDir;
    int       mDefaultWidth;
    int       mDefaultHeight;
    int       mDefaultFps;

    bool      mPlayerLoop;

    QPoint    mLastMainPos;
    QSize     mLastMainSize;
    QRect     mLastMainGeometry;
};

#endif // DEFSETTINGS_H
