// File vsVecQuat.c++

#include "vsVecQuat.h++"

// ------------------------------------------------------------------------
// Default constructor - Clears the vector and quaternion
// ------------------------------------------------------------------------
vsVecQuat::vsVecQuat()
{
    clear();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsVecQuat::~vsVecQuat()
{
}

// ------------------------------------------------------------------------
// Clears the vector and quaternion
// ------------------------------------------------------------------------
void vsVecQuat::clear()
{
    vector.clear();
    quat.clear();
}

// ------------------------------------------------------------------------
// Treating the vector as a translation and the quaternion as a rotation,
// this function constructs and returns a matrix that contains the
// composite transformation.
// ------------------------------------------------------------------------
vsMatrix vsVecQuat::getAsMatrix()
{
    vsMatrix resultMat;
    int loop;
    
    resultMat.setQuatRotation(quat);
    
    for (loop = 0; (loop < 3) && (loop < vector.getSize()); loop++)
	resultMat[3][loop] = vector[loop];

    return resultMat;
}

// ------------------------------------------------------------------------
// Decomposes the transformation contained within the matrix into a
// translation and a rotation, which are stored in the vector and quat,
// respectively. Scale (uniform and nonuniform) is ignored.
// ------------------------------------------------------------------------
void vsVecQuat::setFromMatrix(vsMatrix theMatrix)
{
    int loop;
    
    vector.setSize(3);
    for (loop = 0; loop < 3; loop++)
	vector[loop] = theMatrix[3][loop];

    quat.setMatrixRotation(theMatrix);
}
