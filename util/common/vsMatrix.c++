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
//    VESS Module:  vsMatrix.c++
//
//    Description:  Class implementing a 4x4 graphical transformation
//		    matrix
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <math.h>
#include <stdio.h>
#include "vsGlobals.h++"
#include "vsMatrix.h++"

// ------------------------------------------------------------------------
// Constructor
// Clears the matrix to zero (not identity)
// ------------------------------------------------------------------------
vsMatrix::vsMatrix()
{
    clear();
}

// ------------------------------------------------------------------------
// Constructor
// Sets the matrix data to the given two-dimensional array
// ------------------------------------------------------------------------
vsMatrix::vsMatrix(double values[4][4])
{
    set(values);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMatrix::~vsMatrix()
{
}

// ------------------------------------------------------------------------
// Sets the matrix data to the given two-dimensional array
// ------------------------------------------------------------------------
void vsMatrix::set(double values[4][4])
{
    // Copy the matix values from the specified array to the matrix's
    // internal storage area
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            data[i][j] = values[i][j];
}

// ------------------------------------------------------------------------
// Makes this matrix an exact duplicate of the source matrix
// ------------------------------------------------------------------------
void vsMatrix::copy(vsMatrix source)
{
    (*this) = source;
}

// ------------------------------------------------------------------------
// Sets the matrix data to zero
// ------------------------------------------------------------------------
void vsMatrix::clear()
{
    for (int i = 0; i < 4; i++)
        data[i].clear();
}

// ------------------------------------------------------------------------
// Sets one specific data value in matrix
// ------------------------------------------------------------------------
void vsMatrix::setValue(int row, int column, double value)
{
    // Bounds checking
    if ((row < 0) || (row > 3))
    {
        printf("vsMatrix::setValue: Bad row index\n");
        return;
    }
    if ((column < 0) || (column > 3))
    {
        printf("vsMatrix::setValue: Bad column index\n");
        return;
    }
    
    // Set the specified value
    data[row][column] = value;
}

// ------------------------------------------------------------------------
// Retrieves one specific data value from matrix
// ------------------------------------------------------------------------
double vsMatrix::getValue(int row, int column)
{
    // Bounds checking
    if ((row < 0) || (row > 3))
    {
        printf("vsMatrix::getValue: Bad row index\n");
        return data[0][0];
    }
    if ((column < 0) || (column > 3))
    {
        printf("vsMatrix::getValue: Bad column index\n");
        return data[0][0];
    }
    
    // Return the desired value
    return data[row][column];
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two matricies. Two elements
// are considered equal if they are within a small default tolerance value.
// ------------------------------------------------------------------------
int vsMatrix::isEqual(vsMatrix operand)
{
    int i, j;

    // Check each pair of values (this object's value and the operand's
    // value) for almost-equality; return false if a pair doesn't match up.
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (fabs(data[i][j] - operand[i][j]) > VS_DEFAULT_TOLERANCE)
                return VS_FALSE;

    // If all the pairs match, return true
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two matricies. Two elements
// are considered equal if they are within the specified tolerance value.
// ------------------------------------------------------------------------
int vsMatrix::isAlmostEqual(vsMatrix operand, double tolerance)
{
    int i, j;

    // Check each pair of values (this object's value and the operand's
    // value) for almost-equality, 'almost' being specified by a given
    // tolerance value. Return false if a pair doesn't match up.
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (fabs(data[i][j] - operand[i][j]) > tolerance)
                return VS_FALSE;

    // If all the pairs match, return true
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Adds the specified matrix to this matrix, keeping the result
// ------------------------------------------------------------------------
void vsMatrix::add(vsMatrix addend)
{
    int i, j;
    
    // Add each element of the addend matrix to this matrix
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            data[i][j] += addend[i][j];
}

// ------------------------------------------------------------------------
// Adds the specified matrix to this matrix, returning the result
// ------------------------------------------------------------------------
vsMatrix vsMatrix::getSum(vsMatrix addend)
{
    int i, j;
    vsMatrix result;
    
    // Create the target matrix by adding each element of this matrix
    // to each element of the addend matrix
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] + addend[i][j];

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Subtracts the specified matrix from this matrix, keeping the result
// ------------------------------------------------------------------------
void vsMatrix::subtract(vsMatrix subtrahend)
{
    int i, j;
    
    // Subtract each element of the subtrahend matrix from this matrix
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            data[i][j] -= subtrahend[i][j];
}

// ------------------------------------------------------------------------
// Subtracts the specified matrix from this matrix, returning the result
// ------------------------------------------------------------------------
vsMatrix vsMatrix::getDifference(vsMatrix subtrahend)
{
    int i, j;
    vsMatrix result;
    
    // Create the target matrix by subtracting each element of the
    // subtrahend from each element of this matrix
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] - subtrahend[i][j];

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Multiplies this matrix by the given scalar, keeping the result
// ------------------------------------------------------------------------
void vsMatrix::scale(double multiplier)
{
    int i, j;
    
    // Multiply each element of this matrix by the given scalar
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            data[i][j] *= multiplier;
}

// ------------------------------------------------------------------------
// Multiplies this matrix by the given scalar, returning the result
// ------------------------------------------------------------------------
vsMatrix vsMatrix::getScaled(double multiplier)
{
    int i, j;
    vsMatrix result;
    
    // Create the target matrix by multiplying each element of this matrix
    // by the given scalar
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] * multiplier;

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Transposes this matrix, keeping the result
// ------------------------------------------------------------------------
void vsMatrix::transpose()
{
    int i, j;
    double temp;
    
    // Swap the elements of this matrix across its diagonal
    for (i = 0; i < 4; i++)
        for (j = 0; j < i; j++)
        {
            temp = data[i][j];
            data[i][j] = data[j][i];
            data[j][i] = temp;
        }
}

// ------------------------------------------------------------------------
// Transposes this matrix, returning the result
// ------------------------------------------------------------------------
vsMatrix vsMatrix::getTranspose()
{
    int i, j;
    vsMatrix result;
    
    // Create the target matrix by copying the values from this matrix
    // to the target matrix in a swapped-across-the-diagonal fashion
    for (i = 0; i < 4; i++)
        for (j = 0; j < i; j++)
            result[j][i] = data[i][j];

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Returns the value of the determinant for this matrix
// ------------------------------------------------------------------------
double vsMatrix::getDeterminant()
{
    // Cheat: Rather than coming up with a fancy algorithm for calculating
    // the determinant, since the matrix is of fixed size I can just
    // hardcode the pattern of multiplications.

    // The values in this array represent column indices; the row indices
    // are implied by the position of each index within its array. Each
    // array of four numbers represents four values in the matrix that
    // must be multiplied together, and the result added (or subtracted,
    // for the second half of the entries) to the determinant total.
    int detArray[24][4] = { {0, 1, 2, 3}, {0, 2, 3, 1}, {0, 3, 1, 2},
			    {1, 3, 2, 0}, {1, 0, 3, 2}, {1, 2, 0, 3},
			    {2, 0, 1, 3}, {2, 1, 3, 0}, {2, 3, 0, 1},
			    {3, 2, 1, 0}, {3, 0, 2, 1}, {3, 1, 0, 2},
			    {0, 3, 2, 1}, {0, 1, 3, 2}, {0, 2, 1, 3},
			    {1, 0, 2, 3}, {1, 2, 3, 0}, {1, 3, 0, 2},
			    {2, 3, 1, 0}, {2, 0, 3, 1}, {2, 1, 0, 3},
			    {3, 0, 1, 2}, {3, 1, 2, 0}, {3, 2, 0, 1} };

    // For example, the third entry in the array is {0, 3, 1, 2}. This
    // corresponds to the third term of the determinant calculation; this
    // term is calculated by multiplying (data[0][0] * data[1][3] *
    // data[2][1] * data[3][2]). Each value in the array entry is paired
    // with its position within the entry to come up with the indices
    // for the data values to multiply. Finally, after computing the
    // product, the result is added to the determinant total, because the
    // entry is one of the first twelve entries. (Had this been, say, the
    // fifteenth entry instead, the product would be subtracted indtead.)

    double total, intermediate;
    int loop, sloop;
    
    total = 0.0;
    // Cycle over the entries in the array and 'play back' the
    // multiplications indicated therein
    for (loop = 0; loop < 24; loop++)
    {
	// Multiply the four values
	intermediate = 1.0;
	for (sloop = 0; sloop < 4; sloop++)
	    intermediate *= data[sloop][detArray[loop][sloop]];

	// The first half of the array contains all of the positive
	// products; the second half contains the negative ones
	if (loop > 11)
	    total -= intermediate;
	else
	    total += intermediate;
    }
    
    // Return the total of the determinant calculation
    return total;
}

// ------------------------------------------------------------------------
// Sets this matrix to the inverse of itself. If the matrix has a
// determinant of zero, it cannot be inverted and is left unchanged.
// ------------------------------------------------------------------------
void vsMatrix::invert()
{
    double det;
    int loop, sloop, tloop;
    vsMatrix result;
    vsMatrix minorMatrix;
    
    // A matrix with a zero determinant can't be inverted
    det = getDeterminant();
    if (fabs(det) < 1E-6)
    {
	printf("vsMatrix::invert: Matrix has no inverse\n");
	return;
    }
    
    // Find the cofactor matrix
    for (loop = 0; loop < 4; loop++)
	for (sloop = 0; sloop < 4; sloop++)
	{
            // Compute this value in the cofactor matrix by finding
	    // the determinant of the minor matrix for this entry.

	    // Since we're just going to do a determinant calculation
	    // afterwards, the effect of creating a minor matrix can be
	    // emulated by clearing the row and column of the matrix
	    // that would have been removed intead to zero, except for
	    // the value at the intersection of the row and column,
	    // which is set to one.
	    minorMatrix = (*this);
	    for (tloop = 0; tloop < 4; tloop++)
	    {
		minorMatrix[loop][tloop] = 0.0;
		minorMatrix[tloop][sloop] = 0.0;
	    }
	    minorMatrix[loop][sloop] = 1.0;
	    
	    result[loop][sloop] = minorMatrix.getDeterminant();
	}

    // Create the adjoint matrix by transposing the cofactor matrix
    result.transpose();
    
    // Divide the adjoint matrix by the determinant of the original
    // matrix to form the inverse
    result.scale(1.0 / getDeterminant());
    
    // Assign the contents of the result matrix to this matrix
    (*this) = result;
}

// ------------------------------------------------------------------------
// Returns the inverse matrix of this matrix. Returns a zero matrix if
// this matrix does not have an inverse.
// ------------------------------------------------------------------------
vsMatrix vsMatrix::getInverse()
{
    vsMatrix result;
    
    // If the determinant of the matrix is zero (or so close that it might
    // as well be zero), then the matrix can't be inverted
    if (fabs(getDeterminant()) < 1E-6)
    {
	result.clear();
    }
    else
    {
	// Cheat: call the other version of the inverse function
	result = (*this);
	result.invert();
    }
    
    // Return the inverted matrix
    return result;
}

// ------------------------------------------------------------------------
// Multiplies this matrix with the given matrix; the operand matrix is
// considered to be on the left. The result is stored.
// ------------------------------------------------------------------------
void vsMatrix::preMultiply(vsMatrix operand)
{
    double result[4][4];
    int i, j, k;
    
    // Do a matrix-multiply operation between this matrix and the operand
    // matrix, with this matrix second; place the results in a temporary
    // matrix instead of this one so that we don't overwrite values that
    // may be needed for later calculations.
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (operand[i][k] * data[k][j]);
        }

    // Copy the result from the temporary matrix back to this one
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            data[i][j] = result[i][j];
}

// ------------------------------------------------------------------------
// Multiplies this matrix with the given matrix; the operand matrix is
// considered to be on the left. The result is returned.
// ------------------------------------------------------------------------
vsMatrix vsMatrix::getPreMultiplied(vsMatrix operand)
{
    vsMatrix result;
    int i, j, k;
    
    // Do a matrix-multiply operation between this matrix and the operand
    // matrix, with this matrix second; place the results in the target
    // matrix.
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (operand[i][k] * data[k][j]);
        }

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Multiplies this matrix with the given matrix; the operand matrix is
// considered to be on the right. The result is stored.
// ------------------------------------------------------------------------
void vsMatrix::postMultiply(vsMatrix operand)
{
    double result[4][4];
    int i, j, k;
    
    // Do a matrix-multiply operation between this matrix and the operand
    // matrix, with this matrix first; place the results in a temporary
    // matrix instead of this one so that we don't overwrite values that
    // may be needed for later calculations.
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (data[i][k] * operand[k][j]);
        }

    // Copy the result from the temporary matrix back to this one
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            data[i][j] = result[i][j];
}

// ------------------------------------------------------------------------
// Multiplies this matrix with the given matrix; the operand matrix is
// considered to be on the right. The result is returned.
// ------------------------------------------------------------------------
vsMatrix vsMatrix::getPostMultiplied(vsMatrix operand)
{
    vsMatrix result;
    int i, j, k;
    
    // Do a matrix-multiply operation between this matrix and the operand
    // matrix, with this matrix first; place the results in the target
    // matrix.
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (data[i][k] * operand[k][j]);
        }

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Transforms the given point by multiplying this matrix by the point as
// a column vector on the right, returning the result. The fourth element
// of the operand is assumed to be one.
// ------------------------------------------------------------------------
vsVector vsMatrix::getPointXform(vsVector operand)
{
    vsVector result;
    int i, j;

    // To be transformed in this manner, the operand vector must be
    // at least size 3.
    if (operand.getSize() < 3)
    {
        printf("vsMatrix::getPointXForm: Operand vector is too small\n");
        return result;
    }

    // Transform the vector by this matrix
    for (i = 0; i < 4; i++)
    {
        result[i] = 0.0;
        for (j = 0; j < 4; j++)
        {
            // Assume the fourth value of the vector is one
            if (j == 3)
                result[i] += data[i][j];
            else
                result[i] += (data[i][j] * operand[j]);
        }
    }
    
    // Resize the result to match the size of the operand vector, and
    // return it.
    result.setSize(operand.getSize());
    return result;
}

// ------------------------------------------------------------------------
// Transforms the given vector by multiplying this matrix by the vector as
// a column vector on the right, returning the result. The fourth element
// of the operand is assumed to be zero.
// ------------------------------------------------------------------------
vsVector vsMatrix::getVectorXform(vsVector operand)
{
    vsVector result;
    int i, j;

    // To be transformed in this manner, the operand vector must be
    // at least size 3.
    if (operand.getSize() < 3)
    {
        printf("vsMatrix::getVectorXForm: Operand vector is too small\n");
        return result;
    }

    // Transform the vector by this matrix
    for (i = 0; i < 4; i++)
    {
        result[i] = 0.0;
	// Ignore the fourth value of the vector, if there is one
        for (j = 0; j < 3; j++)
            result[i] += (data[i][j] * operand[j]);
    }
    
    // Resize the result to match the size of the operand vector, and
    // return it.
    result.setSize(operand.getSize());
    return result;
}

// ------------------------------------------------------------------------
// Transforms the given homogeneous-coordinate point by multiplying this
// matrix by the point as a column vector on the right. The result is
// returned.
// ------------------------------------------------------------------------
vsVector vsMatrix::getFullXform(vsVector operand)
{
    vsVector result;
    int i, j;

    // To be transformed in this manner, the operand vector must be
    // at least size 4.
    if (operand.getSize() < 4)
    {
        printf("vsMatrix::getFullXForm: Operand vector is too small\n");
        return result;
    }

    // Transform the vector by this matrix
    for (i = 0; i < 4; i++)
    {
        result[i] = 0.0;
        for (j = 0; j < 4; j++)
            result[i] += (data[i][j] * operand[j]);
    }
    
    // Return the transformed vector
    return result;
}

// ------------------------------------------------------------------------
// Sets this matrix to the identity matrix.
// ------------------------------------------------------------------------
void vsMatrix::setIdentity()
{
    int loop;

    // Clear the matrix to all zeroes
    clear();

    // Set the matrix entries along the diagonal to one
    for (loop = 0; loop < 4; loop++)
        data[loop][loop] = 1.0;
}


// ------------------------------------------------------------------------
// Sets this matrix to a rotation matrix. The rotation is specified as a
// set of three Euler angle rotations, with the given axis constant used
// to specify the order of the axes.
// ------------------------------------------------------------------------
void vsMatrix::setEulerRotation(vsMathEulerAxisOrder axisOrder,
    double axis1Degrees, double axis2Degrees, double axis3Degrees)
{
    int loop;
    int axis[3];
    vsMatrix axisRotation[3];
    double axisDegrees[3];
    double tempVal;
    
    // Place the degree values into an array to make then easier to
    // iterate through
    axisDegrees[0] = axis1Degrees;
    axisDegrees[1] = axis2Degrees;
    axisDegrees[2] = axis3Degrees;
    
    // Decompose the axisOrder constant into three separate rotation axes
    switch (axisOrder)
    {
        case VS_EULER_ANGLES_XYZ_S:
        case VS_EULER_ANGLES_XYZ_R:
            axis[0] = 0;   axis[1] = 1;   axis[2] = 2;
            break;
        case VS_EULER_ANGLES_XZY_S:
        case VS_EULER_ANGLES_XZY_R:
            axis[0] = 0;   axis[1] = 2;   axis[2] = 1;
            break;
        case VS_EULER_ANGLES_YXZ_S:
        case VS_EULER_ANGLES_YXZ_R:
            axis[0] = 1;   axis[1] = 0;   axis[2] = 2;
            break;
        case VS_EULER_ANGLES_YZX_S:
        case VS_EULER_ANGLES_YZX_R:
            axis[0] = 1;   axis[1] = 2;   axis[2] = 0;
            break;
        case VS_EULER_ANGLES_ZXY_S:
        case VS_EULER_ANGLES_ZXY_R:
            axis[0] = 2;   axis[1] = 0;   axis[2] = 1;
            break;
        case VS_EULER_ANGLES_ZYX_S:
        case VS_EULER_ANGLES_ZYX_R:
            axis[0] = 2;   axis[1] = 1;   axis[2] = 0;
            break;
        case VS_EULER_ANGLES_XYX_S:
        case VS_EULER_ANGLES_XYX_R:
            axis[0] = 0;   axis[1] = 1;   axis[2] = 0;
            break;
        case VS_EULER_ANGLES_XZX_S:
        case VS_EULER_ANGLES_XZX_R:
            axis[0] = 0;   axis[1] = 2;   axis[2] = 0;
            break;
        case VS_EULER_ANGLES_YXY_S:
        case VS_EULER_ANGLES_YXY_R:
            axis[0] = 1;   axis[1] = 0;   axis[2] = 1;
            break;
        case VS_EULER_ANGLES_YZY_S:
        case VS_EULER_ANGLES_YZY_R:
            axis[0] = 1;   axis[1] = 2;   axis[2] = 1;
            break;
        case VS_EULER_ANGLES_ZXZ_S:
        case VS_EULER_ANGLES_ZXZ_R:
            axis[0] = 2;   axis[1] = 0;   axis[2] = 2;
            break;
        case VS_EULER_ANGLES_ZYZ_S:
        case VS_EULER_ANGLES_ZYZ_R:
            axis[0] = 2;   axis[1] = 1;   axis[2] = 2;
            break;
    }
    
    // Compute a rotation matrix for each of the three rotations
    for (loop = 0; loop < 3; loop++)
    {
        // Initialize the matrix to zero, with a one in the
	// homogeneous scale position
        axisRotation[loop].clear();
        axisRotation[loop][3][3] = 1.0;

        // Construct a rotation matrix based on the rotation degree
	// value and the axis of rotation
        switch (axis[loop])
        {
            case 0:
		// X-axis rotation matrix
                axisRotation[loop][0][0] = 1.0;

		tempVal = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][1][1] = tempVal;
                axisRotation[loop][2][2] = tempVal;

		tempVal = sin(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][2][1] = tempVal;
                axisRotation[loop][1][2] = -tempVal;

                break;
            case 1:
		// Y-axis rotation matrix
                axisRotation[loop][1][1] = 1.0;

		tempVal = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][0][0] = tempVal;
                axisRotation[loop][2][2] = tempVal;

		tempVal = sin(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][0][2] = tempVal;
                axisRotation[loop][2][0] = -tempVal;

                break;
            case 2:
		// Z-axis rotation matrix
                axisRotation[loop][2][2] = 1.0;

		tempVal = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][0][0] = tempVal;
                axisRotation[loop][1][1] = tempVal;

		tempVal = sin(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][1][0] = tempVal;
                axisRotation[loop][0][1] = -tempVal;

                break;
        }
    }

    // Combine the three separate rotations into a composite rotation
    // matrix. Also check for relative rotations; reverse the order of
    // the matrices if a relative axis order is specified.
    if ((axisOrder >= VS_EULER_ANGLES_XYZ_R) &&
        (axisOrder <= VS_EULER_ANGLES_ZYZ_R))
        (*this) = axisRotation[0] * axisRotation[1] * axisRotation[2];
    else
        (*this) = axisRotation[2] * axisRotation[1] * axisRotation[0];
}

