
#include <QFile>
#include <QImageReader>
#include <QColor>
#include <QImage>
#include <QDir>
#include <QDebug>
#include <QTime>
#include <QFileInfoList>
#include <QFileInfo>
#include "shared/imagearray.h"
#include "shared/fileimage.h"
#include "shared/cinelerratoc.h"

//---------------------------------------------------------------
ImageArray::ImageArray(const QString &videosource)
{
    mIsValid           = false;
    mFrameIndex        = -1;
    mSource            = Uninitialized;
    mMaxWidth          = -1;

#ifdef WITH_OPENCV
    mCvCapture         = NULL;
    mCvFrame           = NULL;
    mCvFrameCount      = 0;
#endif
    setVideoSource(videosource);
}

//---------------------------------------------------------------
ImageArray::~ImageArray()
{
#ifdef WITH_OPENCV
    if (mCvCapture)
        cvReleaseCapture(&mCvCapture);
#endif
}

//---------------------------------------------------------------
void ImageArray::setVideoSource(const QString &videosource)
{
    mSourceName = videosource;

    // qDebug() << "ImageArray::setVideoSource" << videosource;
    if (mSourceName.isEmpty())
        return;

    mFrameIndex    = 0;
    if (mSourceName.toLower().endsWith(".toc")) {
        mSource        = ImageList;
        CinelerraToc toc;
        mIsValid = toc.load(mSourceName);
        if (mIsValid)
            mExternalFiles = toc.images();
    } else if (validateExternalImage(mSourceName)) {
        mSource        = ImageList;
        mExternalFiles = mSourceName.contains('%') ? expandTemplateName(mSourceName) : QStringList() << mSourceName;
        mIsValid       = !mExternalFiles.isEmpty();
    } else if (loadFromDirectory()) {
        mSource = ImageList;
    } else {

#ifdef WITH_OPENCV
        mSource       = VideoFile;
        reset();
#endif
        // TODO
    }
}

//---------------------------------------------------------------
bool ImageArray::appendSingleFrame(const QString &externalFileName)
{
    Q_ASSERT(mSource == Uninitialized || mSource == ImageList);
    if (!validateExternalImage(externalFileName))
        return false;
    mExternalFiles << externalFileName;

    if (mSource == Uninitialized) {      // on first call:
        mSource            = ImageList;
        mFrameIndex        = 0;
        mIsValid           = true;
    }
    return true;
}

//---------------------------------------------------------------
bool ImageArray::appendSingleFrames(const QStringList &externalFileNames)
{
    foreach(const QString &nextFrame,externalFileNames) {
        if (!appendSingleFrame(nextFrame))
            return false;
    }
    return true;
}

//---------------------------------------------------------------
void ImageArray::setMaxWidthHint(int maxWidth)
{
    mMaxWidth = maxWidth;
}

//---------------------------------------------------------------
void ImageArray::reset()
{
    switch(mSource) {
    case ImageList : {
        mFrameIndex = 0;
    } break;
    case VideoFile : {
#ifdef WITH_OPENCV
        if (mCvCapture)
            cvReleaseCapture(&mCvCapture);
        mCvCapture    = cvCreateFileCapture(mSourceName.toAscii().data());
        mIsValid      = mCvCapture ? true:false;
        mCvFrameCount = mCvCapture ? (int)cvGetCaptureProperty(mCvCapture,CV_CAP_PROP_FRAME_COUNT) : 0;
        mFrameIndex   = 0;
#endif
    }
    }

}

//---------------------------------------------------------------
bool     ImageArray::isValid() const
{
    return mIsValid;
}

//---------------------------------------------------------------
ImageArray::Source ImageArray::source() const
{
    return mSource;
}

