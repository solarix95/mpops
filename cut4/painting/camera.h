#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
public:
    Camera(float x, float y, float z);

    virtual void move(float *p) = 0;

protected:
    float mZeroDist;  // mPos -> [0,0,0]
    float mPos[3];

};

#endif // CAMERA_H
