//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsMatrix.h++
//
//    Description:  Class implementing a 4x4 graphical transformation
//		    matrix
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_MATRIX_HPP
#define VS_MATRIX_HPP

class vsMatrix;

#include "vsGlobals.h++"
#include "vsVector.h++"
#include "vsQuat.h++"

class vsMatrix
{
private:

    // Internal storage for this object is a set of four vsVectors
    // representing the four rows of the matrix.
    vsVector    data[4];

public:

                vsMatrix();
                vsMatrix(double values[4][4]);
    virtual     ~vsMatrix();

    void        set(double values[4][4]);
    void        copy(vsMatrix source);

    void        clear();
    
    void        setValue(int row, int column, double value);
    double      getValue(int row, int column);
    int         isEqual(vsMatrix operand);
    int         isAlmostEqual(vsMatrix operand, double tolerance);

    void        add(vsMatrix addend);
    vsMatrix    getSum(vsMatrix addend);
    void        subtract(vsMatrix subtrahend);
    vsMatrix    getDifference(vsMatrix subtrahend);
    void        scale(double multiplier);
    vsMatrix    getScaled(double multiplier);
    void        transpose();
    vsMatrix    getTranspose();
    
    double      getDeterminant();
    void        invert();
    vsMatrix    getInverse();
    
    void        preMultiply(vsMatrix operand);
    vsMatrix    getPreMultiplied(vsMatrix operand);
    void        postMultiply(vsMatrix operand);
    vsMatrix    getPostMultiplied(vsMatrix operand);
    
    vsVector    getPointXform(vsVector operand);
    vsVector    getVectorXform(vsVector operand);
    vsVector    getFullXform(vsVector operand);
    
    void        setIdentity();
    void        setEulerRotation(vsMathEulerAxisOrder axisOrder,
                                 double axis1Degrees, double axis2Degrees,
                                 double axis3Degrees);
    void        getEulerRotation(vsMathEulerAxisOrder axisOrder,
                                 double *axis1Degrees, double *axis2Degrees,
                                 double *axis3Degrees);
    void        setQuatRotation(vsQuat quat);
    void        setTranslation(double dx, double dy, double dz);
    void        setScale(double sx, double sy, double sz);

    vsVector    &operator[](int index);
    vsMatrix    operator+(vsMatrix addend);
    vsMatrix    operator-(vsMatrix subtrahend);
    vsMatrix    operator*(vsMatrix operand);
    void        operator+=(vsMatrix addend);
    void        operator-=(vsMatrix subtrahend);
    int         operator==(vsMatrix operand);
};

#endif
