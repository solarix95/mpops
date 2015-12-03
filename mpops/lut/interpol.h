#ifndef INTERPOL_H
#define INTERPOL_H

/*
  Source: https://cg2010studio.wordpress.com/2013/03/05/cc-linear-bilinear-trilinear/


#include "interpol.h"
#include <iostream>

using namespace std;

int main()
{
    // Test Linear interpolation
    float fVarsLin[2] = {1.0, 2.0};
    int iVarsLin[2] = {100, 200};

    float targetLin = 0.5;

    cout<<"Linear (f): "<<Interpolator<float>::Linear(targetLin, fVarsLin)<<endl;
    cout<<"Linear (i): "<<Interpolator<int>::Linear(targetLin, iVarsLin)<<endl;

    // Test Bilinear interpolation
    float fVarsBilin[4] = {1.0, 2.0, 3.0, 4.0};
    int iVarsBilin[4] = {100, 200, 300, 400};

    float targetBilin[2] = {0.5, 0.5};

    cout<<"Bilinear (f): "<<Interpolator<float>::Bilinear(targetBilin, fVarsBilin)<<endl;
    cout<<"Bilinear (i): "<<Interpolator<int>::Bilinear(targetBilin, iVarsBilin)<<endl;

    // Test Trilinear interpolation
    float fVarsTrilin[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    int iVarsTrilin[8] = {100, 200, 300, 400, 500, 600, 700, 800};

    float targetTrilin[3] = {0.5, 0.5, 0.5};

    cout<<"Trilinear (f): "<<Interpolator<float>::Trilinear(targetTrilin, fVarsTrilin)<<endl;
    cout<<"Trilinear (i): "<<Interpolator<int>::Trilinear(targetTrilin, iVarsTrilin)<<endl;

    return 0;
}

*/

template<class T> class Interpolator
{
public:
    /*
        Linear interpolation
        target  - the target point, 0.0 - 1.0
        v       - a pointer to an array of size 2 containg the two values
    */
    inline static T Linear(float target, T *v)
    {
        return (T)(target*(v[1])+ (T(1.0f) - target)*(v[0]));
    }

    /*
        BiLinear interpolation, linear interpolation in 2D
        target  - a 2D point (X,Y) [0..1, 0..1]
        v       - an array of size 4 containg values around the square

              v[1]--v[3]
                |    |
              v[0]--v[2]

        cost: performs 3 linear interpolations
    */
    inline static T Bilinear(float *target, T *v)
    {
        T v_prime[2] = {
                        Linear(target[1], &(v[0])),
                        Linear(target[1], &(v[2]))
                       };

        return Linear(target[0], v_prime);

    }

    /*
        TriLinear interpolation, linear interpolation in 2D
        target  - a 3D point (X,Y,Z) [0..1, 0..1, 0..1]
        v       - an array of size 8 containg the values of the 8 corners
                    of a cube defined as two faces: 0-3 face one (front face)
                                                    4-7 face two (back face)

        cost: 7 linear interpolations
    */
    inline static T Trilinear(float *target, T *v)
    {
        T v_prime[2] = {
                         Bilinear(&(target[0]), &(v[0])),
                         Bilinear(&(target[1]), &(v[4]))
                       };
        return Linear(target[2], v_prime);
    }
};

#endif // INTERPOL_H
