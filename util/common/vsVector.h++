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

#include <stdio.h>
#include "vsGlobals.h++"

class VS_UTIL_DLL vsVector
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
    void        copy(const vsVector &source);
    
    void        clear();
    void        clearCopy(const vsVector &source);

    void        setSize(int size);
    int         getSize() const;
    void        setValue(int index, double value);
    double      getValue(int index) const;
    bool        isEqual(const vsVector &operand) const;
    bool        isAlmostEqual(const vsVector &operand, double tolerance) const;

    void        add(const vsVector &addend);
    vsVector    getSum(const vsVector &addend) const;
    void        subtract(const vsVector &subtrahend);
    vsVector    getDifference(const vsVector &subtrahend) const;
    void        scale(double multiplier);
    vsVector    getScaled(double multiplier) const;
    
    double      getMagnitude() const;
    double      getMagnitudeSquared() const;
    double      getDotProduct(const vsVector &operand) const;
    void        normalize();
    vsVector    getNormalized() const;
    void        crossProduct(const vsVector &operand);
    vsVector    getCrossProduct(const vsVector &operand) const;
    double      getAngleBetween(const vsVector &endVector) const;

    double      &operator[](int index);
    vsVector    operator+(const vsVector &addend) const;
    vsVector    operator-(const vsVector &subtrahend) const;
    vsVector    operator*(double multiplier) const;
    void        operator+=(const vsVector &addend);
    void        operator-=(const vsVector &subtrahend);
    void        operator*=(double multiplier);
    bool        operator==(const vsVector &operand) const;

    void        print() const;
    void        print(FILE *fp) const;
};

vsVector operator*(double multiplier, vsVector operand);

#endif
