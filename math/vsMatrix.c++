// File vsMatrix.c++

#include <math.h>
#include <stdio.h>
#include "vsGlobals.h++"
#include "vsMatrix.h++"

// ------------------------------------------------------------------------
// Default constructor - Clears the matrix to zero
// ------------------------------------------------------------------------
vsMatrix::vsMatrix()
{
    clear();
}

// ------------------------------------------------------------------------
// Data constructor - Set matrix data to given two-dimensional array
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
// Sets matrix data to given two-dimensional array
// ------------------------------------------------------------------------
void vsMatrix::set(double values[4][4])
{
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
// Sets matrix data to zero
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
    
    data[row][column] = value;
}

// ------------------------------------------------------------------------
// Retrieves one specific data value from matrix
// ------------------------------------------------------------------------
double vsMatrix::getValue(int row, int column)
{
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
    
    return data[row][column];
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two matricies. Two elements
// are considered equal if they are within a small default tolerance value.
// ------------------------------------------------------------------------
int vsMatrix::isEqual(vsMatrix operand)
{
    int i, j;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (fabs(data[i][j] - operand[i][j]) > VS_MATH_DEFAULT_TOLERANCE)
                return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two matricies. Two elements
// are considered equal if they are within the specified tolerance value.
// ------------------------------------------------------------------------
int vsMatrix::isAlmostEqual(vsMatrix operand, double tolerance)
{
    int i, j;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (fabs(data[i][j] - operand[i][j]) > tolerance)
                return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Adds the specified matrix to this matrix, keeping the result
// ------------------------------------------------------------------------
void vsMatrix::add(vsMatrix addend)
{
    int i, j;
    
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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] + addend[i][j];

    return result;
}

// ------------------------------------------------------------------------
// Subtracts the specified matrix from this matrix, keeping the result
// ------------------------------------------------------------------------
void vsMatrix::subtract(vsMatrix subtrahend)
{
    int i, j;
    
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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] - subtrahend[i][j];

    return result;
}

// ------------------------------------------------------------------------
// Multiplies this matrix by the given scalar, keeping the result
// ------------------------------------------------------------------------
void vsMatrix::scale(double multiplier)
{
    int i, j;
    
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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] * multiplier;

    return result;
}

// ------------------------------------------------------------------------
// Transposes this matrix, keeping the result
// ------------------------------------------------------------------------
void vsMatrix::transpose()
{
    int i, j;
    double temp;
    
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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < i; j++)
            result[j][i] = data[i][j];

    return result;
}

// ------------------------------------------------------------------------
// Returns the value of the determinant for this matrix
// ------------------------------------------------------------------------
double vsMatrix::getDeterminant()
{
    int detArray[24][4] = { {0, 1, 2, 3}, {0, 2, 3, 1}, {0, 3, 1, 2},
			    {1, 3, 2, 0}, {1, 0, 3, 2}, {1, 2, 0, 3},
			    {2, 0, 1, 3}, {2, 1, 3, 0}, {2, 3, 0, 1},
			    {3, 2, 1, 0}, {3, 0, 2, 1}, {3, 1, 0, 2},
			    {0, 3, 2, 1}, {0, 1, 3, 2}, {0, 2, 1, 3},
			    {1, 0, 2, 3}, {1, 2, 3, 0}, {1, 3, 0, 2},
			    {2, 3, 1, 0}, {2, 0, 3, 1}, {2, 1, 0, 3},
			    {3, 0, 1, 2}, {3, 1, 2, 0}, {3, 2, 0, 1} };
    double total, intermediate;
    int loop, sloop;
    
    total = 0.0;
    for (loop = 0; loop < 24; loop++)
    {
	intermediate = 1.0;
	for (sloop = 0; sloop < 4; sloop++)
	    intermediate *= data[sloop][detArray[loop][sloop]];
	if (loop > 11)
	    total -= intermediate;
	else
	    total += intermediate;
    }
    
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
    
    (*this) = result;
}

// ------------------------------------------------------------------------
// Returns the inverse matrix of this matrix. Returns a zero matrix if
// this matrix does not have an inverse.
// ------------------------------------------------------------------------
vsMatrix vsMatrix::getInverse()
{
    vsMatrix result;
    
    if (fabs(getDeterminant()) < 1E-6)
    {
	result.clear();
    }
    else
    {
	result = (*this);
	result.invert();
    }
    
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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (operand[i][k] * data[k][j]);
        }

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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (operand[i][k] * data[k][j]);
        }

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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (data[i][k] * operand[k][j]);
        }

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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (data[i][k] * operand[k][j]);
        }

    return result;
}