// ------------------------------------------------------------------------
// Retreives the rotation indicated by this matrix as a set of three Euler
// angle rotations. The specified axis constant is used to determine the
// order of the reconstructed rotations.
//
// Note: NULL pointers may be passed in to denote unwanted return values
// ------------------------------------------------------------------------
void vsMatrix::getEulerRotation(vsMathEulerAxisOrder axisOrder,
    double *axis1Degrees, double *axis2Degrees, double *axis3Degrees)
{
    // I hope this works...  I got the actual engine for this code
    // from some other site (Princeton, I think). It effectively
    // compresses all of the different axis combinations into
    // two different cases.
    
    int isRepeat, isOdd;
    int i, j, k;
    double tempDouble;
    double result1, result2, result3;
    
    // First, determine all of the vital data for each axis combination
    switch (axisOrder)
    {
        case VS_EULER_ANGLES_XYZ_S:
        case VS_EULER_ANGLES_ZYX_R:
            i = 0;   j = 1;   k = 2;   isRepeat = 0;   isOdd = 0;
            break;
        case VS_EULER_ANGLES_XZY_S:
        case VS_EULER_ANGLES_YZX_R:
            i = 0;   j = 2;   k = 1;   isRepeat = 0;   isOdd = 1;
            break;
        case VS_EULER_ANGLES_YXZ_S:
        case VS_EULER_ANGLES_ZXY_R:
            i = 1;   j = 0;   k = 2;   isRepeat = 0;   isOdd = 1;
            break;
        case VS_EULER_ANGLES_YZX_S:
        case VS_EULER_ANGLES_XZY_R:
            i = 1;   j = 2;   k = 0;   isRepeat = 0;   isOdd = 0;
            break;
        case VS_EULER_ANGLES_ZXY_S:
        case VS_EULER_ANGLES_YXZ_R:
            i = 2;   j = 0;   k = 1;   isRepeat = 0;   isOdd = 0;
            break;
        case VS_EULER_ANGLES_ZYX_S:
        case VS_EULER_ANGLES_XYZ_R:
            i = 2;   j = 1;   k = 0;   isRepeat = 0;   isOdd = 1;
            break;
        case VS_EULER_ANGLES_XYX_S:
        case VS_EULER_ANGLES_XYX_R:
            i = 0;   j = 1;   k = 2;   isRepeat = 1;   isOdd = 0;
            break;
        case VS_EULER_ANGLES_XZX_S:
        case VS_EULER_ANGLES_XZX_R:
            i = 0;   j = 2;   k = 1;   isRepeat = 1;   isOdd = 1;
            break;
        case VS_EULER_ANGLES_YXY_S:
        case VS_EULER_ANGLES_YXY_R:
            i = 1;   j = 0;   k = 2;   isRepeat = 1;   isOdd = 1;
            break;
        case VS_EULER_ANGLES_YZY_S:
        case VS_EULER_ANGLES_YZY_R:
            i = 1;   j = 2;   k = 0;   isRepeat = 1;   isOdd = 0;
            break;
        case VS_EULER_ANGLES_ZXZ_S:
        case VS_EULER_ANGLES_ZXZ_R:
            i = 2;   j = 0;   k = 1;   isRepeat = 1;   isOdd = 0;
            break;
        case VS_EULER_ANGLES_ZYZ_S:
        case VS_EULER_ANGLES_ZYZ_R:
            i = 2;   j = 1;   k = 0;   isRepeat = 1;   isOdd = 1;
            break;
    }

    // Run the angle-finder algorithm
    double yVal;
    if (isRepeat)
    {
        // One axis was repeated
        yVal = sqrt(VS_SQR(data[i][j]) + VS_SQR(data[i][k]));
        if (yVal > 1E-6)
        {
            result1 = VS_RAD2DEG(atan2(data[i][j], data[i][k]));
            result2 = VS_RAD2DEG(atan2(yVal, data[i][i]));
            result3 = VS_RAD2DEG(atan2(data[j][i], -data[k][i]));
        }
        else
        {
            result1 = VS_RAD2DEG(atan2(-data[j][k], data[j][j]));
            result2 = VS_RAD2DEG(atan2(yVal, data[i][i]));
            result3 = VS_RAD2DEG(0.0);
        }
    }
    else
    {
        // Each axis used only once
        yVal = sqrt(VS_SQR(data[i][i]) + VS_SQR(data[j][i]));
        if (yVal > 1E-6)
        {
            result1 = VS_RAD2DEG(atan2(data[k][j], data[k][k]));
            result2 = VS_RAD2DEG(atan2(-data[k][i], yVal));
            result3 = VS_RAD2DEG(atan2(data[j][i], data[i][i]));
        }
        else
        {
            result1 = VS_RAD2DEG(atan2(-data[j][k], data[j][j]));
            result2 = VS_RAD2DEG(atan2(-data[k][i], yVal));
            result3 = VS_RAD2DEG(0.0);
        }
    }
    
    // Check for 'odd' axis ordering and negate the result if so
    if (isOdd)
    {
        result1 = -result1;
        result2 = -result2;
        result3 = -result3;
    }

    // Check for relative rotations; swap the first and third result if so
    if ((axisOrder >= VS_EULER_ANGLES_XYZ_R) &&
        (axisOrder <= VS_EULER_ANGLES_ZYZ_R))
    {
        tempDouble = result1;
        result1 = result3;
        result3 = tempDouble;
    }
    
    // Copy the desired results to the designated locations
    if (axis1Degrees)
        *axis1Degrees = result1;
    if (axis2Degrees)
        *axis2Degrees = result2;
    if (axis3Degrees)
        *axis3Degrees = result3;
}

