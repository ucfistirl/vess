// File vsVector.c++

#include <stdio.h>
#include <math.h>
#include "vsVector.h++"

// ------------------------------------------------------------------------
// Default constructor - Clears the vector to zero and sets the size to 4
// ------------------------------------------------------------------------
vsVector::vsVector()
{
    clear();
    setSize(4);
}

// ------------------------------------------------------------------------
// Size constructor - Clears the vector to zero and sets the size to the
// indicated value, if legal.
// ------------------------------------------------------------------------
vsVector::vsVector(int size)
{
    clear();
    setSize(4);
    setSize(size);
}

// ------------------------------------------------------------------------
// Data constructor - Sets the size of the vector to the size of the array,
// and sets the vector data to the array data.
// ------------------------------------------------------------------------
vsVector::vsVector(int size, double values[])
{
    clear();
    setSize(4);
    set(size, values);
}

// ------------------------------------------------------------------------
// Data constructor - Sets the size of the vector to 2, and sets the
// vector data to the given data.
// ------------------------------------------------------------------------
vsVector::vsVector(double x, double y)
{
    set(x, y);
}

// ------------------------------------------------------------------------
// Data constructor - Sets the size of the vector to 3, and sets the
// vector data to the given data.
// ------------------------------------------------------------------------
vsVector::vsVector(double x, double y, double z)
{
    set(x, y, z);
}

