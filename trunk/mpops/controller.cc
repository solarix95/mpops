#include "controller.h"
#include <iostream>
#include <QImage>
#include <QImageReader>
#include <QDebug>
#include <QFile>
#include <QDir>
#include "openjob.h"
#include "cropjob.h"
#include "resizejob.h"
#include "blueboxjob.h"
#include "saveandclosejob.h"
#include "tweeningavg.h"
#include "shared/geometrypicker.h"

//---------------------------------------------------------------
#define SCALE(a,b,imfFactor)  ((a) + (imgFactor*((b)-(a))))

//---------------------------------------------------------------
Controller::Controller(const Args &args, QObject *parent) :
    QObject(parent)
{
  mArgs = args;
  expandFilenames();
  mJobIndex = 1;
}

//---------------------------------------------------------------
bool Controller::run()
{
  if (mFilenames.isEmpty()) { // Nothing to be done
    return false;
  }

  if (!pickGeometry())
      return false;

  Q_ASSERT(mThreads.isEmpty());
  for (int i=0; i<mArgs.threadCount; i++) {
    mThreads << new OpQueue();
    mThreads.last()->start();
    connect(mThreads.last(), SIGNAL(queueChanged(OpQueue*)), this, SLOT(queueChanged(OpQueue*)), Qt::QueuedConnection);
  }

  foreach(OpQueue *nextQueue, mThreads) {
     queueChanged(nextQueue);
  }

  if (mArgs.withResize)
      mToc.setSize(mArgs.toSize);

  if (!mArgs.outDir.isEmpty())
    mToc.setFilename(mArgs.outDir + QDir::separator() + mToc.filename());

  return true;
}

//---------------------------------------------------------------
void Controller::queueChanged(OpQueue *queue)
{
  if ((queue->jobCount() == 0) && (mCurrentIndex < mFilenames.count())) {
    ImagePtr img(new Image(new QImage));
    bool     withTweens = !mArgs.tweening.isEmpty() && (mCurrentIndex > 0);

    QString fromFileName = mFilenames[mCurrentIndex];
    std::cout << "starting " << (mCurrentIndex+1) << "/" << mFilenames.count() << std::endl;

    // Loading:
    queue->addJob(new OpenJob(img,fromFileName,mArgs.colorPickerPixel));

    // Crop-Operation:
    if (mArgs.withCropFrom) {
      QRect currentRect;
      if (mArgs.withCropTo) {

        double imgFactor = (mCurrentIndex+1)/((double)mFilenames.count()); // 0 .. 1
        currentRect.setX(SCALE(mArgs.fromRect.x(), mArgs.toRect.x(),imgFactor));
        currentRect.setY(SCALE(mArgs.fromRect.y(), mArgs.toRect.y(),imgFactor));
        currentRect.setWidth(SCALE(mArgs.fromRect.width(), mArgs.toRect.width(),imgFactor));
        currentRect.setHeight(SCALE(mArgs.fromRect.height(), mArgs.toRect.height(),imgFactor));
      } else
        currentRect = mArgs.fromRect;
      queue->addJob(new CropJob(img,currentRect));
    }

    // Resize-Operation
    if (mArgs.withResize)
        queue->addJob(new ResizeJob(img,mArgs.toSize));

    // Bluebox-Operation (add-alpha)
    if (mArgs.withAlpha)
        queue->addJob(new BlueboxJob(img,mArgs.alphaHue, mArgs.alphaHueTolerance));

    // Store-Operation:
    QString toFileName = createFileName(fromFileName, withTweens ? mJobIndex+1 : mJobIndex);

    queue->addJob(new SaveAndCloseJob(img,toFileName));
    QDir dir;
    mToc.appendImage(dir.absoluteFilePath(toFileName));

    if (withTweens && (mArgs.tweening == "avg")) {
      Q_ASSERT(!mArgs.outfileTemplate.isEmpty());
      ImagePtr out(new Image(new QImage()));
      queue->addJob(new TweeningAvg(mLastImage,img,out));
      QString tweenFileName = createFileName(fromFileName,mJobIndex);
      queue->addJob(new SaveAndCloseJob(out,tweenFileName));
      mJobIndex++;
    }
    mCurrentIndex++;
    mJobIndex++;
    mLastImage = img;
  }


  if ((mCurrentIndex == mFilenames.count())) {
    int jobCount = 0;
    foreach(OpQueue *nextQueue, mThreads) {
      if (nextQueue->jobCount() == 0) {
        mThreads.removeAll(nextQueue);
        nextQueue->quit();
        nextQueue->wait();
        nextQueue->deleteLater();
      } else
        jobCount += nextQueue->jobCount();
    }
    if (jobCount > 0)
      std::cout << "waiting for " << jobCount << " jobs" << std::endl;
  }

  if (mThreads.isEmpty()) {
    if (mArgs.withToc && mToc.save())
        std::cout << "cinelerra-toc created" << std::endl;
    emit finished();
  }
}

//---------------------------------------------------------------
void Controller::expandFilenames()
{
  mFilenames.clear();
  mCurrentIndex = 0;
  if ((mArgs.fileList.count() == 1) && !QFile::exists(mArgs.fileList[0])) { // maybe a template?
    int index = 0;
    bool someFilesFound;
    do {
      someFilesFound = false;
      for(int i=0; i<100; i++) {
        QString expandedFilename = mArgs.fileList[0];
        expandedFilename.sprintf(expandedFilename.toAscii().data(),index);
        if (QFile::exists(expandedFilename)) {
          someFilesFound = true;
          mFilenames << expandedFilename;
        }
        index++;
      }
    } while (someFilesFound);
  } else {
    mFilenames = mArgs.fileList;
  }
  std::cout << "todo: " << mFilenames.count() << " images" << std::endl;
}

//---------------------------------------------------------------
bool Controller::pickGeometry()
{
    if (mFilenames.isEmpty() || !QFile::exists(mFilenames.first()))
        return true;

    QImage img(mFilenames.first());
    if (img.isNull())
        return false;

    if (mArgs.withCropFrom && mArgs.fromRect.width() <= 0) {
        GeometryPicker picker(&img,"pick 'from-crop'");
        if (!picker.exec())
            return false;
        mArgs.fromRect = picker.selection();
    }
    if (mArgs.withCropTo && mArgs.toRect.width() <= 0) {
        GeometryPicker picker(&img,"pick 'to-crop'");
        if (!picker.exec())
            return false;
        mArgs.toRect = picker.selection();
    }
    return true;
}

//---------------------------------------------------------------
QString Controller::createFileName(const QString originalFileName, int frameIndex)
{
  QString originalFileNameWithoutPath = originalFileName;
  while (originalFileNameWithoutPath.indexOf(QDir::separator()) >= 0)
    originalFileNameWithoutPath.remove(0,originalFileNameWithoutPath.indexOf(QDir::separator()) + 1);
    
  QString toFileName = mArgs.outfileTemplate.isEmpty() ? originalFileNameWithoutPath : QString().sprintf(mArgs.outfileTemplate.toAscii().data(),frameIndex);
  if (!mArgs.outDir.isEmpty())
    toFileName = mArgs.outDir + QDir::separator() + toFileName;
  if (!mArgs.format.isEmpty()) // replace extension:
    toFileName = toFileName.left(toFileName.indexOf(".")+1) + mArgs.format;

  return toFileName;
}
