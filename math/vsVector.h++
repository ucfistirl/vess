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
//    VESS Module:  vsVector.h++
//
//    Description:  Class representing a variable-sized array of double
//		    presicion values. The size of the vector is limited
//		    to four entries.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_VECTOR_HPP
#define VS_VECTOR_HPP

#include "vsGlobals.h++"

class vsVector
{
private:

    double      data[4];
    int         vecSize;

public:

                // Default vector size is 4
                vsVector();
                vsVector(int size);
                vsVector(int size, double values[]);
                vsVector(double x, double y);
                vsVector(double x, double y, double z);
                vsVector(double x, double y, double z, double w);
                ~vsVector();

    void        set(int size, double values[]);
    void        set(double x, double y);
    void        set(double x, double y, double z);
    void        set(double x, double y, double z, double w);
    void        copy(vsVector source);
    
    void        clear();
    void        clearCopy(vsVector source);

    void        setSize(int size);
    int         getSize();
    void        setValue(int index, int value);
    double      getValue(int index);
    int         isEqual(vsVector operand);
    int         isAlmostEqual(vsVector operand, double tolerance);

    void        add(vsVector addend);
    vsVector    getSum(vsVector addend);
    void        subtract(vsVector subtrahend);
    vsVector    getDifference(vsVector subtrahend);
    void        scale(double multiplier);
    vsVector    getScaled(double multiplier);
    
    double      getMagnitude();
    double      getDotProduct(vsVector operand);
    void        normalize();
    vsVector    getNormalized();
    void        crossProduct(vsVector operand);
    vsVector    getCrossProduct(vsVector operand);
    double      getAngleBetween(vsVector endVector);

    double      &operator[](int index);
    vsVector    operator+(vsVector addend);
    vsVector    operator-(vsVector subtrahend);
    vsVector    operator*(double multiplier);
    void        operator+=(vsVector addend);
    void        operator-=(vsVector subtrahend);
    void        operator*=(double multiplier);
    int         operator==(vsVector operand);
};

vsVector operator*(double multiplier, vsVector operand);

#endif
