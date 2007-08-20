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
//    VESS Module:  vsMassProperties.c++
//
//    Description:  
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsMassProperties.h++"

#include "stdlib.h"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsMassProperties::vsMassProperties()
{
    // Begin with an empty vsGrowableArray.
    pointArray = new vsGrowableArray(0, 5);
    pointCount = 0;

    // Start with the center at (0, 0, 0) with a magnitude of 0.0.
    centerOfMass.set(0.0, 0.0, 0.0, 0.0);

    // Until a point is added, the default inertia matrix is valid.
    inertiaValid = true;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsMassProperties::~vsMassProperties()
{
    // Free up the mass points.
    delete pointArray;
}

// ------------------------------------------------------------------------
// Clear the mass of this object.
// ------------------------------------------------------------------------
void vsMassProperties::clear()
{
    vsVector *pointMass;
    int i;

    // Iterate through the array, freeing the vectors.
    for (i = 0; i < pointCount; i++)
    {
        // Fetch the vector pointer.
        pointMass = (vsVector *)pointArray->getData(i);

        // Free the memory used for the vector.
        free(pointMass);
    }

    // Set pointCount to 0, but leave the memory allocated in case it is
    // needed repeatedly. FIXME: This should probably be an option.
    pointCount = 0;

    // Reset the center of mass.
    centerOfMass.clear();

    // Independent of the previous state, this empty matrix is now valid again.
    inertiaValid = true;
}

// ------------------------------------------------------------------------
// Add a new point mass to the composite moment of inertia of this object.
// The position should be relative to the origin of the object's coordinate
// space.
// ------------------------------------------------------------------------
void vsMassProperties::addPointMass(vsVector position, double mass)
{
    // Store the magnitude of the mass in the fourth parameter.
    position.setSize(4);
    position[3] = mass;

    // Store the new vsVector in the point array and increment the point count.
    pointArray->setData(pointCount, new vsVector(position));
    pointCount++;

    // Update the center of mass with the new point. For each axis, calculate
    // the new weighted average, then set the fourth parameter to the new sum
    // of masses.
    centerOfMass[0] = ((centerOfMass[3] * centerOfMass[0]) +
        (mass * position[0])) / (centerOfMass[3] + mass);
    centerOfMass[1] = ((centerOfMass[3] * centerOfMass[1]) +
        (mass * position[1])) / (centerOfMass[3] + mass);
    centerOfMass[2] = ((centerOfMass[3] * centerOfMass[2]) +
        (mass * position[2])) / (centerOfMass[3] + mass);
    centerOfMass[3] += mass;

    // Mark that the moment of inertia matrix is no longer valid.
    inertiaValid = false;
}

// ------------------------------------------------------------------------
// This function returns a vector containing the center of all of the point
// masses that have been added (relative to the origin of the object). The
// fourth parameter contains the magnitude of that mass.
// ------------------------------------------------------------------------
vsVector vsMassProperties::getCenterOfMass()
{
    return centerOfMass;
}

// ------------------------------------------------------------------------
// This function returns a vector containing the center of all of the point
// masses that have been added (relative to the origin of the object). The
// fourth parameter contains the magnitude of that mass.
// ------------------------------------------------------------------------
vsMatrix vsMassProperties::getInertiaMatrix()
{
    vsVector *pointData;
    vsVector pointDelta;
    int i;

    // See if the moment of inertia matrix needs to be calculated.
    if (!inertiaValid)
    {
        // Clear the old inertia matrix.
        inertiaMatrix.clear();

        // Sum all of the points.
        for (i = 0; i < pointCount; i++)
        {
            // Fetch the point from the array.
            pointData = (vsVector *)pointArray->getData(i);

            // Calculate the difference to the point.
            pointDelta = pointData->getDifference(centerOfMass);

            // Adjust the moment of inertia matrix according to the following:
            //              /(y^2 + z^2)      -xy         -xz    \
            //        __   |                                      |
            // I(t) = \    |     -xy      (x^2 + z^2)     -yz     |
            //        /_ M |                                      |
            //              \    -xz          -yz     (x^2 + y^2)/
            //
            // for discrete calculation of the moment of inertia of a rigid
            // body, where M is the mass of a given point and x, y, and z
            // represent the difference between that point and the center of
            // mass. (ACM, SIGGraph 94, Course Notes 32, Section 2.9).

            // Accumulate over the diagonals.
            inertiaMatrix[0][0] += (*pointData)[3] *
                ((pointDelta[VS_Y] * pointDelta[VS_Y]) +
                (pointDelta[VS_Z] * pointDelta[VS_Z]));
            inertiaMatrix[1][1] += (*pointData)[3] *
                ((pointDelta[VS_X] * pointDelta[VS_X]) +
                (pointDelta[VS_Z] * pointDelta[VS_Z]));
            inertiaMatrix[2][2] += (*pointData)[3] *
                ((pointDelta[VS_X] * pointDelta[VS_X]) +
                (pointDelta[VS_Y] * pointDelta[VS_Y]));

            // Accumulate the negative products for the upper off-diagonals.
            inertiaMatrix[0][1] -= (*pointData)[3] *
                pointDelta[VS_X] * pointDelta[VS_Y];
            inertiaMatrix[0][2] -= (*pointData)[3] *
                pointDelta[VS_X] * pointDelta[VS_Z];
            inertiaMatrix[1][2] -= (*pointData)[3] *
                pointDelta[VS_Y] * pointDelta[VS_Z];
        }

        // The off-diagonals mirror one another. Copy the upper elements into
        // the lower elements.
        inertiaMatrix[1][0] = inertiaMatrix[0][1];
        inertiaMatrix[2][0] = inertiaMatrix[0][2];
        inertiaMatrix[2][1] = inertiaMatrix[1][2];

        // Mark that the moment of inertia matrix is again valid.
        inertiaValid = true;
    }

    // Return the matrix.
    return inertiaMatrix;
}

