#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QRect>
#include <QStringList>

#include "opqueue.h"
#include "cinelerratoc.h"

//---------------------------------------------------------------
typedef struct sArgs
{
  bool            withResize;
  QSize           toSize;

  bool            withCropFrom;
  QRect           fromRect;

  bool            withCropTo;
  QRect           toRect;

  int             threadCount;
  QList<QString>  fileList;

  QString         outDir;
  QString         format;
  QString         outfileTemplate;
  QString         tweening;
  bool            printHelp;
  bool            withToc;    // Create Cinelerra-TOC

  // --add-alpha
  bool            withAlpha;
  int             alphaHue;
  int             alphaHueTolerance;

  // --color-picker
  QPoint          colorPickerPixel;

  sArgs() : withResize(false), withCropFrom(false), withCropTo(false),
            threadCount(QThread::idealThreadCount()),
            printHelp(false), withToc(false), withAlpha(false) {};
} Args;

//---------------------------------------------------------------
class Controller : public QObject
{
Q_OBJECT
public:
    explicit Controller(const Args &args, QObject *parent = 0);

    bool run();

signals:
    void finished();

public slots:

private slots:
    void queueChanged(OpQueue*);

private:
    void    expandFilenames();
    bool    pickGeometry();
    QString createFileName(const QString originalFileName, int frameIndex);

    Args            mArgs;
    QList<QString>  mFilenames;
    int             mCurrentIndex;
    int             mJobIndex;
    QList<OpQueue*> mThreads;
    CinelerraToc    mToc;
    ImagePtr        mLastImage;
};

#endif // CONTROLLER_H
