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
    vecSize = 4;
}

// ------------------------------------------------------------------------
// Size constructor - Clears the vector to zero and sets the size to the
// indicated value, if legal.
// ------------------------------------------------------------------------
vsVector::vsVector(int size)
{
    clear();
    vecSize = 4;
    setSize(size);
}

// ------------------------------------------------------------------------
// Data constructor - Sets the size of the vector to the size of the array,
// and sets the vector data to the array data.
// ------------------------------------------------------------------------
vsVector::vsVector(int size, double values[])
{
    clear();
    vecSize = 4;
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
    int i;

    // Bounds checking
    if ((size < 1) || (size > 4))
    {
        printf("vsVector::set(int, double[]): Invalid size parameter\n");
        return;
    }

    // Copy the data values from the given array to the vector, setting
    // the size of the vector and setting the unused entries to zero
    clear();
    vecSize = size;
    for (i = 0; i < size; i++)
        data[i] = values[i];
}

// ------------------------------------------------------------------------
// Sets the size of the vector to 2, and sets the vector data to the given
// data.
// ------------------------------------------------------------------------
void vsVector::set(double x, double y)
{
    // Copy the data values from the given array to the vector, setting the
    // size of the vector to two and setting the unused entries to zero
    clear();
    vecSize = 2;
    data[0] = x;
    data[1] = y;
}

