#ifndef IMAGEARRAY_H
#define IMAGEARRAY_H

#include <QString>
#include <QStringList>
#include "shared/image.h"

#ifdef WITH_OPENCV
#include <opencv2/opencv.hpp>
#endif
/*
  Intro

  An "ImageArray" is a generic "VideoSource" with a "frame-by-frame" access. Every returned "ImagePtr"
  must be loaded ("load"-Method) before the first use. By this way, an Image frame can be prepared threaded.
  In case of a video file, this doesn't make any sense. But for external (Timelapse)Images, this boots the full
  application because the File-loader and -Interpreter can run threaded.

  Usage:
  ImageArray arr;

  arr.setVideoSource("frame_%d.png");             // ffmpeg/avconv filelist..

  arr.setVideoSource("video.mpg");                // videosource

  or

  foreach(programmargument)
    arr.appendSingleFrame(programmargument)       // "tool *.jpg" for timelapses

*/

class ImageArray
{
public:
    typedef enum eSource
    {
        Uninitialized,
        ImageList,      // frame1.png frame2.png frame...
        VideoFile       // video.mpg
    } Source;

    ImageArray(const QString &videosource = "");
    ~ImageArray();

    void setVideoSource(const QString &videosource);
    bool appendSingleFrame(const QString &externalFileName); // only for source "ImageList"!
    bool appendSingleFrames(const QStringList &externalFileNames); // only for source "ImageList"!
    void setMaxWidthHint(int maxWidth);

    void     reset();
    bool     isValid() const;
    Source   source() const;
    ImagePtr nextFrame(bool *hasNext = NULL);
    int      nextIndex() const;
    bool     skipFrame();
    qint64   frameCount() const;

    static bool        validateExternalImage(const QString &fileName);
    static QStringList expandTemplateName(const QString &templateName);

private:
    bool        loadFromDirectory();

    QString     mSourceName;
    Source      mSource;
    bool        mIsValid;
    int         mFrameIndex;
    QStringList mExternalFiles;
    int         mMaxWidth;

#ifdef WITH_OPENCV
    CvCapture* mCvCapture;
    IplImage*  mCvFrame;
    int        mCvFrameCount;
#endif
};


#endif // IMAGEARRAY_H
