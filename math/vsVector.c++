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
//    VESS Module:  vsVector.c++
//
//    Description:  Class representing a variable-sized array of double
//		    precision values. The size of the vector is limited
//		    to four entries.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

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
    // Bounds checking
    if ((size < 1) || (size > 4))
    {
        printf("vsVector::set(int, double[]): Invalid size parameter\n");
        return;
    }

    // Copy the data values from the given array to the vector, setting
    // the size of the vector and setting the unused entries to zero
    clear();
    setSize(size);
    for (int i = 0; i < size; i++)
        data[i] = values[i];
}

// ------------------------------------------------------------------------
// Sets the size of the vector to 2, and sets the vector data to the given
// data
// ------------------------------------------------------------------------
void vsVector::set(double x, double y)
{
    // Copy the data values from the given array to the vector, setting the
    // size of the vector to two and setting the unused entries to zero
    clear();
    setSize(2);
    data[0] = x;
    data[1] = y;
}

// ------------------------------------------------------------------------
// Sets the size of the vector to 3, and sets the vector data to the given
// data
// ------------------------------------------------------------------------
void vsVector::set(double x, double y, double z)
{
    // Copy the data values from the given array to the vector, setting the
    // size of the vector to three and setting the unused entry to zero
    clear();
    setSize(3);
    data[0] = x;
    data[1] = y;
    data[2] = z;
}

// ------------------------------------------------------------------------
// Sets the size of the vector to 4, and sets the vector data to the given
// data
// ------------------------------------------------------------------------
void vsVector::set(double x, double y, double z, double w)
{
    // Copy the data values from the given array to the vector, setting the
    // size of the vector to four
    setSize(4);
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

// ------------------------------------------------------------------------
// Makes this vector an exact duplicate of the source vector
// ------------------------------------------------------------------------
void vsVector::copy(vsVector source)
{
    (*this) = source;
}

// ------------------------------------------------------------------------
// Clears the vector to zero
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
    
    // Clear the vector, and then copy its data into this vector
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
    // Bounds checking
    if ((size < 1) || (size > 4))
    {
        printf("vsVector::setSize: Invalid size parameter\n");
        return;
    }

    // Set the vector size
    vecSize = size;
}

// ------------------------------------------------------------------------
// Retrieves the size of this vector
// ------------------------------------------------------------------------
int vsVector::getSize()
{
    return vecSize;
}

// ------------------------------------------------------------------------
// Sets one specific value in the vector
// ------------------------------------------------------------------------
void vsVector::setValue(int index, double value)
{
    // Bounds checking
    if ((index < 0) || (index >= vecSize))
    {
        printf("vsVector::setValue: Invalid index\n");
        return;
    }
    
    // Set the specified value
    data[index] = value;
}

