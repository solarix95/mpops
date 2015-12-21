#ifndef C4SHAPE_H
#define C4SHAPE_H

class C4Painter;

class C4Shape
{
public:
    C4Shape();
    virtual ~C4Shape();

    int  id() const;

    // frustum-check
    virtual const float *pos() const;
    virtual float radius() const  = 0;

    // Shape tranforms
    //
    virtual void scale(float dx, float dy, float dz);
    virtual void rotate(float rx, float ry, float rz);
    virtual void setPos(float x, float y, float z);

    // do the paintings..
    void  render(C4Painter *painter, int frame);

    // Aliases
    void rotate(float *r3);
    void setPos(float *p3);
    void scale(float  *d3);

protected:
    virtual void shapeRender(C4Painter *painter,int frame) = 0;

private:
    float mPos[3];
    float mRot[3];
    float mScale[3];
    int   mId;

    static int sNextId;
};

#endif // C4SHAPE_H
