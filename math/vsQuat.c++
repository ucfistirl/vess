// File vsQuat.c++

#include "vsQuat.h++"

#include <math.h>
#include <stdio.h>
#include "vsGlobals.h++"

// ------------------------------------------------------------------------
// Default constructor - Clears the quaternion to zero.
// ------------------------------------------------------------------------
vsQuat::vsQuat()
{
    clear();
}

// ------------------------------------------------------------------------
// Data constructor - Sets the quaternion to the given data.
// ------------------------------------------------------------------------
vsQuat::vsQuat(double x, double y, double z, double w)
{
    set(x, y, z, w);
}

// ------------------------------------------------------------------------
// Data constructor - Sets the quaternion to the given data array.
// ------------------------------------------------------------------------
vsQuat::vsQuat(double values[])
{
    set(values);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsQuat::~vsQuat()
{
}

// ------------------------------------------------------------------------
// Sets the quaternion to the given data.
// ------------------------------------------------------------------------
void vsQuat::set(double x, double y, double z, double w)
{
    data[0] = x;
    data[1] = y;
    data[2] = z;
    data[3] = w;
}

// -------------------------------------------------------------------------
// Sets the quaternion to the given data array
// ------------------------------------------------------------------------
void vsQuat::set(double values[])
{
    for (int i = 0; i < 4; i++)
        data[i] = values[i];
}

// -------------------------------------------------------------------------
// Makes this quaternion an exact duplicate of the source quaternion
// ------------------------------------------------------------------------
void vsQuat::copy(vsQuat source)
{
    (*this) = source;
}

// ------------------------------------------------------------------------
// Sets the quaternion to zero.
// ------------------------------------------------------------------------
void vsQuat::clear()
{
    for (int i = 0; i < 4; i++)
        data[i] = 0.0;
}
    
// ------------------------------------------------------------------------
// Sets one specific data value in the quaternion.
// ------------------------------------------------------------------------
void vsQuat::setValue(int index, double value)
{
    if ((index < 0) || (index > 3))
    {
        printf("vsQuat::setValue: Bad index\n");
        return;
    }
    
    data[index] = value;
}

// ------------------------------------------------------------------------
// Retrieves one specific data value from the quaternion.
// ------------------------------------------------------------------------
double vsQuat::getValue(int index)
{
    if ((index < 0) || (index > 3))
    {
        printf("vsQuat::getValue: Bad index\n");
        return data[0];
    }
    
    return data[index];
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two quaternions. Two elements
// are considered equal if they are within a small default tolerance value
// of each other.
// ------------------------------------------------------------------------
int vsQuat::isEqual(vsQuat operand)
{
    int loop;
    
    for (loop = 0; loop < 4; loop++)
        if (fabs(data[loop] - operand[loop]) > VS_MATH_DEFAULT_TOLERANCE)
            return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two quaternions. Two elements
// are considered equal if they are within the specified tolerance value
// of each other.
// ------------------------------------------------------------------------
int vsQuat::isAlmostEqual(vsQuat operand, double tolerance)
{
    int loop;
    
    for (loop = 0; loop < 4; loop++)
        if (fabs(data[loop] - operand[loop]) > tolerance)
            return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Adds the addend quaternion to this one, storing the result in this
// quaternion.
// ------------------------------------------------------------------------
void vsQuat::add(vsQuat addend)
{
    for (int i = 0; i < 4; i++)
        data[i] += addend[i];
}

// ------------------------------------------------------------------------
// Adds the addend quaternion to this one, returning the result.
// ------------------------------------------------------------------------
vsQuat vsQuat::getSum(vsQuat addend)
{
    int loop;
    vsQuat result;
    
    for (loop = 0; loop < 4; loop++)
        result[loop] = data[loop] + addend[loop];

    return result;
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend quaternion from this one, storing the result
// in this quaternion.
// ------------------------------------------------------------------------
void vsQuat::subtract(vsQuat subtrahend)
{
    for (int i = 0; i < 4; i++)
        data[i] -= subtrahend[i];
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend quaternion from this one, returning the result.
// ------------------------------------------------------------------------
vsQuat vsQuat::getDifference(vsQuat subtrahend)
{
    int loop;
    vsQuat result;
    
    for (loop = 0; loop < 4; loop++)
        result[loop] = data[loop] - subtrahend[loop];

    return result;
}

// ------------------------------------------------------------------------
// Multiplies each element of this quaternion by the given scalar, storing
// the result in this quaternion.
// ------------------------------------------------------------------------
void vsQuat::scale(double multiplier)
{
    for (int i = 0; i < 4; i++)
        data[i] *= multiplier;
}

// ------------------------------------------------------------------------
// Multiplies each element of this quaternion by the given scalar,
// returning the result.
// ------------------------------------------------------------------------
vsQuat vsQuat::getScaled(double multiplier)
{
    int loop;
    vsQuat result;
    
    for (loop = 0; loop < 4; loop++)
        result[loop] = data[loop] * multiplier;

    return result;
}

// ------------------------------------------------------------------------
// Multiplies this quaternion by the operand quaternion, storing the result
// in this quaternion. Quaternions are multiplied using the equation:
//
//  qq' = [V, w] * [V', w']
//      = [VxV' + wV' + w'V, ww' - V.V']
//
// Where q = [V, w], q' = [V', w'], x denotes cross product, and . denotes
// dot product.
// ------------------------------------------------------------------------
void vsQuat::multiplyQuat(vsQuat operand)
{
    int loop;
    vsQuat tempQuat, resultQuat;
    vsVector myVec, operandVec, crossVec;
    double dotProd;
    
    // Find the cross product of the vector part of both quaternions (VxV')
    myVec.set(data[0], data[1], data[2]);
    operandVec.set(operand[0], operand[1], operand[2]);
    crossVec = myVec.getCrossProduct(operandVec);
    // Vector part total = VxV'
    resultQuat.set(crossVec[0], crossVec[1], crossVec[2], 0.0);
    
    // Add on the rest of the terms for the vector part
    // Find wV'
    tempQuat = operand;
    tempQuat[3] = 0.0;
    tempQuat.scale(data[3]);
    // Vector part total = VxV' + wV'
    resultQuat = resultQuat + tempQuat;
    
    // Find w'V
    tempQuat.set(data[0], data[1], data[2], 0.0);
    tempQuat.scale(operand[3]);
    // Vector part total = VxV' + wV' + w'V
    resultQuat = resultQuat + tempQuat;
    
    // Compute the scalar part
    // Find V.V'
    dotProd = myVec.getDotProduct(operandVec);
    // Scalar part total = ww' - V.V'
    resultQuat[3] = (data[3] * operand[3]) - dotProd;
    
    for (loop = 0; loop < 4; loop++)
        data[loop] = resultQuat[loop];
}

// ------------------------------------------------------------------------
// Multiplies this quaternion by the operand quaternion, returning the
// result. This function 'cheats' by just calling multiplyQuat for it's
// answer.
// ------------------------------------------------------------------------
vsQuat vsQuat::getMultipliedQuat(vsQuat operand)
{
    vsQuat result(data);
    result.multiplyQuat(operand);
    return result;
}

// ------------------------------------------------------------------------
// Returns the magnitude of this quaternion.
// ------------------------------------------------------------------------
double vsQuat::getMagnitude()
{
    double result = 0.0;

    for (int i = 0; i < 4; i++)
        result += VS_SQR(data[i]);

    return sqrt(result);
}

// ------------------------------------------------------------------------
// Returns a normalized version of this quaternion.
// ------------------------------------------------------------------------
vsQuat vsQuat::getNormalized()
{
    vsQuat result;
    double mag;
    int loop;
    
    mag = getMagnitude();
    for (loop = 0; loop < 4; loop++)
        result[loop] = data[loop] / mag;

    return result;
}

// ------------------------------------------------------------------------
// Normalizes this quaternion, keeping the result.
// ------------------------------------------------------------------------
void vsQuat::normalize()
{
    double mag;
    int loop;
    
    mag = getMagnitude();
    for (loop = 0; loop < 4; loop++)
        data[loop] /= mag;
}

// ------------------------------------------------------------------------
// Conjugates this quaternion, keeping the result. Quaternion conjugation
// negates the vector portion but leaves the scalar portion unchanged.
// If the quaternion represents a rotation, the conjugate is the opposite
// rotation.
// ------------------------------------------------------------------------
void vsQuat::conjugate()
{
    int loop;
    
    for (loop = 0; loop < 3; loop++)
        data[loop] = -(data[loop]);
}

// ------------------------------------------------------------------------
// Conjugates this quaternion, returning the result. Quaternion conjugation
// negates the vector portion but leaves the scalar portion unchanged.
// If the quaternion represents a rotation, the conjugate is the opposite
// rotation.
// ------------------------------------------------------------------------
vsQuat vsQuat::getConjugate()
{
    vsQuat result;
    int loop;
    
    for (loop = 0; loop < 4; loop++)
    {
        if (loop == 3)
            result[loop] = data[loop];
        else
            result[loop] = -data[loop];
    }

    return result;
}

// ------------------------------------------------------------------------
// Sets this quaternion to its multiplicative inverse. The inverse of a
// quaternion is its conjugate divided by the square of its magnitude.
// ------------------------------------------------------------------------
void vsQuat::invert()
{
    double mag;
    int loop;
    
    mag = getMagnitude();

    conjugate();
    for (loop = 0; loop < 4; loop++)
        data[loop] /= (mag * mag);
}

// ------------------------------------------------------------------------
// Returns the multiplicative inverse of this quaternion. The inverse of a
// quaternion is its conjugate divided by the square of its magnitude.
// ------------------------------------------------------------------------
vsQuat vsQuat::getInverse()
{
    vsQuat result(data);
    result.invert();
    return result;
}

// ------------------------------------------------------------------------
// Sets this quaternion to a rotational quaternion representing the same
// rotation as what is stored within the matrix parameter.
// ------------------------------------------------------------------------
void vsQuat::setMatrixRotation(vsMatrix theMatrix)
{
    double ws, xs, ys;

    ws = (1.0 + theMatrix[0][0] + theMatrix[1][1] + theMatrix[2][2]) / 4.0;
    if (ws > 1E-6)
    {
        data[3] = sqrt(ws);
        data[0] = (theMatrix[2][1] - theMatrix[1][2]) / (4.0 * data[3]);
        data[1] = (theMatrix[0][2] - theMatrix[2][0]) / (4.0 * data[3]);
        data[2] = (theMatrix[1][0] - theMatrix[0][1]) / (4.0 * data[3]);
    }
    else
    {
        data[3] = 0.0;
        xs = -(theMatrix[1][1] + theMatrix[2][2]) / 2.0;
        if (xs > 1E-6)
        {
            data[0] = sqrt(xs);
            data[1] = theMatrix[1][0] / (2.0 * data[0]);
            data[2] = theMatrix[2][0] / (2.0 * data[0]);
        }
        else
        {
            data[0] = 0.0;
            ys = (1.0 - theMatrix[2][2]) / 2.0;
            if (ys > 1E-6)
            {
                data[1] = sqrt(ys);
                data[2] = theMatrix[2][1] / (2.0 * data[1]);
            }
            else
            {
                data[1] = 0.0;
                data[2] = 1.0;
            }
        }
    }

}

// ------------------------------------------------------------------------
// Sets this quaternion to a rotational quaternion representing the same
// rotation as the given three Euler angles. The axis constant specifies
// the order of the axes for the Euler angles.
// ------------------------------------------------------------------------
void vsQuat::setEulerRotation(vsMathEulerAxisOrder axisOrder,
    double axis1Degrees, double axis2Degrees, double axis3Degrees)
{
    vsQuat firstQuat, secondQuat, thirdQuat, resultQuat;
    
    switch (axisOrder)
    {
        case VS_EULER_ANGLES_XYZ_S:
        case VS_EULER_ANGLES_XYZ_R:
            firstQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_XZY_S:
        case VS_EULER_ANGLES_XZY_R:
            firstQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_YXZ_S:
        case VS_EULER_ANGLES_YXZ_R:
            firstQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_YZX_S:
        case VS_EULER_ANGLES_YZX_R:
            firstQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_ZXY_S:
        case VS_EULER_ANGLES_ZXY_R:
            firstQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_ZYX_S:
        case VS_EULER_ANGLES_ZYX_R:
            firstQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis3Degrees);
            break;

        case VS_EULER_ANGLES_XYX_S:
        case VS_EULER_ANGLES_XYX_R:
            firstQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_XZX_S:
        case VS_EULER_ANGLES_XZX_R:
            firstQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_YXY_S:
        case VS_EULER_ANGLES_YXY_R:
            firstQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_YZY_S:
        case VS_EULER_ANGLES_YZY_R:
            firstQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_ZXZ_S:
        case VS_EULER_ANGLES_ZXZ_R:
            firstQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(1.0, 0.0, 0.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis3Degrees);
            break;
        case VS_EULER_ANGLES_ZYZ_S:
        case VS_EULER_ANGLES_ZYZ_R:
            firstQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis1Degrees);
            secondQuat.setAxisAngleRotation(0.0, 1.0, 0.0, axis2Degrees);
            thirdQuat.setAxisAngleRotation(0.0, 0.0, 1.0, axis3Degrees);
            break;

        default:
            printf("vsQuat::setEulerRotation: Invalid axis order value\n");
            return;
    }
    
    // I _think_ quaternions are multiplied in the same way as
    // rotation matricies; since the point to be rotated gets multiplied
    // as a column vector on the right, the first quaternion that
    // would get multiplied in should be on the right as well.
    // If this is a relative axis rotation, then that order is reversed.
    if ((axisOrder >= VS_EULER_ANGLES_XYZ_R) &&
        (axisOrder <= VS_EULER_ANGLES_ZYZ_R))
        // reversed
        (*this) = firstQuat * secondQuat * thirdQuat;
    else
        // standard
        (*this) = thirdQuat * secondQuat * firstQuat;
}

// ------------------------------------------------------------------------
// Retrieves the rotation indicated by this quaternion as a set of three
// Euler angles. The axis constant specifies the axis order of the
// resulting reconstructed rotations.
//
// Note: NULL pointers may be passed in to denote unwanted return values
// ------------------------------------------------------------------------
void vsQuat::getEulerRotation(vsMathEulerAxisOrder axisOrder,
    double *axis1Degrees, double *axis2Degrees, double *axis3Degrees)
{
    // Cheat: call the matrix version of this routine and return
    // whatever that returns.
    vsMatrix theMatrix;
    
    theMatrix.setQuatRotation(*this);
    theMatrix.getEulerRotation(axisOrder, axis1Degrees, axis2Degrees,
        axis3Degrees);
}

// ------------------------------------------------------------------------
// Sets this quaternion to a rotational quaternion representing a rotation
// around the axis specified by the vector (x, y, z), and rotating by
// the specified number of degrees.
// If the axis passed in is all zero, the final quaternion will have no
// rotation, regardless of the degrees specified.
// ------------------------------------------------------------------------
void vsQuat::setAxisAngleRotation(double x, double y, double z,
    double rotDegrees)
{
    vsVector axis(x, y, z);

    if ((x == 0.0) && (y == 0.0) && (z == 0.0))
    {
        data[0] = 0.0;
        data[1] = 0.0;
        data[2] = 0.0;
        data[3] = 1.0;
        return;
    }

    axis.normalize();
    data[0] = axis[0] * sin(VS_DEG2RAD(rotDegrees / 2.0));
    data[1] = axis[1] * sin(VS_DEG2RAD(rotDegrees / 2.0));
    data[2] = axis[2] * sin(VS_DEG2RAD(rotDegrees / 2.0));
    data[3] = cos(VS_DEG2RAD(rotDegrees / 2.0));
}

// ------------------------------------------------------------------------
// Retrieves the axis and amount of rotation represented by this
// quaternion.
//
// Note: NULL pointers may be passed in to denote unwanted return values
// ------------------------------------------------------------------------
void vsQuat::getAxisAngleRotation(double *x, double *y, double *z,
    double *rotDegrees)
{
    vsVector axis;
    double mag, degrees;
    
    axis.set(data[0], data[1], data[2]);
    mag = axis.getMagnitude();
    if (mag < 1E-6)
    {
	if (x)
	    *x = 0.0;
	if (y)
	    *y = 0.0;
	if (z)
	    *z = 0.0;
	if (rotDegrees)
	    *rotDegrees = 0.0;
	return;
    }
    
    axis.normalize();
    degrees = VS_RAD2DEG(acos(data[3]) * 2.0);
    
    if (x)
	*x = axis[0];
    if (y)
	*y = axis[1];
    if (z)
	*z = axis[2];
    if (rotDegrees)
	*rotDegrees = degrees;
}

// ------------------------------------------------------------------------
// Sets this quaternion to represent the coordinate space rotation that
// will rotate the directions specified by originForward and originUp to
// match those specified by targetForward and targetUp, respectively.
// ------------------------------------------------------------------------
void vsQuat::setVecsRotation(vsVector originForward, vsVector originUp,
    vsVector targetForward, vsVector targetUp)
{
    vsVector startDir, startUp, endDir, endUp;
    vsVector newUp;
    vsVector rotAxis;
    double rotAngle, dotProd;
    vsVector componentVec;
    vsQuat roll;

    startDir.clearCopy(originForward);
    startDir.setSize(3);
    startDir.normalize();
    startUp.clearCopy(originUp);
    startUp.setSize(3);
    startUp.normalize();
    endDir.clearCopy(targetForward);
    endDir.setSize(3);
    endDir.normalize();
    endUp.clearCopy(targetUp);
    endUp.setSize(3);
    endUp.normalize();

    set(0.0, 0.0, 0.0, 1.0);

    // First, rotate the forward directions to match
    if (!(startDir == endDir))
    {
        rotAxis = startDir.getCrossProduct(endDir);
        rotAngle = startDir.getAngleBetween(endDir);
        setAxisAngleRotation(rotAxis[0], rotAxis[1], rotAxis[2], rotAngle);
    }

    // Second, with both forward directions aligned, roll the up
    // directions to match
    newUp = (*this).rotatePoint(startUp);
    if (!(newUp == endUp) && !(startDir == startUp) && !(endDir == endUp))
    {
        dotProd = endDir.getDotProduct(newUp);
        componentVec = endDir * dotProd;
        newUp -= componentVec;
        newUp.normalize();

        dotProd = endDir.getDotProduct(endUp);
        componentVec = endDir * dotProd;
        endUp -= componentVec;
        endUp.normalize();

        rotAxis = newUp.getCrossProduct(endUp);
        rotAngle = newUp.getAngleBetween(endUp);

        roll.setAxisAngleRotation(rotAxis[0], rotAxis[1], rotAxis[2], rotAngle);
        (*this) = roll * (*this);
    }
}

// ------------------------------------------------------------------------
// Transforms the given point by this quaternion as a rotation. Equivalent
// to changing the quaternion into a rotation matrix and multiplying the
// point by the resulting matrix. The homogeneous coordinate value w of
// the point, if it exists, is ignored.
// ------------------------------------------------------------------------
vsVector vsQuat::rotatePoint(vsVector targetPoint)
{
    vsVector resultPt;
    vsQuat targetAsQuat, conjQuat, resultQuat;
    int loop;
    
    conjQuat.set(data);
    conjQuat.conjugate();

    targetAsQuat.clear();
    for (loop = 0; (loop < targetPoint.getSize()) && (loop < 3); loop++)
        targetAsQuat[loop] = targetPoint[loop];

    resultQuat = (*this) * targetAsQuat * conjQuat;
    
    resultPt = targetPoint;
    for (loop = 0; (loop < targetPoint.getSize()) && (loop < 3); loop++)
        resultPt[loop] = resultQuat[loop];

    return resultPt;
}

// ------------------------------------------------------------------------
// Spherical Linear intERPolation
// Returns a quaternion that is an interpolation between this quaternion
// as the source point and the destination quaternion. The parameter value
// should range from 0.0 to 1.0, inclusive, and denotes the interpolation
// value. The path taken by the interpolated quaternion is linear, but
// the velocity is non-linear (due to the trig functions involved).
// ------------------------------------------------------------------------
vsQuat vsQuat::slerp(vsQuat destination, double parameter)
{
    vsQuat startQuat, endQuat, resultQuat;
    double theta, q1val, q2val;
    int loop;
    
    if ((parameter < 0.0) || (parameter > 1.0))
    {
        printf("vsQuat::slerp: 'parameter' must be in range 0.0 - 1.0\n");
        return resultQuat;
    }

    theta = 0.0;
    for (loop = 0; loop < 4; loop++)
        theta += (data[loop] * destination[loop]);
    theta = acos(theta);
    
    startQuat.set(data);
    q1val = sin((1.0 - parameter) * theta) / sin(theta);
    startQuat.scale(q1val);
    
    endQuat = destination;
    q2val = sin(parameter * theta) / sin(theta);
    endQuat.scale(q2val);
    
    resultQuat = startQuat + endQuat;
    return resultQuat;
}

// ------------------------------------------------------------------------
// Retrieves one value from the quaternion as a reference to a double.
// ------------------------------------------------------------------------
double &vsQuat::operator[](int index)
{
    if ((index < 0) || (index > 3))
    {
        printf("vsQuat::operator[]: Illegal subscript\n");
        return data[0];
    }
    
    return data[index];
}

// ------------------------------------------------------------------------
// Adds the addend quaternion to this one, returning the result.
// Equivalent to getSum(addend)
// ------------------------------------------------------------------------
vsQuat vsQuat::operator+(vsQuat addend)
{
    int loop;
    vsQuat result;
    
    for (loop = 0; loop < 4; loop++)
        result[loop] = data[loop] + addend[loop];

    return result;
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend quaternion from this one, returning the result.
// Equivalent to getDifference(subtrahend)
// ------------------------------------------------------------------------
vsQuat vsQuat::operator-(vsQuat subtrahend)
{
    int loop;
    vsQuat result;
    
    for (loop = 0; loop < 4; loop++)
        result[loop] = data[loop] - subtrahend[loop];

    return result;
}

// ------------------------------------------------------------------------
// Multiplies each element of this quaternion by the given scalar,
// returning the result.
// Equivalent to getScaled(multiplier)
// ------------------------------------------------------------------------
vsQuat vsQuat::operator*(double multiplier)
{
    int loop;
    vsQuat result;
    
    for (loop = 0; loop < 4; loop++)
        result[loop] = data[loop] * multiplier;

    return result;
}

// ------------------------------------------------------------------------
// Multiplies this quaternion by the operand quaternion, returning the
// result. This function 'cheats' by just calling multiplyQuat for it's
// answer.
// Equivalent to getMultipliedQuat(operand)
// ------------------------------------------------------------------------
vsQuat vsQuat::operator*(vsQuat operand)
{
    vsQuat result(data);
    result.multiplyQuat(operand);
    return result;
}

// ------------------------------------------------------------------------
// Adds the addend quaternion to this one, storing the result in this
// quaternion.
// Equivalent to add(addend)
// ------------------------------------------------------------------------
void vsQuat::operator+=(vsQuat addend)
{
    for (int i = 0; i < 4; i++)
        data[i] += addend[i];
}

// ------------------------------------------------------------------------
// Subtracts the subtrahend quaternion from this one, storing the result
// in this quaternion.
// Equivalent to subtract(subtrahend)
// ------------------------------------------------------------------------
void vsQuat::operator-=(vsQuat subtrahend)
{
    for (int i = 0; i < 4; i++)
        data[i] -= subtrahend[i];
}

// ------------------------------------------------------------------------
// Multiplies each element of this quaternion by the given scalar, storing
// the result in this quaternion.
// Equivalent to scale(multiplier)
// ------------------------------------------------------------------------
void vsQuat::operator*=(double multiplier)
{
    for (int i = 0; i < 4; i++)
        data[i] *= multiplier;
}

// ------------------------------------------------------------------------
// Multiplies this quaternion by the operand quaternion, storing the result
// in this quaternion.
// Equivalent to multiplyQuat(operand)
// ------------------------------------------------------------------------
void vsQuat::operator*=(vsQuat operand)
{
    multiplyQuat(operand);
}

// ------------------------------------------------------------------------
// Checks for element-wise equality between two quaternions. Two elements
// are considered equal if they are within a small default tolerance value
// of each other.
// Equivalent to isEqual(operand)
// ------------------------------------------------------------------------
int vsQuat::operator==(vsQuat operand)
{
    int loop;
    
    for (loop = 0; loop < 4; loop++)
        if (fabs(data[loop] - operand[loop]) < VS_MATH_DEFAULT_TOLERANCE)
            return VS_FALSE;

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Related function
// Multiplies each element of the operand quaternion by the given scalar,
// returning the result.
// ------------------------------------------------------------------------
vsQuat operator*(double multiplier, vsQuat operand)
{
    int loop;
    vsQuat result;
    
    for (loop = 0; loop < 4; loop++)
        result[loop] = multiplier * operand[loop];

    return result;
}