// ------------------------------------------------------------------------
// Retrieves one specific value from the vector
// ------------------------------------------------------------------------
double vsVector::getValue(int index)
{
    // Bounds checking
    if ((index < 0) || (index >= vecSize))
    {
        printf("vsVector::getValue: Invalid index\n");
        return data[0];
    }
    
    // Return the desired value
    return data[index];
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two vectors. The two vectors
// must be the same size. Two elements are considered equal if they are
// within a small default tolerance value of each other.
// ------------------------------------------------------------------------
int vsVector::isEqual(vsVector operand)
{
    // Verify that the vectors are the same size
    if (vecSize != operand.getSize())
    {
        printf("vsVector::isEqual: Vector size mismatch\n");
        return VS_FALSE;
    }

    // Check each pair of values (this vector's and the operand vector's)
    // for almost-equality; return false if a pair doesn't match up.
    for (int i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand[i]) > VS_MATH_DEFAULT_TOLERANCE)
            return VS_FALSE;

    // If all the pairs match, return true
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two vectors. The two vectors
// must be the same size. Two elements are considered equal if they are
// within the specified tolerance value of each other.
// ------------------------------------------------------------------------
int vsVector::isAlmostEqual(vsVector operand, double tolerance)
{
    // Verify that the vectors are the same size
    if (vecSize != operand.getSize())
    {
        printf("vsVector::isEqual: Vector size mismatch\n");
        return VS_FALSE;
    }

    // Check each pair of values (this vector's and the operand vector's)
    // for almost-equality, 'almost' being specified by a given tolerance
    // value. Return false if a pair doesn't match up.
    for (int i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand[i]) > tolerance)
            return VS_FALSE;

    // If all the pairs match, return true
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, keeping the result in this vector.
// The two vectors must be the same size.
// ------------------------------------------------------------------------
void vsVector::add(vsVector addend)
{
    // Verify that the vectors are the same size
    if (vecSize != addend.getSize())
    {
        printf("vsVector::add: Vector size mismatch\n");
        return;
    }

    // Add each element of the addend vector to this vector
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
    
    // Verify that the vectors are the same size
    if (vecSize != addend.getSize())
    {
        printf("vsVector::getSum: Vector size mismatch\n");
        return result;
    }

    // Create the target vector by adding each element of this vector to
    // the corresponding element of the addend vector
    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] + addend[i];

    // Return the target vector
    return result;
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, keeping the result in
// this vector. The two vectors must be the same size.
// ------------------------------------------------------------------------
void vsVector::subtract(vsVector subtrahend)
{
    // Verify that the vectors are the same size
    if (vecSize != subtrahend.getSize())
    {
        printf("vsVector::subtract: Vector size mismatch\n");
        return;
    }

    // Subtract each element of the subtrahend vector from this vector
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
    
    // Verify that the vectors are the same size
    if (vecSize != subtrahend.getSize())
    {
        printf("vsVector::operator-: Vector size mismatch\n");
        return result;
    }

    // Create the target vector by subtracting each element of the
    // subtrahend from the corresponding element of this vectur
    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] - subtrahend[i];

    // Return the target vector
    return result;
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, keeping the
// result in this vector.
// ------------------------------------------------------------------------
void vsVector::scale(double multiplier)
{
    // Multiply each element of this vector by the given scalar
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
    
    // Create the target vector by multiplying each element of this vector
    // by the given scalar
    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] * multiplier;

    // Return the target vector
    return result;
}
    
// ------------------------------------------------------------------------
// Returns the magnitude of this vector.
// ------------------------------------------------------------------------
double vsVector::getMagnitude()
{
    // Compute the squared magnitude of this vector
    double total = 0.0;
    for (int i = 0; i < vecSize; i++)
        total += (data[i] * data[i]);

    // Return the square root of the square of the magnitude
    return sqrt(total);
}

// ------------------------------------------------------------------------
// Returns the square of the magnitude of this vector.
// ------------------------------------------------------------------------
double vsVector::getMagnitudeSquared()
{
    // Compute the squared magnitude of this vector...
    double total = 0.0;
    for (int i = 0; i < vecSize; i++)
        total += (data[i] * data[i]);

    // ...and return it.
    return total;
}

// ------------------------------------------------------------------------
// Returns the result of the vector dot product between the operand vector
// and this vector. The two vectors must be the same size.
// ------------------------------------------------------------------------
double vsVector::getDotProduct(vsVector operand)
{
    double total = 0.0;

    // Verify that the vectors are the same size
    if (vecSize != operand.getSize())
    {
        printf("vsVector::getDotProduct: Vector size mismatch\n");
        return 0.0;
    }

    // Compute the dot product
    for (int i = 0; i < vecSize; i++)
        total += (data[i] * operand[i]);

    // Return the result
    return total;
}

// ------------------------------------------------------------------------
// Normalizes this vector, keeping the result in this vector.
// ------------------------------------------------------------------------
void vsVector::normalize()
{
    double mag;

    // Get the magnitude of this vector
    mag = getMagnitude();
    
    // If the magnitude is zero, then normalization is undefined. Abort.
    if (mag == 0.0)
	return;
    
    // Divide each element of this vector by the magnitude
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

    // Set the size of the result
    result.setSize(vecSize);

    // Get the magnitude of this vector
    mag = getMagnitude();
    
    // If the magnitude is zero, then normalization is undefined. Return
    // a dummy zero vector result.
    if (mag == 0.0)
	return result;

    // Divide each element of the result vector by the magnitude
    for (int i = 0; i < vecSize; i++)
        result[i] = (data[i] / mag);

    // Return the result vector
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

    // Verify that both vectors are large enough
    if ((vecSize < 3) || (operand.getSize() < 3))
    {
        printf("vsVector::crossProduct: Both vectors must be at least size "
	    "3\n");
        return;
    }

    // Compute the cross product, putting the result in a temporary array
    result[0] = (data[1] * operand[2]) - (data[2] * operand[1]);
    result[1] = (data[2] * operand[0]) - (data[0] * operand[2]);
    result[2] = (data[0] * operand[1]) - (data[1] * operand[0]);
    
    // Copy the resulting product back into this vector
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

    // Verify that both vectors are at least size 3. Return a dummy zero
    // vector if they're not.
    result.setSize(3);
    if ((vecSize < 3) || (operand.getSize() < 3))
    {
        printf("vsVector::getCrossProduct: Both vectors must be at least "
	    "size 3\n");
        return result;
    }

    // Compute the cross product...
    result[0] = (data[1] * operand[2]) - (data[2] * operand[1]);
    result[1] = (data[2] * operand[0]) - (data[0] * operand[2]);
    result[2] = (data[0] * operand[1]) - (data[1] * operand[0]);
    
    // ...and return it.
    return result;
}

