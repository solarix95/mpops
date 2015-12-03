#ifndef LUTCORRECTION_H
#define LUTCORRECTION_H

#include <QString>
#include <QImage>
#include "cubelut.h"
#include "interpol.h"

class LutCorrection
{
public:
    LutCorrection(const char *filename = NULL);

    bool   loadLut(const char *filename);
    bool   isValid() const;
    void   correct(QImage &img) const;
    QColor correct(const QColor &col) const;

    void   initSelfTest();
private:
    void correct(float *c3) const;

    typedef struct {
        int   i[2];
        int   count;    // 1: i[0], 2: i[0] + i[1]
    } Indexes;

    void interpolate0d(float *c3, const Indexes &rIdxs, const Indexes &gIdxs, const Indexes &bIdxs) const;
    void interpolate1d(float *c3, const Indexes &rIdxs, const Indexes &gIdxs, const Indexes &bIdxs) const;
    void interpolate2d(float *c3, const Indexes &rIdxs, const Indexes &gIdxs, const Indexes &bIdxs) const;
    void interpolate3d(float *c3, const Indexes &rIdxs, const Indexes &gIdxs, const Indexes &bIdxs) const;

    void  searchAxisIndexes(float color, Indexes &indexes) const;
    float normalized(float color, int lowerIndex) const;

    CubeLUT             mCube;
    Interpolator<float> mInterpol;
};

#endif // LUTCORRECTION_H
