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
//    VESS Module:  vsPhantomCollision.c++
//
//    Description:  Motion model that implements collision detection for
//                  any object and applys forces to a Phantom. Works by
//                  taking a set of designated 'hot' points on an object
//                  and making sure that none of those points pass through
//                  a solid object.
//
//    Author(s):    Bryan Kline, Duvan Cope
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsPhantomCollision.h++"
#include "vsTimer.h++"
#include "vsGeometry.h++"

// ------------------------------------------------------------------------
// Constructor - Sets up the collsion's variables and allocates a 
// vsIntersect object
// ------------------------------------------------------------------------
vsPhantomCollision::vsPhantomCollision(vsPhantomSystem *thePhantomSys,
                                       vsKinematics *objectKin,
                                       vsNode *theScene)
{
    // Store the given kinematics object and scene pointer
    phantomSys = thePhantomSys;
    kinematics = objectKin;
    scene = theScene;

    // Start with zero hot points
    offsetCount = 0;

    // Create and initialize the intersection object
    intersect = new vsSphereIntersect();
    intersect->setSphereListSize(0);
    intersect->setMask(0xffffffff);
    
    // Set the default margin distance
    sphereRadius = VS_PHANTOM_COLLISION_DEFAULT_RADIUS;

    // Set the default maximum force
    maximumForce = VS_PHANTOM_COLLISION_DEFAULT_FORCE;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the internal vsIntersect object
// ------------------------------------------------------------------------
vsPhantomCollision::~vsPhantomCollision()
{
    delete intersect;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsPhantomCollision::getClassName()
{
    return "vsPhantomCollision";
}

// ------------------------------------------------------------------------
// Sets the number of 'hot points' that this collision object uses
// ------------------------------------------------------------------------
void vsPhantomCollision::setPointCount(int numPoints)
{
    // Bounds checking
    if ((numPoints < 0) || (numPoints > VS_PHANTOM_COLLISION_POINTS_MAX))
    {
        printf("vsPhantomCollision::setPointCount: Parameter out of bounds\n");
        return;
    }
    
    // Set the number of hot points, and resize the intersection object's
    // list of segments
    offsetCount = numPoints;
    intersect->setSphereListSize(numPoints);
}

// ------------------------------------------------------------------------
// Gets the number of 'hot points' that this collision object uses
// ------------------------------------------------------------------------
int vsPhantomCollision::getPointCount()
{
    return offsetCount;
}

// ------------------------------------------------------------------------
// Sets the position of one of the hot points of the collision object, in
// the local coordinate system of the associated kinematics' component.
// ------------------------------------------------------------------------
void vsPhantomCollision::setPoint(int index, atVector newOffset)
{
    // Bounds checking
    if ((index < 0) || (index >= VS_PHANTOM_COLLISION_POINTS_MAX))
    {
        printf("vsPhantomCollision::setPoint: Index out of bounds\n");
        return;
    }
    
    // Copy the point data to our offset point list, forcing the size
    // of the vector to be 3.
    (offsetPoints[index]).clearCopy(newOffset);
    (offsetPoints[index]).setSize(3);
}

// ------------------------------------------------------------------------
// Gets the position of one of the hot points of the collision object, in
// the local coordinate system of the associated kinematics' component.
// ------------------------------------------------------------------------
atVector vsPhantomCollision::getPoint(int index)
{
    atVector zero(0.0, 0.0, 0.0);

    // Bounds checking
    if ((index < 0) || (index >= VS_PHANTOM_COLLISION_POINTS_MAX))
    {
        printf("vsPhantomCollision::setPoint: Index out of bounds\n");
        return zero;
    }
    
    // Return the desired point
    return (offsetPoints[index]);
}

// ------------------------------------------------------------------------
// Sets the intersection mask for the collision object. The intersection
// mask is passed directly to the collision's associated vsIntersect 
// object.
// ------------------------------------------------------------------------
void vsPhantomCollision::setIntersectMask(unsigned int newMask)
{
    // Pass the mask to the vsIntersect object
    intersect->setMask(newMask);
}

// ------------------------------------------------------------------------
// Gets the intersection mask for the collision object
// ------------------------------------------------------------------------
unsigned int vsPhantomCollision::getIntersectMask()
{
    // Get the mask from the vsIntersect object
    return (intersect->getMask());
}

// ------------------------------------------------------------------------
// Sets the collision margin for the object. The margin is the closest the
// geometry governed by the collision object can approach any obstacle.
// ------------------------------------------------------------------------
void vsPhantomCollision::setRadius(double newRadius)
{
    sphereRadius = newRadius;
}

// ------------------------------------------------------------------------
// Gets the collision margin for the object
// ------------------------------------------------------------------------
double vsPhantomCollision::getRadius()
{
    return sphereRadius;
}

// ------------------------------------------------------------------------
// Gets the maximum force to apply to the Phantom.
// ------------------------------------------------------------------------
void vsPhantomCollision::setMaxForce(double newMaxForce)
{
    if ((newMaxForce >= 0.0) && (newMaxForce < VS_PHANTOM_COLLISION_MAX_FORCE))
    {
        maximumForce = newMaxForce;
    }
}

// ------------------------------------------------------------------------
// Gets the maximum force to apply to the Phantom.
// ------------------------------------------------------------------------
double vsPhantomCollision::getMaxForce()
{
    return maximumForce;
}

// ------------------------------------------------------------------------
// Updates the collision object by determining the direction of travel of
// the geometry, performing an intersection test to see if that direction
// of travel is clear, and adjusting the direction and/or speed if
// necessary.
// ------------------------------------------------------------------------
void vsPhantomCollision::update()
{
    vsComponent *objectComp;
    atMatrix globalXform;
    double distFromCollision;
    atVector collideNormal;
    atVector forceVector;
    atVector positionDelta;


    // If there aren't any key points defined, then there's nothing we can do
    if (offsetCount == 0)
        return;

    // Obtain the current local-to-global coordinate transform
    objectComp = kinematics->getComponent();
    globalXform = objectComp->getGlobalXform();

    distFromCollision = getCollisionData(globalXform, &collideNormal);

    // Insure it is normalized, or else the force magnitude will be off.
    collideNormal.normalize();

    // If there is any distance left to move, then we didn't get to
    // go as far as we wanted, indicating that a collision occurred.
    // Alter the current position and/or velocity as indicated.
    if (distFromCollision > 1E-6)
    {
        forceVector.set(0.0, 0.0, 0.0);

        if (distFromCollision < sphereRadius)
        {
/*
            // Original (linear) scaling formula.
            forceVector = collideNormal *
              ((distFromCollision / sphereRadius) * maximumForce);
*/

            // Elliptical function to have the magnitude curve some.
            forceVector = collideNormal *
              (maximumForce * (1 - sqrt(1 -
              (AT_SQR(distFromCollision)/AT_SQR(sphereRadius)))));
        }

        phantomSys->setForce(forceVector);
    }
    else
    {
        phantomSys->setForce(atVector(0.0, 0.0, 0.0));
    }
}

// ------------------------------------------------------------------------
// Private function
// ------------------------------------------------------------------------
double vsPhantomCollision::getCollisionData(atMatrix globalXform,
                                            atVector *hitNorm)
{
    int loop;
    bool firstIntersection;
    atVector centerPoints[VS_PHANTOM_COLLISION_POINTS_MAX];
    atVector hitPoints1[VS_PHANTOM_COLLISION_POINTS_MAX];
    atVector normals[VS_PHANTOM_COLLISION_POINTS_MAX];
    int valid1[VS_PHANTOM_COLLISION_POINTS_MAX];
    double resultDist, newDist;

    firstIntersection = true;
    
    // Clear the reported intersection normal
    hitNorm->set(0.0, 0.0, 0.0);

    // The first intersection test consists of rays fired in the direction
    // of movement from each key point
    for (loop = 0; loop < offsetCount; loop++)
    {
        // Compute the world location of the hot point by transforming
        // the point into global coordinates, and adding on any specified
        // offset translation.
        centerPoints[loop] = globalXform.getPointXform(offsetPoints[loop]);

        // Set the intersection segment for the point as starting at
        // the point's global position, and proceeding in the direction
        // of movement an arbitrarily long distance.
        intersect->setSphere(loop, centerPoints[loop], sphereRadius);
    }

    // Run the intersection traversal
    intersect->intersect(scene);

    // For each point, figure out if and where an intersection occurred
    for (loop = 0; loop < offsetCount; loop++)
    {
        if (intersect->getIntersection(loop)->isValid())
        {
            // Obtain the point and normal of intersection
            hitPoints1[loop] = intersect->getIntersection(loop)->getPoint();

            normals[loop] = intersect->getIntersection(loop)->getNormal();

            // Check to see if we hit the back side of a poly; if so, we
            // need to invert the normal
            if ((hitPoints1[loop] - centerPoints[loop]).getDotProduct(
                normals[loop]) > 0.0)
            {
                normals[loop].scale(-1.0);
            }

            valid1[loop] = 1;
        }
        else
        {
            // If there was no intersection, set the valid flag to false
            // and zero the normal direction
            normals[loop].clear();
            valid1[loop] = 0;
        }
    }
    
    // Initialize the result distance to zero, in case nothing overwrites it.
    resultDist = 0.0;

    for (loop = 0; loop < offsetCount; loop++)
    {
        // First intersection: straight distance
        if (valid1[loop])
        {
            // Figure out the distance from the hot point to the
            // intersection point, accounting for the margin distance
            newDist = sphereRadius -
              (centerPoints[loop] - hitPoints1[loop]).getMagnitude();

            // If it is the first intersection we check, use as current result.
            if (firstIntersection)
            {
                resultDist = newDist;
                (*hitNorm) = normals[loop];
                firstIntersection = false;
            }
            // Else check if this is the shortest result so far.
            else if (newDist < resultDist)
            {
                resultDist = newDist;
                (*hitNorm) = normals[loop];
            }
        }
    }
    
    // Cap the lower bound of the result to zero; we don't want to back up
    // if we're too close to an object.
    if (resultDist < 0.0)
    {
        return 0.0;
    }

    // Return the closest distance
    return resultDist;
}