// ------------------------------------------------------------------------
// Sets this matrix to a rotation matrix. The rotation is specified by a
// rotational quaternion.
// ------------------------------------------------------------------------
void vsMatrix::setQuatRotation(vsQuat quat)
{
    vsQuat theQuat;
    double x, y, z, w;

    // Normalize the given quaternion and extract the values from it
    theQuat = quat.getNormalized();
    x = theQuat[0];
    y = theQuat[1];
    z = theQuat[2];
    w = theQuat[3];
    
    // Initialize the matrix with zeroes and set the homogeneous
    // coordinate scale to one
    clear();
    data[3][3] = 1.0;
    
    // Compute the rotation matrix; the formula for doing this should
    // be available from any decent source of information about
    // quaternions.
    data[0][0] = 1.0 - (2.0 * VS_SQR(y)) - (2.0 * VS_SQR(z));
    data[0][1] = (2.0 * x * y) - (2.0 * w * z);
    data[0][2] = (2.0 * x * z) + (2.0 * w * y);
    data[1][0] = (2.0 * x * y) + (2.0 * w * z);
    data[1][1] = 1.0 - (2.0 * VS_SQR(x)) - (2.0 * VS_SQR(z));
    data[1][2] = (2.0 * y * z) - (2.0 * w * x);
    data[2][0] = (2.0 * x * z) - (2.0 * w * y);
    data[2][1] = (2.0 * y * z) + (2.0 * w * x);
    data[2][2] = 1.0 - (2.0 * VS_SQR(x)) - (2.0 * VS_SQR(y));
}

