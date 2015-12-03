#include "lut/lutcorrection.h"
#include <QColor>
#include <QDebug>

#define TEST_RGB(c3)   Q_ASSERT(c3[0] <= 1.0 && c3[0] >= 0 && c3[1] <= 1.0 && c3[1] >= 0 && c3[2] <= 1.0 && c3[2] >= 0)

//-----------------------------------------------------------------------------
LutCorrection::LutCorrection(const char *filename)
{
    if (filename)
        loadLut(filename);
}

//-----------------------------------------------------------------------------
bool LutCorrection::loadLut(const char *filename)
{
    ifstream infile (filename);
    if ( !infile.good() )
        return false;

    int ret = mCube.LoadCubeFile ( infile );
    if ( ret != CubeLUT::OK )
        return false;

    return true;
}

//-----------------------------------------------------------------------------
bool LutCorrection::isValid() const
{
    return mCube.status == CubeLUT::OK;
}

//-----------------------------------------------------------------------------
void LutCorrection::correct(QImage &img) const
{
    if (mCube.status != CubeLUT::OK)
        return;

    if (img.isNull())
        return;

    float nextColor[3];
    for (int y=0; y<img.height(); y++) {
        const uchar *line = img.scanLine(y);
        const QRgb  *pixel = (const QRgb  *)line;
        for (int x = 0; x<img.width(); x++) {
            nextColor[0] = qRed(*pixel)/255.0;
            nextColor[1] = qGreen(*pixel)/255.0;
            nextColor[2] = qBlue(*pixel)/255.0;
            correct(nextColor);
            pixel++;
            img.setPixel(x,y,QColor(nextColor[0]*255,nextColor[1]*255,nextColor[2]*255).rgb());
        }
    }

    return;
}

//-----------------------------------------------------------------------------
QColor LutCorrection::correct(const QColor &col) const
{
    if (mCube.status != CubeLUT::OK)
        return col;

    float c3[] = { col.redF(), col.greenF(), col.blueF() };
    correct(c3);

    return QColor(c3[0]*255 + 0.5, c3[1]*255 + 0.5, c3[2]*255 + 0.5);
}

//-----------------------------------------------------------------------------
void LutCorrection::initSelfTest()
{
    mCube.initSelfTest();
}

//-----------------------------------------------------------------------------
void LutCorrection::correct(float *c3) const
{
    int N = mCube.LUT3D.size(); // dots per dimension
    Q_ASSERT(N >= 2);           // only 3D-LUTs supportet

    Indexes redIndexes;
    searchAxisIndexes(c3[0],redIndexes);

    Indexes greenIndexes;
    searchAxisIndexes(c3[1],greenIndexes);

    Indexes blueIndexes;
    searchAxisIndexes(c3[2],blueIndexes);

    int pointCount = redIndexes.count * greenIndexes.count * blueIndexes.count;

    switch (pointCount) {
        case 1 : interpolate0d(c3, redIndexes, greenIndexes, blueIndexes); return; // single dot
        case 2 : interpolate1d(c3, redIndexes, greenIndexes, blueIndexes); return; // line
        case 4 : interpolate2d(c3, redIndexes, greenIndexes, blueIndexes); return; // 2d quad
        case 8 : interpolate3d(c3, redIndexes, greenIndexes, blueIndexes); return; // 3d cube
    }
    Q_ASSERT(0); // never reach this point!
}

//-----------------------------------------------------------------------------
void LutCorrection::interpolate0d(float *c3, const Indexes &rIdxs, const Indexes &gIdxs, const Indexes &bIdxs) const
{
    // exact match, no "interpolation": take new color direct from LUT:
    Q_ASSERT(rIdxs.count == 1);
    Q_ASSERT(gIdxs.count == 1);
    Q_ASSERT(bIdxs.count == 1);
    c3[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][0];
    c3[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][1];
    c3[2] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][2];
    TEST_RGB(c3);
}