// ------------------------------------------------------------------------
// Data constructor - Sets the size of the vector to 4, and sets the
// vector data to the given data.
// ------------------------------------------------------------------------
vsVector::vsVector(double x, double y, double z, double w)
{
    set(x, y, z, w);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsVector::~vsVector()
{
}

// ------------------------------------------------------------------------
// Sets the size of this vector to the given size (if legal), and copies
// the data from the specified array into the vector.
// ------------------------------------------------------------------------
void vsVector::set(int size, double values[])
{
    if ((size < 1) || (size > 4))
    {
        printf("vsVector::set(int, double[]): Invalid size parameter\n");
        return;
    }

    clear();
    setSize(size);
    for (int i = 0; i < size; i++)
        data[i] = values[i];
}

// ------------------------------------------------------------------------
// Sets the size of the vector to 2, and sets the vector data to the given
// data.
// ------------------------------------------------------------------------
void vsVector::set(double x, double y)
{
    clear();
    setSize(2);
    data[0] = x;
    data[1] = y;
}

// ------------------------------------------------------------------------
// Sets the size of the vector to 3, and sets the vector data to the given
// data.
// ------------------------------------------------------------------------
void vsVector::set(double x, double y, double z)
{
    clear();
    setSize(3);
    data[0] = x;
    data[1] = y;
    data[2] = z;
}

// ------------------------------------------------------------------------
// Sets the size of the vector to 4, and sets the vector data to the given
// data.
// ------------------------------------------------------------------------
void vsVector::set(double x, double y, double z, double w)
{
    setSize(4);
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

// ------------------------------------------------------------------------
// Makes this vector an exact duplicate of the source vector.
// ------------------------------------------------------------------------
void vsVector::copy(vsVector source)
{
    (*this) = source;
}

// ------------------------------------------------------------------------
// Clears the vector to zero.
// ------------------------------------------------------------------------
void vsVector::clear()
{
    data[0] = 0.0;
    data[1] = 0.0;
    data[2] = 0.0;
    data[3] = 0.0;
}

// ------------------------------------------------------------------------
// Copies the data from the source vector to this vector, setting all
// unused values to zero. This vector's size is unchanged.
// ------------------------------------------------------------------------
void vsVector::clearCopy(vsVector source)
{
    int loop;
    
    clear();
    for (loop = 0; loop < source.getSize(); loop++)
        data[loop] = source[loop];
}

// ------------------------------------------------------------------------
// Sets the size of this vector to the given size (if legal). The vector
// data is unchanged.
// ------------------------------------------------------------------------
void vsVector::setSize(int size)
{
    if ((size < 1) || (size > 4))
    {
        printf("vsVector::setSize: Invalid size parameter\n");
        return;
    }

    vecSize = size;
}

// ------------------------------------------------------------------------
// Retrieves the size of this vector.
// ------------------------------------------------------------------------
int vsVector::getSize()
{
    return vecSize;
}

// ------------------------------------------------------------------------
// Sets one specific value in the vector.
// ------------------------------------------------------------------------
void vsVector::setValue(int index, int value)
{
    if ((index < 0) || (index >= vecSize))
    {
        printf("vsVector::setValue: Invalid index\n");
        return;
    }
    
    data[index] = value;
}

// ------------------------------------------------------------------------
// Retrieves one specific value from the vector.
// ------------------------------------------------------------------------
double vsVector::getValue(int index)
{
    if ((index < 0) || (index >= vecSize))
    {
        printf("vsVector::getValue: Invalid index\n");
        return data[0];
    }
    
    return data[index];
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two vectors. The two vectors
// must be the same size. Two elements are considered equal if they are
// within a small default tolerance value of each other.
// ------------------------------------------------------------------------
int vsVector::isEqual(vsVector operand)
{
    if (vecSize != operand.getSize())
    {
        printf("vsVector::isEqual: Vector size mismatch\n");
        return VS_FALSE;
    }

    for (int i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand[i]) > VS_MATH_DEFAULT_TOLERANCE)
            return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two vectors. The two vectors
// must be the same size. Two elements are considered equal if they are
// within the specified tolerance value of each other.
// ------------------------------------------------------------------------
int vsVector::isAlmostEqual(vsVector operand, double tolerance)
{
    if (vecSize != operand.getSize())
    {
        printf("vsVector::isEqual: Vector size mismatch\n");
        return VS_FALSE;
    }

    for (int i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand[i]) > tolerance)
            return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, keeping the result in this vector.
// The two vectors must be the same size.
// ------------------------------------------------------------------------
void vsVector::add(vsVector addend)
{
    if (vecSize != addend.getSize())
    {
        printf("vsVector::add: Vector size mismatch\n");
        return;
    }

    for (int i = 0; i < vecSize; i++)
        data[i] += addend[i];
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, returning the result. The two
// vectors must be the same size.
// ------------------------------------------------------------------------
vsVector vsVector::getSum(vsVector addend)
{
    vsVector result;
    
    if (vecSize != addend.getSize())
    {
        printf("vsVector::getSum: Vector size mismatch\n");
        return result;
    }

    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] + addend[i];

    return result;
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, keeping the result in
// this vector. The two vectors must be the same size.
// ------------------------------------------------------------------------
void vsVector::subtract(vsVector subtrahend)
{
    if (vecSize != subtrahend.getSize())
    {
        printf("vsVector::subtract: Vector size mismatch\n");
        return;
    }

    for (int i = 0; i < vecSize; i++)
        data[i] -= subtrahend[i];
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, returning the result. The
// two vectors must be the same size.
// ------------------------------------------------------------------------
vsVector vsVector::getDifference(vsVector subtrahend)
{
    vsVector result;
    
    if (vecSize != subtrahend.getSize())
    {
        printf("vsVector::operator-: Vector size mismatch\n");
        return result;
    }

    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] - subtrahend[i];

    return result;
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, keeping the
// result in this vector.
// ------------------------------------------------------------------------
void vsVector::scale(double multiplier)
{
    for (int i = 0; i < vecSize; i++)
        data[i] *= multiplier;
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, returning
// the result.
// ------------------------------------------------------------------------
vsVector vsVector::getScaled(double multiplier)
{
    vsVector result;
    
    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] * multiplier;

    return result;
}
    
// ------------------------------------------------------------------------
// Returns the magnitude of this vector.
// ------------------------------------------------------------------------
double vsVector::getMagnitude()
{
    double total = 0.0;
    for (int i = 0; i < vecSize; i++)
        total += (data[i] * data[i]);

    return sqrt(total);
}

// ------------------------------------------------------------------------
// Returns the result of the vector dot product between the operand vector
// and this vector. The two vectors must be the same size.
// ------------------------------------------------------------------------
double vsVector::getDotProduct(vsVector operand)
{
    double total = 0.0;

    if (vecSize != operand.getSize())
    {
        printf("vsVector::getDotProduct: Vector size mismatch\n");
        return 0.0;
    }

    for (int i = 0; i < vecSize; i++)
        total += (data[i] * operand[i]);

    return total;
}

// ------------------------------------------------------------------------
// Normalizes this vector, keeping the result in this vector.
// ------------------------------------------------------------------------
void vsVector::normalize()
{
    double mag;

    mag = getMagnitude();
    
    if (mag == 0.0)
	return;
    
    for (int i = 0; i < vecSize; i++)
        data[i] /= mag;
}

// ------------------------------------------------------------------------
// Returns a normalized version of this vector.
// ------------------------------------------------------------------------
vsVector vsVector::getNormalized()
{
    vsVector result;
    double mag;

    result.setSize(vecSize);
    mag = getMagnitude();
    
    if (mag == 0.0)
	return result;

    for (int i = 0; i < vecSize; i++)
        result[i] = (data[i] / mag);

    return result;
}

// ------------------------------------------------------------------------
// Computes the vector cross product between this vector and the operand
// vector, keeping the result in this vector. Both vectors must be of
// size 3.
// ------------------------------------------------------------------------
void vsVector::crossProduct(vsVector operand)
{
    double result[3];

    if ((vecSize < 3) || (operand.getSize() < 3))
    {
        printf("vsVector::crossProduct: Both vectors must be at least size "
	    "3\n");
        return;
    }

    result[0] = (data[1] * operand[2]) - (data[2] * operand[1]);
    result[1] = (data[2] * operand[0]) - (data[0] * operand[2]);
    result[2] = (data[0] * operand[1]) - (data[1] * operand[0]);
    
    data[0] = result[0];
    data[1] = result[1];
    data[2] = result[2];
}

// ------------------------------------------------------------------------
// Returns the result of the vector cross product between this vector and
// the operand vector. Both vectors must be of size 3.
// ------------------------------------------------------------------------
vsVector vsVector::getCrossProduct(vsVector operand)
{
    vsVector result;

    result.setSize(3);
    if ((vecSize < 3) || (operand.getSize() < 3))
    {
        printf("vsVector::getCrossProduct: Both vectors must be at least "
	    "size 3\n");
        return result;
    }

    result[0] = (data[1] * operand[2]) - (data[2] * operand[1]);
    result[1] = (data[2] * operand[0]) - (data[0] * operand[2]);
    result[2] = (data[0] * operand[1]) - (data[1] * operand[0]);
    
    return result;
}

// ------------------------------------------------------------------------
// Returns the angle, in degrees, formed by this vector and the specified
// vector.
// ------------------------------------------------------------------------
double vsVector::getAngleBetween(vsVector endVector)
{
    double crossMag, dot;
    
    crossMag = (getCrossProduct(endVector)).getMagnitude();
    dot = getDotProduct(endVector);
    
    return VS_RAD2DEG(atan2(crossMag, dot));
}

// ------------------------------------------------------------------------
// Retrieves one value from the vector as a reference to a double.
// ------------------------------------------------------------------------
double &vsVector::operator[](int index)
{
    if ((index < 0) || (index >= getSize()))
    {
        printf("vsVector::operator[]: Invalid index\n");
        return data[0];
    }
    
    return data[index];
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, returning the result. The two
// vectors must be the same size.
// Equivalent to getSum(addend)
// ------------------------------------------------------------------------
vsVector vsVector::operator+(vsVector addend)
{
    vsVector result;
    
    if (vecSize != addend.getSize())
    {
        printf("vsVector::operator+: Vector size mismatch\n");
        return result;
    }

    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] + addend[i];

    return result;
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, returning the result. The
// two vectors must be the same size.
// Equivalent to getDifference(subtrahend)
// ------------------------------------------------------------------------
vsVector vsVector::operator-(vsVector subtrahend)
{
    vsVector result;
    
    if (vecSize != subtrahend.getSize())
    {
        printf("vsVector::operator-: Vector size mismatch\n");
        return result;
    }

    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] - subtrahend[i];

    return result;
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, returning
// the result.
// Equivalent to getScaled(multiplier)
// ------------------------------------------------------------------------
vsVector vsVector::operator*(double multiplier)
{
    vsVector result;
    
    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] * multiplier;

    return result;
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, keeping the result in this vector.
// The two vectors must be the same size.
// Equivalent to add(addend)
// ------------------------------------------------------------------------
void vsVector::operator+=(vsVector addend)
{
    if (vecSize != addend.getSize())
    {
        printf("vsVector::operator+=: Vector size mismatch\n");
        return;
    }

    for (int i = 0; i < vecSize; i++)
        data[i] += addend[i];
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, keeping the result in
// this vector. The two vectors must be the same size.
// Equivalent to subtract(subtrahend)
// ------------------------------------------------------------------------
void vsVector::operator-=(vsVector subtrahend)
{
    if (vecSize != subtrahend.getSize())
    {
        printf("vsVector::operator-=: Vector size mismatch\n");
        return;
    }

    for (int i = 0; i < vecSize; i++)
        data[i] -= subtrahend[i];
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, keeping the
// result in this vector.
// Equivalent to scale(multiplier)
// ------------------------------------------------------------------------
void vsVector::operator*=(double multiplier)
{
    for (int i = 0; i < vecSize; i++)
        data[i] *= multiplier;
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two vectors. The two vectors
// must be the same size. Two elements are considered equal if they are
// within a small default tolerance value of each other.
// Equivalent to isEqual(operand)
// ------------------------------------------------------------------------
int vsVector::operator==(vsVector operand)
{
    if (vecSize != operand.getSize())
    {
        printf("vsVector::isEqual: Vector size mismatch\n");
        return VS_FALSE;
    }

    for (int i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand[i]) > VS_MATH_DEFAULT_TOLERANCE)
            return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Related function
// Multiplies each element of this vector by the given scalar, returning
// the result.
// ------------------------------------------------------------------------
vsVector operator*(double multiplier, vsVector operand)
{
    vsVector result;
    
    result.setSize(operand.getSize());
    for (int i = 0; i < operand.getSize(); i++)
        result[i] = multiplier * operand[i];

    return result;
}
