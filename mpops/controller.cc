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
    mOutIndex         = 1;
    mInIndex          = 0;
    mOrigIndex        = 0;
    mAtEnd            = false;
    mMaxFramesInQueue = qMax(2,args.contExposureFrames);

    // qDebug() << "ARGS" << args.fileList;
    if (args.fileList.count() == 1) {
        mImageArray.setVideoSource(args.fileList.first());
    } else
        mImageArray.appendSingleFrames(args.fileList);
}

//---------------------------------------------------------------
bool Controller::run()
{
    // qDebug() << mImageArray.isValid();
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
        mToc.setFilename(mArgs.outDir + QDir::separator() + (mArgs.tocName.isEmpty() ? mToc.filename():mArgs.tocName));

    return true;
}

//---------------------------------------------------------------
void Controller::queueChanged(OpQueue *queue)
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());

    if (!mThreads.contains(queue)) // ignore old Events
        return;

    ImagePtr img;
    if (queue->jobCount() == 0 && !mAtEnd) {
        img = selectFrame();
        if (img.isNull()) {
            mAtEnd = true;
        } else {
            if (mInIndex == 0) {
                img->load();
                pickGeometry(img);
            }
            bool     withTweens = !mArgs.tweening.isEmpty() && (mInIndex > 0);

            std::cout << "starting " << (mInIndex+1) << "/" << (mArgs.maxFrames > 0 ? mArgs.maxFrames : mImageArray.frameCount()) << std::endl;

            // Loading:
            queue->addJob(new OpenJob(img,mArgs.colorPickerPixel));

            // Crop-Operation:
            if (mArgs.withCropFrom) {
                QRect currentRect;
                if (mArgs.withCropTo) {

                    double imgFactor = (mInIndex+1)/((double)mImageArray.frameCount()); // 0 .. 1
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
            QString toFileName = createFileName(img->frameName(), withTweens ? mOutIndex+1 : mOutIndex);

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
                QString tweenFileName = createFileName(img->frameName(),mOutIndex);
                queue->addJob(new SaveAndCloseJob(out,tweenFileName));
                mToc.appendImage(dir.absoluteFilePath(tweenFileName));
                mOutIndex++;
            }

            mToc.appendImage(dir.absoluteFilePath(toFileName));

            mInIndex++;
            mOutIndex++;


            /* FIXME: why crash!??!?!?
            mLastImages << img;

            Q_ASSERT(mMaxFramesInQueue >= 0);
            while (mLastImages.count() > mMaxFramesInQueue)
                mLastImages.takeFirst();

            */
            if (mArgs.maxFrames > 0 && mOutIndex >= mArgs.maxFrames)
                mAtEnd = true;

        }
    }

    if ((mInIndex == mImageArray.frameCount()) || mAtEnd) {
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
        if (mArgs.withToc) {
            if (mToc.save())
                std::cout << "cinelerra-toc " << mToc.filename().toAscii().data() << " created" << std::endl;
            else
                std::cerr << "cannot write cinelerra-toc " <<  mToc.filename().toAscii().data() << std::endl;
        }
        emit finished();
    }

}

//---------------------------------------------------------------
ImagePtr Controller::selectFrame()
/*
  Skip frames if needed (--select=x/y and/or --skip=frms argument..)
*/
{
    // Skip frames..
    while (mArgs.skipFrames > mOrigIndex) {
        std::cout << "skip frame " << mOrigIndex << std::endl;
        mImageArray.skipFrame();
        mOrigIndex++;
    }

    ImagePtr ret;
    int groupIndex;
    bool skip;
    do {
        groupIndex = mOrigIndex % (mArgs.selectIgnFrames+mArgs.selectInFrames);
        skip       = mArgs.withSelect && groupIndex >= mArgs.selectInFrames;
        if (skip)
            mImageArray.skipFrame();
        else
            ret = mImageArray.nextFrame();

        mOrigIndex++;
    } while (skip);

    return ret;
}

//---------------------------------------------------------------
bool Controller::pickGeometry(ImagePtr firstFrame)
{
    // qDebug() << "PICK" << firstFrame.isNull() << mArgs.withCropFrom << mArgs.withCropTo;
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
        picker.setReferenceSelection(mArgs.fromRect);
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

    // qDebug() << "CREATE FILENAME" << originalFileName << frameIndex << mArgs.outfileTemplate << toFileName;
    return toFileName;
}