// ------------------------------------------------------------------------
// Sets this matrix to a translation matrix
// ------------------------------------------------------------------------
void vsMatrix::setTranslation(double dx, double dy, double dz)
{
    int loop;
    
    // Initialize the matrix to an identity matrix
    clear();
    for (loop = 0; loop < 4; loop++)
        data[loop][loop] = 1.0;

    // Copy the translation values to the translation entries in the
    // matrix
    data[0][3] = dx;
    data[1][3] = dy;
    data[2][3] = dz;
}

// ------------------------------------------------------------------------
// Sets this matrix to a (not necessarily uniform) scaling matrix
// ------------------------------------------------------------------------
void vsMatrix::setScale(double sx, double sy, double sz)
{
    // Clear the matrix, and set the values along the diagonal to the
    // desired scale values
    clear();
    data[0][0] = sx;
    data[1][1] = sy;
    data[2][2] = sz;
    data[3][3] = 1.0;
}

// ------------------------------------------------------------------------
// Retrieves one row of the matrix as a vsVector reference. Useful in
// conjunction with the vsVector's operator[] to access one specific
// element of the matrix.
// ------------------------------------------------------------------------
vsVector &vsMatrix::operator[](int index)
{
    // Bounds checking
    if ((index < 0) || (index >= 4))
    {
        printf("vsMatrix::operator[]: Invalid index\n");
        return data[0];
    }
    
    // Return the desired row
    return data[index];
}

