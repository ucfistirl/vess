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

#include <stdio.h>
#include "vsGlobals.h++"
#include "vsVector.h++"
#include "vsQuat.h++"

class VS_UTIL_DLL vsMatrix
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
    void        copy(const vsMatrix &source);

    void        clear();
    
    void        setValue(int row, int column, double value);
    double      getValue(int row, int column) const;
    bool        isEqual(const vsMatrix &operand) const;
    bool        isAlmostEqual(const vsMatrix &operand, double tolerance) const;

    void        add(const vsMatrix &addend);
    vsMatrix    getSum(const vsMatrix &addend) const;
    void        subtract(const vsMatrix &subtrahend);
    vsMatrix    getDifference(const vsMatrix &subtrahend) const;
    void        scale(double multiplier);
    vsMatrix    getScaled(double multiplier) const;
    void        transpose();
    vsMatrix    getTranspose() const;
    
    double      getDeterminant() const;
    void        invert();
    vsMatrix    getInverse() const;
    
    void        preMultiply(const vsMatrix &operand);
    vsMatrix    getPreMultiplied(const vsMatrix &operand) const;
    void        postMultiply(const vsMatrix &operand);
    vsMatrix    getPostMultiplied(const vsMatrix &operand) const;
    
    vsVector    getPointXform(const vsVector &operand) const;
    vsVector    getVectorXform(const vsVector &operand) const;
    vsVector    getFullXform(const vsVector &operand) const;
    
    void        setIdentity();
    void        setEulerRotation(vsMathEulerAxisOrder axisOrder,
                                 double axis1Degrees, double axis2Degrees,
                                 double axis3Degrees);
    void        getEulerRotation(vsMathEulerAxisOrder axisOrder,
                                 double *axis1Degrees, double *axis2Degrees,
                                 double *axis3Degrees) const;
    void        setQuatRotation(const vsQuat &quat);
    void        setTranslation(double dx, double dy, double dz);
    void        setScale(double sx, double sy, double sz);

    vsVector    &operator[](int index);
    vsMatrix    operator+(const vsMatrix &addend) const;
    vsMatrix    operator-(const vsMatrix &subtrahend) const;
    vsMatrix    operator*(const vsMatrix &operand) const;
    void        operator+=(const vsMatrix &addend);
    void        operator-=(const vsMatrix &subtrahend);
    bool        operator==(const vsMatrix &operand) const;

    void        printRow(int rowNum) const;
    void        printRow(int rowNum, FILE *fp) const;

    void        print() const;
    void        print(FILE *fp) const;
};

#endif
