#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QRect>
#include <QStringList>
#include "opqueue.h"
#include "cinelerratoc.h"
#include "shared/fileimage.h"
#include "shared/imagearray.h"

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

  bool            withContExposure;
  int             contExposureFrames;

  sArgs() : withResize(false), withCropFrom(false), withCropTo(false),
            threadCount(QThread::idealThreadCount()),
            printHelp(false), withToc(false), withAlpha(false),
            withContExposure(false),contExposureFrames(-1) {}
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
    bool    pickGeometry(ImagePtr firstFrame);
    QString createFileName(const QString originalFileName, int frameIndex);

    Args            mArgs;
    int             mCurrentIndex;
    int             mJobIndex;
    QList<OpQueue*> mThreads;
    CinelerraToc    mToc;
    ImageArray      mImageArray;
    ImagePtrs       mLastImages;
    int             mMaxFramesInQueue;
};

#endif // CONTROLLER_H
