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
  QString         tocName;

  // --add-alpha
  bool            withAlpha;
  int             alphaHue;
  int             alphaHueTolerance;

  // --color-picker
  QPoint          colorPickerPixel;

  bool            withContExposure;
  int             contExposureFrames;

  // select
  bool            withSelect;
  int             selectInFrames;
  int             selectIgnFrames;

  sArgs() : withResize(false), withCropFrom(false), withCropTo(false),
            threadCount(QThread::idealThreadCount()),
            printHelp(false), withToc(false), withAlpha(false),
            withContExposure(false),contExposureFrames(-1),
            withSelect(false),selectInFrames(-1), selectIgnFrames(-1) {}
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
    ImagePtr selectFrame();
    bool     pickGeometry(ImagePtr firstFrame);
    QString  createFileName(const QString originalFileName, int frameIndex);

    Args            mArgs;
    int             mOrigIndex;  // original index, before "select"
    int             mInIndex;    // input index, after "select"
    int             mOutIndex;   // output index (tweening...)
    QList<OpQueue*> mThreads;
    CinelerraToc    mToc;
    ImageArray      mImageArray;
    ImagePtrs       mLastImages;
    int             mMaxFramesInQueue;
};

#endif // CONTROLLER_H
