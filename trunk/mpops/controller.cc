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
#include "closejob.h"
#include "tweeningavg.h"
#include "longexposurejob.h"
#include "shared/geometrypicker.h"

//---------------------------------------------------------------
#define SCALE(a,b,imfFactor)  ((a) + (imgFactor*((b)-(a))))

//---------------------------------------------------------------
Controller::Controller(const Args &args, QObject *parent) :
    QObject(parent)
{
    mArgs             = args;
    mJobIndex         = 1;
    mCurrentIndex     = 0;
    mMaxFramesInQueue = qMax(2,args.contExposureFrames);

    qDebug() << "ARGS" << args.fileList;
    if (args.fileList.count() == 1) {
        mImageArray.setVideoSource(args.fileList.first());
    } else
        mImageArray.appendSingleFrames(args.fileList);
}

//---------------------------------------------------------------
bool Controller::run()
{
    qDebug() << mImageArray.isValid();
    if (!mImageArray.isValid())
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
    ImagePtr img;
    bool  atEnd = false;

    if (queue->jobCount() == 0) {
        img = mImageArray.nextFrame();
        if (img.isNull())
            atEnd = true;
        else {
            if (mCurrentIndex == 0) {
                img->load();
                pickGeometry(img);
            }
            bool     withTweens = !mArgs.tweening.isEmpty() && (mCurrentIndex > 0);

            std::cout << "starting " << (mCurrentIndex+1) << "/" << mImageArray.frameCount() << std::endl;

            // Loading:
            queue->addJob(new OpenJob(img,mArgs.colorPickerPixel));

            // Crop-Operation:
            if (mArgs.withCropFrom) {
                QRect currentRect;
                if (mArgs.withCropTo) {

                    double imgFactor = (mCurrentIndex+1)/((double)mImageArray.frameCount()); // 0 .. 1
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
            QString toFileName = createFileName(img->frameName(), withTweens ? mJobIndex+1 : mJobIndex);

            if (mArgs.withContExposure) {
                queue->addJob(new CloseJob(img)); // saveing is done by ExposureJob
                ImagePtr outImg(new Image(new QImage,""));
                queue->addJob(new LongExposureJob(ImagePtrs() << mLastImages << img, outImg,toFileName,mArgs.contExposureFrames));
            } else {
                queue->addJob(new SaveAndCloseJob(img,toFileName));
            }

            QDir dir;
            if (withTweens && (mArgs.tweening == "avg")) {
                Q_ASSERT(!mArgs.outfileTemplate.isEmpty());
                ImagePtr out(new Image(new QImage(),"")); // TODO: tweenFileName direkt übergeben?
                queue->addJob(new TweeningAvg(mLastImages.last(),img,out));
                QString tweenFileName = createFileName(img->frameName(),mJobIndex);
                queue->addJob(new SaveAndCloseJob(out,tweenFileName));
                mToc.appendImage(dir.absoluteFilePath(tweenFileName));
                mJobIndex++;
            }

            mToc.appendImage(dir.absoluteFilePath(toFileName));

            mCurrentIndex++;
            mJobIndex++;
            mLastImages << img;
            while (mLastImages.count() > mMaxFramesInQueue)
                mLastImages.takeFirst();
        }
    }

    if ((mCurrentIndex == mImageArray.frameCount()) || atEnd) {
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
bool Controller::pickGeometry(ImagePtr firstFrame)
{
    qDebug() << "PICK" << firstFrame.isNull() << mArgs.withCropFrom << mArgs.withCropTo;
    if (firstFrame.isNull())
        return true;

    if (mArgs.withCropFrom && mArgs.fromRect.width() <= 0) {
        GeometryPicker picker(firstFrame->img(),"pick 'from-crop'");
        if (!picker.exec())
            return false;
        mArgs.fromRect = picker.selection();
    }
    if (mArgs.withCropTo && mArgs.toRect.width() <= 0) {
        GeometryPicker picker(firstFrame->img(),"pick 'to-crop'");
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
    if (!mArgs.format.isEmpty()) { // replace extension:
        int extPos = toFileName.indexOf(".");
        if (extPos > 0)
            toFileName = toFileName.left(toFileName.indexOf(".")+1) + mArgs.format;
        else
            toFileName += "." +mArgs.format;
    }

    return toFileName;
}