//---------------------------------------------------------------
ImagePtr ImageArray::nextFrame(bool *hasNext)
{
    ImagePtr img;
    if (mFrameIndex < 0)
        return img;

    switch(mSource) {
    case ImageList : {
        if (mFrameIndex < mExternalFiles.count()) {
            img = ImagePtr(new FileImage(new QImage,mExternalFiles[mFrameIndex],mMaxWidth));
            mFrameIndex++;
        }
    } break;
    case VideoFile : {
#ifdef WITH_OPENCV
        Q_ASSERT(mCvCapture);

        mCvFrame = cvQueryFrame( mCvCapture );

        if(mCvFrame) {
            if (mCvFrame->nChannels != 3) {
                qWarning() << "CHANNELS";
                return img;
            }
            if (mCvFrame->depth != IPL_DEPTH_8U) {
                qWarning() << "depth" << mCvFrame->depth;
                return img;
            }

            int nc   = mCvFrame->nChannels;
            int wstep= mCvFrame->widthStep;

            unsigned char *data= reinterpret_cast<unsigned char *>
                    (mCvFrame->imageData);


            // Step: don't return Images in a higher resolution than requested..
            int step = mCvFrame->width / mMaxWidth;
            if (step <= 0)
                step = 1;
            else
                step -= step % 2;

            if (!step || (mCvFrame->width % step) || (mCvFrame->height % step))
                step = 1;

            img = ImagePtr(new FileImage(new QImage(mCvFrame->width/step,mCvFrame->height/step,QImage::Format_RGB32),QString("Frame_%1").arg(mFrameIndex)));
            for (int y=0; y<mCvFrame->height; y += step) {
                Q_ASSERT(y < mCvFrame->height);
                QRgb *rawRow = (QRgb*)img->img()->scanLine(y/step);
                for (int x=0; x<mCvFrame->width; x += step) {
                    Q_ASSERT(x < mCvFrame->width);
                    int firstIndex = y*wstep + x*nc;
                    // BGR, not RGB:
                    uchar blue  = data[firstIndex + 0];
                    uchar green = data[firstIndex + 1];
                    uchar red   = data[firstIndex + 2];
                    // Slow:
                    // img->img()->setPixel(x,y,qRgb(red,green,blue));
                    rawRow[x/step] = qRgb(red,green,blue);
                }
            }
            mFrameIndex++;
        }
#endif
    }
    }
    if (hasNext)
        *hasNext = !img.isNull();
    return img;
}

//---------------------------------------------------------------
int ImageArray::nextIndex() const
{
    return mFrameIndex;
}

//---------------------------------------------------------------
bool ImageArray::skipFrame()
{
    if (mFrameIndex < 0)
        return false;

    bool hasMore = true;
    switch(mSource) {
    case ImageList : {
        if (mFrameIndex < mExternalFiles.count())
            mFrameIndex++;
        else
            hasMore = false;
    } break;
    case VideoFile : {
#ifdef WITH_OPENCV
        Q_ASSERT(mCvCapture);
        /*
          mCvFrame = cvQueryFrame( mCvCapture );
          hasMore = mCvFrame != NULL;
          */
        hasMore = cvGrabFrame(mCvCapture) == 1;
        if (hasMore)
            mFrameIndex++;
#endif
    }
    }

    return hasMore;
}

//---------------------------------------------------------------
qint64  ImageArray::frameCount() const
{
    switch(mSource) {
    case ImageList : return mExternalFiles.count();
#ifdef WITH_OPENCV
    case VideoFile : return mCvFrameCount;
#endif
    }
    return -1;
}

//---------------------------------------------------------------
//                       PUBLIC STATIC
//---------------------------------------------------------------
bool ImageArray::validateExternalImage(const QString &fileName)
{
    static QList<QByteArray> formats = QImageReader::supportedImageFormats();
    foreach(QByteArray nextFormat, formats)
        if (fileName.toLower().endsWith(QString(".%1").arg(QString(nextFormat.toLower()))))
            return true;
    return false;
}

//---------------------------------------------------------------
QStringList ImageArray::expandTemplateName(const QString &templateName)
{
    int frameIndex   = 1;
    QStringList fileList;
    do {
        QString nextFilename = QString().sprintf(templateName.toAscii().data(),frameIndex++);
        if (QFile::exists(nextFilename))
            fileList << nextFilename;
        else
            break;
    } while (true);

    return fileList;
}

//---------------------------------------------------------------
bool ImageArray::loadFromDirectory()
{

    QDir dir(mSourceName);
    if (!dir.exists())
        return false;

    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);

    QFileInfoList list = dir.entryInfoList();
    mExternalFiles.clear();
    for (int i=0; i<list.count(); i++) {
        if (validateExternalImage(list[i].fileName()))
            mExternalFiles << dir.absolutePath() + dir.separator() + list[i].fileName();
    }

    return !mExternalFiles.isEmpty();
}

//---------------------------------------------------------------
//                         PRIVATE
//---------------------------------------------------------------