// ------------------------------------------------------------------------
// Adds the specified matrix to this matrix, returning the result
// Equivalent to getSum(addend)
// ------------------------------------------------------------------------
vsMatrix vsMatrix::operator+(vsMatrix addend)
{
    int i, j;
    vsMatrix result;
    
    // Create the target matrix by adding each element of this matrix
    // to each element of the addend matrix
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] + addend[i][j];

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Subtracts the specified matrix from this matrix, returning the result
// Equivalent to getDifference(subtrahend)
// ------------------------------------------------------------------------
vsMatrix vsMatrix::operator-(vsMatrix subtrahend)
{
    int i, j;
    vsMatrix result;
    
    // Create the target matrix by subtracting each element of the
    // subtrahend from each element of this matrix
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] - subtrahend[i][j];

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Multiplies this matrix with the given matrix; the operand matrix is
// considered to be on the right. The result is returned.
// Equivalent to getPostMultiplied(operand)
// ------------------------------------------------------------------------
vsMatrix vsMatrix::operator*(vsMatrix operand)
{
    vsMatrix result;
    int i, j, k;
    
    // Do a matrix-multiply operation between this matrix and the operand
    // matrix, with this matrix first; place the results in the target
    // matrix.
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (data[i][k] * operand[k][j]);
        }

    // Return the target matrix
    return result;
}

