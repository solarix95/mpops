#ifndef PAINTER_H
#define PAINTER_H

#include <QColor>
#include <QList>
#include <QImage>
class C4FrameBuffer;
class Transformation;
class Camera;
class C4Painter
{
public:
    C4Painter(C4FrameBuffer *buffer, Camera *camera = NULL);
    virtual ~C4Painter();

    void rotate(float *r);
    void rotate(float ax, float ay, float az);
    void translate(float *to);
    void translate(float dx, float dy, float dz);
    void scale(float *f);
    void scale(float fx, float fy, float fz);

    void setColor(const QColor &c);
    void setColor(QRgb c);
    void setColorMask(bool red, bool green, bool blue);
    void setColorMatrix(float red1, float red2, float red3,
                        float green1, float green2, float green3,
                        float blue1, float blue2, float blue3);

    void mapPoint(const float *from, float *to);
    void mapPoint(float x, float y, float z, float *to);
    float  pixelPerPoint(const float *p1, const float *p2) const;
    float  pixelPerPoint(float x1, float y1, float z1, float x2, float y2, float z2) const;
    void drawPoint(float *p);
    void drawPoint(float x, float y, float z);
    void drawImage(const QImage &img);
    void drawLine(float *p1, float *p2);
    void drawLine(float p1x, float p1y, float p1z, float p2x, float p2y, float p2z);

    void push();
    void pop();


private:


    // ------------------------------------------------------------------------
    struct State {
        QColor color;
        float  colorMatrix[9];  // rgba
        int    stackDepth;
        QColor getColor() const {
            int r = color.red();
            int g = color.green();
            int b = color.blue();
            const float *m = colorMatrix;

            return QColor(r*m[0] + g*m[1] + b*m[2],r*m[3] + g*m[4] + b*m[5],r*m[6] + g*m[7] + b*m[8]);
        }
        State() : color(Qt::black), stackDepth(-1)
            {
                colorMatrix[0] = 1;
                colorMatrix[1] = 0;
                colorMatrix[2] = 0;
                colorMatrix[3] = 0;
                colorMatrix[4] = 1;
                colorMatrix[5] = 0;
                colorMatrix[6] = 0;
                colorMatrix[7] = 0;
                colorMatrix[8] = 1;
        }
    };

    C4FrameBuffer         *mBuffer;
    Camera                *mCamera;
    State                  mState;
    QList<State>           mStack;
    QList<Transformation*> mTransforms;
};

#endif // PAINTER_H
