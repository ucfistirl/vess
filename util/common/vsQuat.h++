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
//    VESS Module:  vsQuat.h++
//
//    Description:  Class implementing a quaternion used to store graphics
//		    rotations
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_QUAT_HPP
#define VS_QUAT_HPP

class vsQuat;

#include "vsMatrix.h++"

class VS_UTIL_DLL vsQuat
{
private:

    // Quaternion data is represented with vector portion first
    // [x, y, z, w] => [V, w]
    double      data[4];

public:

                vsQuat();
                vsQuat(double x, double y, double z, double w);
                vsQuat(const double values[]);
                ~vsQuat();

    void        set(double x, double y, double z, double w);
    void        set(const double values[]);
    void        copy(const vsQuat &source);
    void        clear();
    
    void        setValue(int index, double value);
    double      getValue(int index) const;
    bool        isEqual(const vsQuat &operand) const;
    bool        isAlmostEqual(const vsQuat &operand, double tolerance) const;

    void        add(const vsQuat &addend);
    vsQuat      getSum(const vsQuat &addend) const;
    void        subtract(const vsQuat &subtrahend);
    vsQuat      getDifference(const vsQuat &subtrahend) const;
    void        scale(double multiplier);
    vsQuat      getScaled(double multiplier) const;
    void        multiplyQuat(const vsQuat &operand);
    vsQuat      getMultipliedQuat(const vsQuat &operand) const;
    
    double      getMagnitude() const;
    double      getMagnitudeSquared() const;
    double      getDotProduct(const vsQuat &operand) const;
    vsQuat      getNormalized() const;
    void        normalize();
    void        conjugate();
    vsQuat      getConjugate() const;
    void        invert();
    vsQuat      getInverse() const;
    
    void        setMatrixRotation(const vsMatrix &theMatrix);
    void        setEulerRotation(vsMathEulerAxisOrder axisOrder,
                    double axis1Degrees, double axis2Degrees,
                    double axis3Degrees);
    void        getEulerRotation(vsMathEulerAxisOrder axisOrder,
                    double *axis1Degrees, double *axis2Degrees,
                    double *axis3Degrees) const;
    void        setAxisAngleRotation(double x, double y, double z,
                    double rotDegrees);
    void        getAxisAngleRotation(double *x, double *y, double *z,
                    double *rotDegrees) const;
    void        setVecsRotation(const vsVector &originForward,
                    const vsVector &originUp,
                    const vsVector &targetForward,
                    const vsVector &targetUp);

    vsVector    rotatePoint(const vsVector &targetPoint) const;

    vsQuat      slerp(const vsQuat &destination, double parameter) const;
    
    double      &operator[](int index);
    vsQuat      operator+(const vsQuat &addend) const;
    vsQuat      operator-(const vsQuat &subtrahend) const;
    vsQuat      operator*(const vsQuat &operand) const;
    void        operator+=(const vsQuat &addend);
    void        operator-=(const vsQuat &subtrahend);
    void        operator*=(const vsQuat &operand);
    bool        operator==(const vsQuat &operand) const;

    void        print() const;
    void        print(FILE *fp) const;
};

vsQuat operator*(double multiplier, vsQuat operand);

#endif