// ------------------------------------------------------------------------
// Adds the specified matrix to this matrix, keeping the result
// Equivalent to add(addend)
// ------------------------------------------------------------------------
void vsMatrix::operator+=(vsMatrix addend)
{
    int i, j;
    
    // Add each element of the addend matrix to this matrix
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            data[i][j] += addend[i][j];
}

// ------------------------------------------------------------------------
// Subtracts the specified matrix from this matrix, keeping the result
// Equivalent to subtract(subtrahend)
// ------------------------------------------------------------------------
void vsMatrix::operator-=(vsMatrix subtrahend)
{
    int i, j;
    
    // Subtract each element of the subtrahend matrix from this matrix
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            data[i][j] -= subtrahend[i][j];
}

// ------------------------------------------------------------------------
// Prints the specified row of the matrix to stdout
// ------------------------------------------------------------------------
void vsMatrix::printRow(int rowNum)
{
    // Make sure the row number is valid
    if ((rowNum < 0) || (rowNum >= 4))
    {
        printf("vsMatrix::printRow:  Invalid row specified\n");
        return;
    }

    // Assume that the matrix is an affine transform matrix, which
    // generally doesn't have large numbers in it.  Doing so allows
    // us to have an idea how wide the matrix's columns will be.
    printf("%8.4lf%8.4lf%8.4lf%8.4lf", data[rowNum][0], data[rowNum][1],
        data[rowNum][2], data[rowNum][3]);
}