//-----------------------------------------------------------------------------
void LutCorrection::interpolate1d(float *c3, const Indexes &rIdxs, const Indexes &gIdxs, const Indexes &bIdxs) const
{
    // interpolate line v1 -> v2
    float values[2];

    if (rIdxs.count == 2) {        // line "red"
        Q_ASSERT(gIdxs.count == 1);
        Q_ASSERT(bIdxs.count == 1);
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][0];
        values[1] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[0]][0];

        c3[0] = mInterpol.Linear(normalized(c3[0],rIdxs.i[0]),values);
        c3[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][1];
        c3[2] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][2];
        TEST_RGB(c3);
    } else if (gIdxs.count == 2) { // line "green"
        Q_ASSERT(rIdxs.count == 1);
        Q_ASSERT(bIdxs.count == 1);
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][1];
        values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[0]][1];

        c3[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][0];
        c3[1] = mInterpol.Linear(normalized(c3[1],gIdxs.i[0]),values);
        c3[2] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][2];
        TEST_RGB(c3);
    } else {                         // line "blue"
        Q_ASSERT(rIdxs.count == 1);
        Q_ASSERT(gIdxs.count == 1);
        Q_ASSERT(bIdxs.count == 2);
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][2];
        values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[1]][2];

        c3[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][0];
        c3[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][1];
        c3[2] = mInterpol.Linear(normalized(c3[2],bIdxs.i[0]),values);

        TEST_RGB(c3);
    }
}

//-----------------------------------------------------------------------------
void LutCorrection::interpolate2d(float *c3, const Indexes &rIdxs, const Indexes &gIdxs, const Indexes &bIdxs) const
{
    /*
      interpolate 2d quad

      v2   -   v4

      |        |

      v1   -   v3

    */

    float  target[2];
    float  values[4];

    if (rIdxs.count == 1) {             // quad green/blue

        // don't interpolate red:
        c3[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][0];

        target[0] = normalized(c3[1],gIdxs.i[0]);
        target[1] = normalized(c3[2],bIdxs.i[0]);

        // interpolate green
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][1];
        values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[1]][1];
        values[2] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[0]][1];
        values[3] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[1]][1];
        c3[1] = mInterpol.Bilinear(target,values);

        // interpolate blue
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][2];
        values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[1]][2];
        values[2] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[0]][2];
        values[3] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[1]][2];
        // qDebug() << values[0] << values[1] << values[2] << values[3] << target[0] << target[1];
        c3[2] = mInterpol.Bilinear(target,values);

        TEST_RGB(c3);

    } else if (gIdxs.count == 1) {      // quad red/blue

        // don't interpolate green:
        c3[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][1];

        target[0] = normalized(c3[0],rIdxs.i[0]);
        target[1] = normalized(c3[2],bIdxs.i[0]);

        // interpolate red
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][0];
        values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[1]][0];
        values[2] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[0]][0];
        values[3] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[1]][0];
        c3[0] = mInterpol.Bilinear(target,values);

        // interpolate blue
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][2];
        values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[1]][2];
        values[2] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[0]][2];
        values[3] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[1]][2];
        c3[2] = mInterpol.Bilinear(target,values);
        TEST_RGB(c3);

    } else {
        Q_ASSERT(bIdxs.count == 1);    // quad red/green

        // don't interpolate blue:
        c3[2] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][2];

        target[0] = normalized(c3[0],rIdxs.i[0]);
        target[1] = normalized(c3[1],gIdxs.i[0]);

        // interpolate red
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][0];
        values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[0]][0];
        values[2] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[0]][0];
        values[3] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[1]][bIdxs.i[0]][0];
        c3[0] = mInterpol.Bilinear(target,values);


        // interpolate green
        values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][1];
        values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[0]][1];
        values[2] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[0]][1];
        values[3] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[1]][bIdxs.i[0]][1];
        c3[1] = mInterpol.Bilinear(target,values);

        TEST_RGB(c3);
    }

}

