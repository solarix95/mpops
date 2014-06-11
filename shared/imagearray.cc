
#include <QFile>
#include <QImageReader>
#include <QColor>
#include <QImage>
#include <QDebug>
#include "shared/imagearray.h"
#include "shared/fileimage.h"

//---------------------------------------------------------------
ImageArray::ImageArray(const QString &videosource)
{
    mIsValid           = false;
    mFrameIndex        = -1;
    mSource            = Uninitialized;

#ifdef WITH_OPENCV
    mCvCapture         = NULL;
    mCvFrame           = NULL;
    mCvFrameCount      = NULL;
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
    qDebug() << videosource;
    if (videosource.isEmpty())
        return;

    if (validateExternalImage(videosource)) {
        mSource        = ImageList;
        mExternalFiles = videosource.contains('%') ? expandTemplateName(videosource) : QStringList() << videosource;
        mFrameIndex    = 0;
        mIsValid       = !mExternalFiles.isEmpty();
    } else {
#ifdef WITH_OPENCV
        qDebug() << "OPENCV";
        mSource       = VideoFile;
        mCvCapture    = cvCreateFileCapture(videosource.toAscii().data());
        mFrameIndex   = 0;
        mIsValid      = mCvCapture ? true:false;
        mCvFrameCount =  mCvCapture ? (int)cvGetCaptureProperty(mCvCapture,CV_CAP_PROP_FRAME_COUNT) : 0;
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
        qDebug() << nextFrame;
        if (!appendSingleFrame(nextFrame))
            return false;
    }
    return true;
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
              img = ImagePtr(new FileImage(new QImage,mExternalFiles[mFrameIndex]));
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

              int nc  = mCvFrame->nChannels;
              int step= mCvFrame->widthStep;

              unsigned char *data= reinterpret_cast<unsigned char *>
                                                     (mCvFrame->imageData);

              img = ImagePtr(new FileImage(new QImage(mCvFrame->width,mCvFrame->height,QImage::Format_RGB32),QString("Frame_%1").arg(mFrameIndex)));
              for (int y=0; y<mCvFrame->height; y++) {
                  for (int x=0; x<mCvFrame->width; x++) {
                      int firstIndex = y*step + x*nc;
                      // BGR, not RGB:
                      uchar blue  = data[firstIndex + 0];
                      uchar green = data[firstIndex + 1];
                      uchar red   = data[firstIndex + 2];
                      img->img()->setPixel(x,y,qRgb(red,green,blue));
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
//                         PRIVATE
//---------------------------------------------------------------
