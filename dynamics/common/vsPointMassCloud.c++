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
//    VESS Module:  vsPointMassCloud.c++
//
//    Description:  This subclass of vsMassProperties represents mass
//                  properties of an object as a cloud of point masses.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#include "vsPointMassCloud.h++"

#include "atVector.h++"
#include "stdlib.h"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsPointMassCloud::vsPointMassCloud()
{
    // Begin with an empty point list.
    pointList = new atList();

    // Until a point is added, use the default inertial conditions (a sphere
    // with a radius 1 meter and a mass of 1 kilogram).
    centerOfMass.set(0.0, 0.0, 0.0, 1.0);
    inertiaMatrix = getDefaultInertiaMatrix();

    // The conditions are valid for now.
    inertiaValid = true;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsPointMassCloud::~vsPointMassCloud()
{
    // Free up the mass points.
    delete pointList;
}

// ------------------------------------------------------------------------
// Add a new point mass to the composite moment of inertia of this object.
// The position should be relative to the origin of the object's coordinate
// space.
// ------------------------------------------------------------------------
void vsPointMassCloud::addPointMass(atVector position, double mass)
{
    atVector *newPoint;

    // Create a composite object representing the position and mass of this
    // new point and add it to the list.
    newPoint = new atVector(position[0], position[1], position[2], mass);
    pointList->addEntry(newPoint);

    // If this is the only point to be added, special handling is required.
    if (pointList->getNumEntries() == 1)
    {
        // If this is the first point to be added, continue to use the unit
        // sphere as the geometric basis for the inertia calculation, but
        // adjust the mass to the value of this point. Note that the inertia
        // matrix remains valid at this point.
        centerOfMass.set(position[0], position[1], position[2], mass);
        inertiaMatrix.scale(mass);
    }
    else
    {
        // Update the center of mass with the new point. For each axis,
        // calculate the new weighted average, then set the fourth parameter to
        // the new sum of masses.
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
}

// ------------------------------------------------------------------------
// Clear the mass of this object.
// ------------------------------------------------------------------------
void vsPointMassCloud::clear()
{
    // Remove all points from the list.
    pointList->removeAllEntries();

    // Until a point is added, use the default inertial conditions (a sphere
    // with a radius 1 meter and a mass of 1 kilogram).
    centerOfMass.set(0.0, 0.0, 0.0, 1.0);
    inertiaMatrix = getDefaultInertiaMatrix();

    // The conditions are valid for now.
    inertiaValid = true;
}

// ------------------------------------------------------------------------
// This function returns a vector containing the center of all of the point
// masses that have been added (relative to the origin of the object). The
// fourth parameter contains the magnitude of that mass.
// ------------------------------------------------------------------------
atVector vsPointMassCloud::getCenterOfMass()
{
    return centerOfMass;
}

// ------------------------------------------------------------------------
// This function returns a vector containing the center of all of the point
// masses that have been added (relative to the origin of the object). The
// fourth parameter contains the magnitude of that mass.
// ------------------------------------------------------------------------
atMatrix vsPointMassCloud::getInertiaMatrix()
{
    atVector *pointData;
    atVector pointDelta;
    int i;

    // See if the moment of inertia matrix needs to be calculated. The only way
    // this can become false is if the cloud size is two or greater, so the
    // zero check does not need to be performed if this condition is met.
    if (!inertiaValid)
    {
        // Clear the old inertia matrix.
        inertiaMatrix.clear();

        // Sum all of the points.
        pointData = (atVector *)pointList->getFirstEntry();
        while (pointData)
        {
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
            inertiaMatrix[0][0] += pointData->getValue(3) *
                ((pointDelta[AT_Y] * pointDelta[AT_Y]) +
                (pointDelta[AT_Z] * pointDelta[AT_Z]));
            inertiaMatrix[1][1] += pointData->getValue(3) *
                ((pointDelta[AT_X] * pointDelta[AT_X]) +
                (pointDelta[AT_Z] * pointDelta[AT_Z]));
            inertiaMatrix[2][2] += pointData->getValue(3) *
                ((pointDelta[AT_X] * pointDelta[AT_X]) +
                (pointDelta[AT_Y] * pointDelta[AT_Y]));

            // Accumulate the negative products for the upper off-diagonals.
            inertiaMatrix[0][1] -= pointData->getValue(3) *
                pointDelta[AT_X] * pointDelta[AT_Y];
            inertiaMatrix[0][2] -= pointData->getValue(3) *
                pointDelta[AT_X] * pointDelta[AT_Z];
            inertiaMatrix[1][2] -= pointData->getValue(3) *
                pointDelta[AT_Y] * pointDelta[AT_Z];

            // Move on to the next entry in the list.
            pointData = (atVector *)pointList->getNextEntry();
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