// ------------------------------------------------------------------------
// Prints the specified row of the matrix to the specfied file
// ------------------------------------------------------------------------
void vsMatrix::printRow(int rowNum, FILE *fp)
{
    // Make sure the row number is valid
    if ((rowNum < 0) || (rowNum >= 4))
    {
        printf("vsMatrix::printRow:  Invalid row specified\n");
        return;
    }

    // Assume that the matrix is an affine transform matrix, which
    // generally doesn't have large numbers in it.  Doing so allows
    // us to have an idea how wide the matrix's columns will be.
    fprintf(fp, "%8.4lf%8.4lf%8.4lf%8.4lf", data[rowNum][0], 
        data[rowNum][1], data[rowNum][2], data[rowNum][3]);
}

// ------------------------------------------------------------------------
// Prints a representation of the matrix to stdout
// ------------------------------------------------------------------------
void vsMatrix::print()
{
    int i;

    // Call printRow() for each of the four rows of the matrix.  Add a
    // newline after each row.
    for (i = 0; i < 4; i++)
    {
        printRow(i);
        printf("\n");
    }
}

// ------------------------------------------------------------------------
// Prints a representation of the matrix to the specfied file
// ------------------------------------------------------------------------
void vsMatrix::print(FILE *fp)
{
    int i;

    // Call printRow() for each of the four rows of the matrix.  Add a
    // newline after each row.
    for (i = 0; i < 4; i++)
    {
        printRow(i, fp);
        fprintf(fp, "\n");
    }
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two matricies. Two elements
// are considered equal if they are within a small default tolerance value.
// Equivalent to isEqual(operand)
// ------------------------------------------------------------------------
int vsMatrix::operator==(vsMatrix operand)
{
    int i, j;

    // Check each pair of values (this object's value and the operand's
    // value) for almost-equality; return false if a pair doesn't match up.
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (fabs(data[i][j] - operand[i][j]) > VS_DEFAULT_TOLERANCE)
                return VS_FALSE;

    // If all the pairs match, return true
    return VS_TRUE;
}