// ------------------------------------------------------------------------
// Returns the angle, in degrees, formed by this vector and the specified
// vector.
// ------------------------------------------------------------------------
double vsVector::getAngleBetween(vsVector endVector)
{
    // By one of the definitions of the cross product, the sine of the
    // angle between two vectors is equal to the magnitude of their cross
    // product divided by the product of their magnitudes. Similarly, the
    // cosine of the angle between two vectors is equal to their dot
    // product divided by the product of their magnitudes. Those two
    // equations are combined here to generate the formula 'angle between
    // two vectors = inverse tangent((cross product magnitude) /
    // (dot product))'.

    double crossMag, dot;

    // Compute the cross product magnitude and dot product
    crossMag = (getCrossProduct(endVector)).getMagnitude();
    dot = getDotProduct(endVector);
    
    // Return the inverse tangent of the quotient of the two products
    return VS_RAD2DEG(atan2(crossMag, dot));
}

// ------------------------------------------------------------------------
// Retrieves one value from the vector as a reference to a double.
// ------------------------------------------------------------------------
double &vsVector::operator[](int index)
{
    // Bounds checking
    if ((index < 0) || (index >= getSize()))
    {
        printf("vsVector::operator[]: Invalid index\n");
        return data[0];
    }
    
    // Return a reference to the desired data value
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
    
    // Verify that the vectors are the same size
    if (vecSize != addend.getSize())
    {
        printf("vsVector::operator+: Vector size mismatch\n");
        return result;
    }

    // Create the target vector by adding each element of this vector to
    // the corresponding element of the addend vector
    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] + addend[i];

    // Return the target vector
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
    
    // Verify that the vectors are the same size
    if (vecSize != subtrahend.getSize())
    {
        printf("vsVector::operator-: Vector size mismatch\n");
        return result;
    }

    // Create the target vector by subtracting each element of the
    // subtrahend from the corresponding element of this vectur
    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] - subtrahend[i];

    // Return the target vector
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
    
    // Create the target vector by multiplying each element of this vector
    // by the given scalar
    result.setSize(vecSize);
    for (int i = 0; i < vecSize; i++)
        result[i] = data[i] * multiplier;

    // Return the target vector
    return result;
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, keeping the result in this vector.
// The two vectors must be the same size.
// Equivalent to add(addend)
// ------------------------------------------------------------------------
void vsVector::operator+=(vsVector addend)
{
    // Verify that the vectors are the same size
    if (vecSize != addend.getSize())
    {
        printf("vsVector::operator+=: Vector size mismatch\n");
        return;
    }

    // Add each element of the addend vector to this vector
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
    // Verify that the vectors are the same size
    if (vecSize != subtrahend.getSize())
    {
        printf("vsVector::operator-=: Vector size mismatch\n");
        return;
    }

    // Subtract each element of the subtrahend vector from this vector
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
    // Multiply each element of this vector by the given scalar
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
    // Verify that the vectors are the same size
    if (vecSize != operand.getSize())
    {
        printf("vsVector::isEqual: Vector size mismatch\n");
        return VS_FALSE;
    }

    // Check each pair of values (this vector's and the operand vector's)
    // for almost-equality; return false if a pair doesn't match up.
    for (int i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand[i]) > VS_MATH_DEFAULT_TOLERANCE)
            return VS_FALSE;

    // If all the pairs match, return true
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
    
    // Create the target vector by multiplying each element of this vector
    // by the given scalar
    result.setSize(operand.getSize());
    for (int i = 0; i < operand.getSize(); i++)
        result[i] = multiplier * operand[i];

    // Return the target vector
    return result;
}
