// File vsQuat.h++

#ifndef VS_QUAT_HPP
#define VS_QUAT_HPP

class vsQuat;

#include "vsMatrix.h++"

class vsQuat
{
private:

    // Quaternion data is represented with vector portion first
    // [x, y, z, w] => [V, w]
    double      data[4];

public:

                vsQuat();
                vsQuat(double x, double y, double z, double w);
                vsQuat(double values[]);
                ~vsQuat();

    void        set(double x, double y, double z, double w);
    void        set(double values[]);
    void        copy(vsQuat source);
    void        clear();
    
    void        setValue(int index, double value);
    double      getValue(int index);
    int         isEqual(vsQuat operand);
    int         isAlmostEqual(vsQuat operand, double tolerance);

    void        add(vsQuat addend);
    vsQuat      getSum(vsQuat addend);
    void        subtract(vsQuat subtrahend);
    vsQuat      getDifference(vsQuat subtrahend);
    void        scale(double multiplier);
    vsQuat      getScaled(double multiplier);
    void        multiplyQuat(vsQuat operand);
    vsQuat      getMultipliedQuat(vsQuat operand);
    
    double      getMagnitude();
    vsQuat      getNormalized();
    void        normalize();
    void        conjugate();
    vsQuat      getConjugate();
    void        invert();
    vsQuat      getInverse();
    
    void        setMatrixRotation(vsMatrix theMatrix);
    void        setEulerRotation(vsMathEulerAxisOrder axisOrder,
                    double axis1Degrees, double axis2Degrees,
                    double axis3Degrees);
    void        getEulerRotation(vsMathEulerAxisOrder axisOrder,
                    double *axis1Degrees, double *axis2Degrees,
                    double *axis3Degrees);
    void        setAxisAngleRotation(double x, double y, double z,
                    double rotDegrees);
    void        setVecsRotation(vsVector originForward, vsVector originUp,
                    vsVector targetForward, vsVector targetUp);

    vsVector    rotatePoint(vsVector targetPoint);

    vsQuat      slerp(vsQuat destination, double parameter);
    
    double      &operator[](int index);
    vsQuat      operator+(vsQuat addend);
    vsQuat      operator-(vsQuat subtrahend);
    vsQuat      operator*(double multiplier);
    vsQuat      operator*(vsQuat operand);
    void        operator+=(vsQuat addend);
    void        operator-=(vsQuat subtrahend);
    void        operator*=(double multiplier);
    void        operator*=(vsQuat operand);
    int         operator==(vsQuat operand);
};

vsQuat operator*(double multiplier, vsQuat operand);

#endif