//-----------------------------------------------------------------------------
void LutCorrection::interpolate3d(float *c3, const Indexes &rIdxs, const Indexes &gIdxs, const Indexes &bIdxs) const
/*
  interpolate 3d cube

      v6  -   v7
     /
    /  |      |
  v2   -   v3
      v5  -  v8
  |  /     |  /
    /        /
  v1   -   v4

*/
{
    float  target[3];
    float  values[8];
    float  result[3];

    // red xyz:
    target[0] = normalized(c3[2],bIdxs.i[0]);
    target[1] = normalized(c3[1],gIdxs.i[0]);
    target[2] = normalized(c3[0],rIdxs.i[0]);

    // red front
    values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][0];
    values[1] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[1]][0];
    values[2] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[1]][0];
    values[3] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[0]][0];

    // red back
    values[4] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[0]][0];
    values[5] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[1]][0];
    values[6] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[1]][bIdxs.i[1]][0];
    values[7] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[1]][bIdxs.i[0]][0];

    // qDebug() << "UML" << values[0] << values[1] << values[2] << values[3] << values[4] << values[5] << values[6] << values[7];
    // qDebug() << "TAG" << target[0] << target[1] << target[2];

    float debug;
    result[0] = mInterpol.Trilinear(target,values);

    // green xyz:
    target[0] = normalized(c3[0],rIdxs.i[0]);
    target[1] = normalized(c3[2],bIdxs.i[0]);
    target[2] = normalized(c3[1],gIdxs.i[0]);

    // green front
    values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][1];
    values[1] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[0]][1];
    values[2] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[1]][1];
    values[3] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[1]][1];

    // green back
    values[4] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[0]][1];
    values[5] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[1]][bIdxs.i[0]][1];
    values[6] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[1]][bIdxs.i[1]][1];
    values[7] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[1]][1];

    // qDebug() << "UML" << values[0] << values[1] << values[2] << values[3] << values[4] << values[5] << values[6] << values[7];
    // qDebug() << "TAG" << target[0] << target[1] << target[2];

    result[1] = mInterpol.Trilinear(target,values);
    debug = c3[1];

    // blue xyz:
    target[0] = normalized(c3[0],rIdxs.i[0]);
    target[1] = normalized(c3[1],gIdxs.i[0]);
    target[2] = normalized(c3[2],bIdxs.i[0]);

    // blue front
    values[0] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[0]][2];
    values[1] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[0]][2];
    values[2] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[1]][bIdxs.i[0]][2];
    values[3] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[0]][2];

    // blue back
    values[4] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[0]][bIdxs.i[1]][2];
    values[5] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[0]][bIdxs.i[1]][2];
    values[6] = mCube.LUT3D[rIdxs.i[1]][gIdxs.i[1]][bIdxs.i[1]][2];
    values[7] = mCube.LUT3D[rIdxs.i[0]][gIdxs.i[1]][bIdxs.i[1]][2];

    // qDebug() << "UML" << values[0] << values[1] << values[2] << values[3] << values[4] << values[5] << values[6] << values[7];
    // qDebug() << "TAG" << target[0] << target[1] << target[2];

    result[2] = mInterpol.Trilinear(target,values);

    c3[0] = result[0];
    c3[1] = result[1];
    c3[2] = result[2];
    TEST_RGB(c3);
    // qDebug() << "UML" << values[0] << values[1] << values[2] << values[3] << values[4] << values[5] << values[6] << values[7];
    // qDebug() << "TAG" << target[0] << target[1] << target[2];
}

//-----------------------------------------------------------------------------
void LutCorrection::searchAxisIndexes(float color, Indexes &indexes) const
{
    int   tableDim  = mCube.dimension();
    float step      = mCube.step();
    float nextPoint = 0;
    indexes.count   = 0;
    for (int i=0; i<tableDim; i++) {
        if (color == nextPoint) {
            indexes.i[0]  = i;
            indexes.count = 1;
            break;
        }
        if ((color > nextPoint) && (color < (nextPoint+step))) {
            indexes.i[0] = i;
            indexes.i[1] = i+1;
            indexes.count = 2;
            break;
        }
        nextPoint += step;
    }
}

//-----------------------------------------------------------------------------
float LutCorrection::normalized(float color, int lowerIndex) const
/*
                    lowerIndex : 2
                           |
                          \/
     Index 0      1       2       3
     Value 0     30      60      90
                  |_______|
                   step: 30
                             |
    color:                  70

    normalized color (0..1): (70 - 60)/30 = 0.3333
*/
{
    // qDebug() << color << lowerIndex << mCube.step();
    Q_ASSERT((color-(lowerIndex * mCube.step())) >= 0);
    return (color-(lowerIndex * mCube.step()))/mCube.step();
}

