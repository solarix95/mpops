#ifndef IMAGELOADERJOB_H
#define IMAGELOADERJOB_H

#include <QObject>
#include "shared/image.h"

class ImageLoaderJob : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoaderJob(int index, ImagePtr img);
    virtual ~ImageLoaderJob();

    void start();

signals:
    void loaded(int index, ImagePtr img);
    void startJob();

private slots:
    void loadImage();

private:
    int      mIndex;
    ImagePtr mImage;
};

#endif // IMAGELOADERJOB_H