// ------------------------------------------------------------------------
// Transforms the given point by multiplying this matrix by the point as
// a column vector on the right, returning the result. The fourth element
// of the operand is considered to be one.
// ------------------------------------------------------------------------
vsVector vsMatrix::getPointXform(vsVector operand)
{
    vsVector result;
    int i, j;

    if (operand.getSize() < 3)
    {
        printf("vsMatrix::getPointXForm: Operand vector is too small\n");
        return result;
    }

    for (i = 0; i < 4; i++)
    {
        result[i] = 0.0;
        for (j = 0; j < 4; j++)
        {
            if (j == 3)
                result[i] += data[i][j];
            else
                result[i] += (data[i][j] * operand[j]);
        }
    }
    
    result.setSize(operand.getSize());
    return result;
}

// ------------------------------------------------------------------------
// Transforms the given vector by multiplying this matrix by the vector as
// a column vector on the right, returning the result. The fourth element
// of the operand is considered to be zero.
// ------------------------------------------------------------------------
vsVector vsMatrix::getVectorXform(vsVector operand)
{
    vsVector result;
    int i, j;

    if (operand.getSize() < 3)
    {
        printf("vsMatrix::getVectorXForm: Operand vector is too small\n");
        return result;
    }

    for (i = 0; i < 4; i++)
    {
        result[i] = 0.0;
        for (j = 0; j < 3; j++)
            result[i] += (data[i][j] * operand[j]);
    }
    
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

    if (operand.getSize() < 4)
    {
        printf("vsMatrix::getFullXForm: Operand vector is too small\n");
        return result;
    }

    for (i = 0; i < 4; i++)
    {
        result[i] = 0.0;
        for (j = 0; j < 4; j++)
            result[i] += (data[i][j] * operand[j]);
    }
    
    return result;
}

// ------------------------------------------------------------------------
// Sets this matrix to the identity matrix.
// ------------------------------------------------------------------------
void vsMatrix::setIdentity()
{
    int loop;

    clear();
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
    
    axisDegrees[0] = axis1Degrees;
    axisDegrees[1] = axis2Degrees;
    axisDegrees[2] = axis3Degrees;
    
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
    
    for (loop = 0; loop < 3; loop++)
    {
        axisRotation[loop].clear();
        axisRotation[loop][3][3] = 1.0;

        switch (axis[loop])
        {
            case 0:
                axisRotation[loop][0][0] = 1.0;
                axisRotation[loop][1][1] = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][2][2] = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][2][1] = sin(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][1][2] = -sin(VS_DEG2RAD(axisDegrees[loop]));
                break;
            case 1:
                axisRotation[loop][1][1] = 1.0;
                axisRotation[loop][0][0] = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][2][2] = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][0][2] = sin(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][2][0] = -sin(VS_DEG2RAD(axisDegrees[loop]));
                break;
            case 2:
                axisRotation[loop][2][2] = 1.0;
                axisRotation[loop][0][0] = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][1][1] = cos(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][1][0] = sin(VS_DEG2RAD(axisDegrees[loop]));
                axisRotation[loop][0][1] = -sin(VS_DEG2RAD(axisDegrees[loop]));
                break;
        }
    }

    // Check for relative rotations; reverse the output if so
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

    theQuat = quat.getNormalized();
    x = theQuat[0];
    y = theQuat[1];
    z = theQuat[2];
    w = theQuat[3];
    
    clear();
    data[3][3] = 1.0;
    
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
    
    clear();
    for (loop = 0; loop < 4; loop++)
        data[loop][loop] = 1.0;

    data[0][3] = dx;
    data[1][3] = dy;
    data[2][3] = dz;
}

// ------------------------------------------------------------------------
// Sets this matrix to a (not neccessarily uniform) scaling matrix
// ------------------------------------------------------------------------
void vsMatrix::setScale(double sx, double sy, double sz)
{
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
    if ((index < 0) || (index >= 4))
    {
        printf("vsMatrix::operator[]: Invalid index\n");
        return data[0];
    }
    
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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] + addend[i][j];

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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            result[i][j] = data[i][j] - subtrahend[i][j];

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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            result[i][j] = 0.0;
            for (k = 0; k < 4; k++)
                result[i][j] += (data[i][k] * operand[k][j]);
        }

    return result;
}

// ------------------------------------------------------------------------
// Adds the specified matrix to this matrix, keeping the result
// Equivalent to add(addend)
// ------------------------------------------------------------------------
void vsMatrix::operator+=(vsMatrix addend)
{
    int i, j;
    
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
    
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            data[i][j] -= subtrahend[i][j];
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two matricies. Two elements
// are considered equal if they are within a small default tolerance value.
// Equivalent to isEqual(operand)
// ------------------------------------------------------------------------
int vsMatrix::operator==(vsMatrix operand)
{
    int i, j;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (fabs(data[i][j] - operand[i][j]) < VS_MATH_DEFAULT_TOLERANCE)
                return VS_FALSE;

    return VS_TRUE;
}
