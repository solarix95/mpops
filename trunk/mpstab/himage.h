#ifndef HIMAGE_H
#define HIMAGE_H

#include <QImage>

class HImage
{
public:
    HImage();
    HImage(const QImage &img);
    HImage(const HImage &other);
    ~HImage();

    HImage & operator= (const HImage &other);

    inline int width() const   { return mWidth;  }
    inline int height() const  { return mHeight; }
    float hueAt(int x,int y) const;

private:
    void copy(const HImage &other);

    float *mValues;
    int    mWidth;
    int    mHeight;
};

#endif // HIMAGE_H