// ------------------------------------------------------------------------
// Sets the size of the vector to 3, and sets the vector data to the given
// data.
// ------------------------------------------------------------------------
void vsVector::set(double x, double y, double z)
{
    // Copy the data values from the given array to the vector, setting the
    // size of the vector to three and setting the unused entry to zero
    clear();
    vecSize = 3;
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
    // Copy the data values from the given array to the vector, setting the
    // size of the vector to four
    vecSize = 4;
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

// ------------------------------------------------------------------------
// Makes this vector an exact duplicate of the source vector.
// ------------------------------------------------------------------------
void vsVector::copy(const vsVector &source)
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
void vsVector::clearCopy(const vsVector &source)
{
    int i;
    
    // Clear the vector, and then copy its data into this vector
    clear();
    for (i = 0; i < source.vecSize; i++)
        data[i] = source.data[i];
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
// Retrieves the size of this vector.
// ------------------------------------------------------------------------
int vsVector::getSize() const
{
    return vecSize;
}

// ------------------------------------------------------------------------
// Sets one specific value in the vector.
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
// Retrieves one specific value from the vector.
// ------------------------------------------------------------------------
double vsVector::getValue(int index) const
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
bool vsVector::isEqual(const vsVector &operand) const
{
    int i;

    // Verify that the vectors are the same size
    if (vecSize != operand.vecSize)
    {
        printf("vsVector::isEqual: Vector size mismatch\n");
        return false;
    }

    // Check each pair of values (this vector's and the operand vector's)
    // for almost-equality; return false if a pair doesn't match up.
    for (i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand.data[i]) > VS_DEFAULT_TOLERANCE)
            return false;

    // If all the pairs match, return true
    return true;
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two vectors. The two vectors
// must be the same size. Two elements are considered equal if they are
// within the specified tolerance value of each other.
// ------------------------------------------------------------------------
bool vsVector::isAlmostEqual(const vsVector &operand, double tolerance) const
{
    int i;

    // Verify that the vectors are the same size
    if (vecSize != operand.vecSize)
    {
        printf("vsVector::isAlmostEqual: Vector size mismatch\n");
        return false;
    }

    // Check each pair of values (this vector's and the operand vector's)
    // for almost-equality, 'almost' being specified by a given tolerance
    // value. Return false if a pair doesn't match up.
    for (i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand.data[i]) > tolerance)
            return false;

    // If all the pairs match, return true
    return true;
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, keeping the result in this vector.
// The two vectors must be the same size.
// ------------------------------------------------------------------------
void vsVector::add(const vsVector &addend)
{
    int i;

    // Verify that the vectors are the same size
    if (vecSize != addend.vecSize)
    {
        printf("vsVector::add: Vector size mismatch\n");
        return;
    }

    // Add each element of the addend vector to this vector
    for (i = 0; i < vecSize; i++)
        data[i] += addend.data[i];
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, returning the result. The two
// vectors must be the same size.
// ------------------------------------------------------------------------
vsVector vsVector::getSum(const vsVector &addend) const
{
    int i;
    vsVector result;

    // Verify that the vectors are the same size
    if (vecSize != addend.vecSize)
    {
        printf("vsVector::getSum: Vector size mismatch\n");
        return result;
    }

    // Create the target vector by adding each element of this vector to
    // the corresponding element of the addend vector
    result.vecSize = vecSize;
    for (i = 0; i < vecSize; i++)
        result.data[i] = data[i] + addend.data[i];

    // Return the target vector
    return result;
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, keeping the result in
// this vector. The two vectors must be the same size.
// ------------------------------------------------------------------------
void vsVector::subtract(const vsVector &subtrahend)
{
    int i;

    // Verify that the vectors are the same size
    if (vecSize != subtrahend.vecSize)
    {
        printf("vsVector::subtract: Vector size mismatch\n");
        return;
    }

    // Subtract each element of the subtrahend vector from this vector
    for (i = 0; i < vecSize; i++)
        data[i] -= subtrahend.data[i];
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, returning the result. The
// two vectors must be the same size.
// ------------------------------------------------------------------------
vsVector vsVector::getDifference(const vsVector &subtrahend) const
{
    int i;
    vsVector result;

    // Verify that the vectors are the same size
    if (vecSize != subtrahend.vecSize)
    {
        printf("vsVector::getDifference: Vector size mismatch\n");
        return result;
    }

    // Create the target vector by subtracting each element of the
    // subtrahend from the corresponding element of this vectur
    result.vecSize = vecSize;
    for (i = 0; i < vecSize; i++)
        result.data[i] = data[i] - subtrahend.data[i];

    // Return the target vector
    return result;
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, keeping the
// result in this vector.
// ------------------------------------------------------------------------
void vsVector::scale(double multiplier)
{
    int i;

    // Multiply each element of this vector by the given scalar
    for (i = 0; i < vecSize; i++)
        data[i] *= multiplier;
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, returning
// the result.
// ------------------------------------------------------------------------
vsVector vsVector::getScaled(double multiplier) const
{
    int i;
    vsVector result;

    // Create the target vector by multiplying each element of this vector
    // by the given scalar
    result.vecSize = vecSize;
    for (i = 0; i < vecSize; i++)
        result.data[i] = data[i] * multiplier;

    // Return the target vector
    return result;
}
    
// ------------------------------------------------------------------------
// Returns the magnitude of this vector.
// ------------------------------------------------------------------------
double vsVector::getMagnitude() const
{
    int i;
    double total = 0.0;

    // Compute the squared magnitude of this vector
    for (i = 0; i < vecSize; i++)
        total += VS_SQR(data[i]);

    // Return the square root of the square of the magnitude
    return sqrt(total);
}

// ------------------------------------------------------------------------
// Returns the square of the magnitude of this vector.
// ------------------------------------------------------------------------
double vsVector::getMagnitudeSquared() const
{
    int i;
    double total = 0.0;

    // Compute the squared magnitude of this vector...
    for (i = 0; i < vecSize; i++)
        total += VS_SQR(data[i]);

    // ...and return it.
    return total;
}

// ------------------------------------------------------------------------
// Returns the result of the vector dot product between the operand vector
// and this vector. The two vectors must be the same size.
// ------------------------------------------------------------------------
double vsVector::getDotProduct(const vsVector &operand) const
{
    int i;
    double total = 0.0;

    // Verify that the vectors are the same size
    if (vecSize != operand.vecSize)
    {
        printf("vsVector::getDotProduct: Vector size mismatch\n");
        return 0.0;
    }

    // Compute the dot product
    for (i = 0; i < vecSize; i++)
        total += (data[i] * operand.data[i]);

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
vsVector vsVector::getNormalized() const
{
    int i;
    vsVector result;
    double mag;

    // Set the size of the result
    result.vecSize = vecSize;

    // Get the magnitude of this vector
    mag = getMagnitude();
    
    // If the magnitude is zero, then normalization is undefined. Return
    // a dummy zero vector result.
    if (mag == 0.0)
	return result;

    // Divide each element of the result vector by the magnitude
    for (i = 0; i < vecSize; i++)
        result.data[i] = (data[i] / mag);

    // Return the result vector
    return result;
}

// ------------------------------------------------------------------------
// Computes the vector cross product between this vector and the operand
// vector, keeping the result in this vector. Both vectors must be of
// size 3.
// ------------------------------------------------------------------------
void vsVector::crossProduct(const vsVector &operand)
{
    double result[3];

    // Verify that both vectors are large enough
    if ((vecSize < 3) || (operand.vecSize < 3))
    {
        printf("vsVector::crossProduct: Both vectors must be at least "
            "size 3\n");
        return;
    }

    // Compute the cross product, putting the result in a temporary array
    result[0] = (data[1] * operand.data[2]) - (data[2] * operand.data[1]);
    result[1] = (data[2] * operand.data[0]) - (data[0] * operand.data[2]);
    result[2] = (data[0] * operand.data[1]) - (data[1] * operand.data[0]);

    // Copy the resulting product back into this vector
    data[0] = result[0];
    data[1] = result[1];
    data[2] = result[2];
}

// ------------------------------------------------------------------------
// Returns the result of the vector cross product between this vector and
// the operand vector. Both vectors must be of size 3.
// ------------------------------------------------------------------------
vsVector vsVector::getCrossProduct(const vsVector &operand) const
{
    vsVector result;

    // Verify that both vectors are at least size 3. Return a dummy zero
    // vector if they're not.
    if ((vecSize < 3) || (operand.vecSize < 3))
    {
        printf("vsVector::getCrossProduct: Both vectors must be at least "
	    "size 3\n");
        return result;
    }

    result.vecSize = 3;

    // Compute the cross product...
    result.data[0] = (data[1] * operand.data[2]) - (data[2] * operand.data[1]);
    result.data[1] = (data[2] * operand.data[0]) - (data[0] * operand.data[2]);
    result.data[2] = (data[0] * operand.data[1]) - (data[1] * operand.data[0]);
    
    // ...and return it.
    return result;
}

// ------------------------------------------------------------------------
// Returns the angle, in degrees, formed by this vector and the specified
// vector.
// ------------------------------------------------------------------------
double vsVector::getAngleBetween(const vsVector &endVector) const
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
// Retrieves one value from the vector as a reference to a double.
// ------------------------------------------------------------------------
const double &vsVector::operator[](int index) const
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
vsVector vsVector::operator+(const vsVector &addend) const
{
    int i;
    vsVector result;

    // Verify that the vectors are the same size
    if (vecSize != addend.vecSize)
    {
        printf("vsVector::operator+: Vector size mismatch\n");
        return result;
    }

    // Create the target vector by adding each element of this vector to
    // the corresponding element of the addend vector
    result.vecSize = vecSize;
    for (i = 0; i < vecSize; i++)
        result.data[i] = data[i] + addend.data[i];

    // Return the target vector
    return result;
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, returning the result. The
// two vectors must be the same size.
// Equivalent to getDifference(subtrahend)
// ------------------------------------------------------------------------
vsVector vsVector::operator-(const vsVector &subtrahend) const
{
    int i;
    vsVector result;

    // Verify that the vectors are the same size
    if (vecSize != subtrahend.vecSize)
    {
        printf("vsVector::operator-: Vector size mismatch\n");
        return result;
    }

    // Create the target vector by subtracting each element of the
    // subtrahend from the corresponding element of this vectur
    result.vecSize = vecSize;
    for (i = 0; i < vecSize; i++)
        result.data[i] = data[i] - subtrahend.data[i];

    // Return the target vector
    return result;
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, returning
// the result.
// Equivalent to getScaled(multiplier)
// ------------------------------------------------------------------------
vsVector vsVector::operator*(double multiplier) const
{
    int i;
    vsVector result;

    // Create the target vector by multiplying each element of this vector
    // by the given scalar
    result.vecSize = vecSize;
    for (i = 0; i < vecSize; i++)
        result.data[i] = data[i] * multiplier;

    // Return the target vector
    return result;
}

// ------------------------------------------------------------------------
// Adds the addend vector to this one, keeping the result in this vector.
// The two vectors must be the same size.
// Equivalent to add(addend)
// ------------------------------------------------------------------------
void vsVector::operator+=(const vsVector &addend)
{
    int i;

    // Verify that the vectors are the same size
    if (vecSize != addend.vecSize)
    {
        printf("vsVector::operator+=: Vector size mismatch\n");
        return;
    }

    // Add each element of the addend vector to this vector
    for (i = 0; i < vecSize; i++)
        data[i] += addend.data[i];
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend vector from this one, keeping the result in
// this vector. The two vectors must be the same size.
// Equivalent to subtract(subtrahend)
// ------------------------------------------------------------------------
void vsVector::operator-=(const vsVector &subtrahend)
{
    int i;

    // Verify that the vectors are the same size
    if (vecSize != subtrahend.vecSize)
    {
        printf("vsVector::operator-=: Vector size mismatch\n");
        return;
    }

    // Subtract each element of the subtrahend vector from this vector
    for (i = 0; i < vecSize; i++)
        data[i] -= subtrahend.data[i];
}

// ------------------------------------------------------------------------
// Multiplies each element of this vector by the given scalar, keeping the
// result in this vector.
// Equivalent to scale(multiplier)
// ------------------------------------------------------------------------
void vsVector::operator*=(double multiplier)
{
    int i;

    // Multiply each element of this vector by the given scalar
    for (i = 0; i < vecSize; i++)
        data[i] *= multiplier;
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two vectors. The two vectors
// must be the same size. Two elements are considered equal if they are
// within a small default tolerance value of each other.
// Equivalent to isEqual(operand)
// ------------------------------------------------------------------------
bool vsVector::operator==(const vsVector &operand) const
{
    int i;

    // Verify that the vectors are the same size
    if (vecSize != operand.vecSize)
    {
        printf("vsVector::operator==: Vector size mismatch\n");
        return false;
    }

    // Check each pair of values (this vector's and the operand vector's)
    // for almost-equality; return false if a pair doesn't match up.
    for (i = 0; i < vecSize; i++)
        if (fabs(data[i] - operand.data[i]) > VS_DEFAULT_TOLERANCE)
            return false;

    // If all the pairs match, return true
    return true;
}

// ------------------------------------------------------------------------
// Prints a text representation of this vector to stdout
// ------------------------------------------------------------------------
void vsVector::print() const
{
    int i;

    // Enclose the components of the vector in angle brackets
    printf("<");

    // Print all but the last component with a trailing comma and space
    for (i = 0; i < vecSize-1; i++)
        printf("%0.4lf, ", data[i]);

    // Print the last component and close with an angle bracket
    printf("%0.4lf>", data[vecSize-1]);
}

// ------------------------------------------------------------------------
// Prints a text representation of this vector to the specified file
// ------------------------------------------------------------------------
void vsVector::print(FILE *fp) const
{
    int i;

    // Enclose the components of the vector in angle brackets
    fprintf(fp, "<");

    // Print all but the last component with a trailing comma and space
    for (i = 0; i < vecSize-1; i++)
        fprintf(fp, "%0.4lf, ", data[i]);

    // Print the last component and close with an angle bracket
    fprintf(fp, "%0.4lf>", data[vecSize-1]);
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
