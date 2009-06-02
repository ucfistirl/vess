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
//    VESS Module:  vsCollision.c++
//
//    Description:  Motion model that implements collision detection for
//                  any object. Works by taking a set of designated 'hot'
//                  points on an object and making sure that none of those
//                  points pass through a solid object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsCollision.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructor - Sets up the collsion's variables and allocates a 
// vsIntersect object
// ------------------------------------------------------------------------
vsCollision::vsCollision(vsKinematics *objectKin, vsNode *theScene)
{
    // Store the given kinematics object and scene pointer
    kinematics = objectKin;
    scene = theScene;

    // Start with zero hot points
    offsetCount = 0;

    // Default collision behavior is to stop
    collisionMode = VS_COLLISION_MODE_STOP;

    // Create and initialize the intersection object
    intersect = new vsIntersect();
    intersect->setSegListSize(0);
    intersect->setMask(0xffffffff);
    
    // Set the default margin distance
    wallMargin = VS_COLLISION_DEFAULT_MARGIN;
}

// ------------------------------------------------------------------------
// Destructor - Deletes the internal vsIntersect object
// ------------------------------------------------------------------------
vsCollision::~vsCollision()
{
    delete intersect;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCollision::getClassName()
{
    return "vsCollision";
}

// ------------------------------------------------------------------------
// Sets the number of 'hot points' that this collision object uses
// ------------------------------------------------------------------------
void vsCollision::setPointCount(int numPoints)
{
    // Bounds checking
    if ((numPoints < 0) || (numPoints > VS_COLLISION_POINTS_MAX))
    {
        printf("vsCollision::setPointCount: Parameter out of bounds\n");
        return;
    }
    
    // Set the number of hot points, and resize the intersection object's
    // list of segments
    offsetCount = numPoints;
    intersect->setSegListSize(numPoints);
}

// ------------------------------------------------------------------------
// Gets the number of 'hot points' that this collision object uses
// ------------------------------------------------------------------------
int vsCollision::getPointCount()
{
    return offsetCount;
}

// ------------------------------------------------------------------------
// Sets the position of one of the hot points of the collision object, in
// the local coordinate system of the associated kinematics' component.
// ------------------------------------------------------------------------
void vsCollision::setPoint(int index, atVector newOffset)
{
    // Boudns checking
    if ((index < 0) || (index >= VS_COLLISION_POINTS_MAX))
    {
        printf("vsCollision::setPointCount: Index out of bounds\n");
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
atVector vsCollision::getPoint(int index)
{
    atVector zero(0.0, 0.0, 0.0);

    // Bounds checking
    if ((index < 0) || (index >= VS_COLLISION_POINTS_MAX))
    {
        printf("vsCollision::setPointCount: Index out of bounds\n");
        return zero;
    }
    
    // Return the desired point
    return (offsetPoints[index]);
}

// ------------------------------------------------------------------------
// Sets the collision mode for the object
// ------------------------------------------------------------------------
void vsCollision::setCollisionMode(int newMode)
{
    collisionMode = newMode;
}

// ------------------------------------------------------------------------
// Gets the collision mode for the object
// ------------------------------------------------------------------------
int vsCollision::getCollisionMode()
{
    return collisionMode;
}

// ------------------------------------------------------------------------
// Sets the intersection mask for the collision object. The intersection
// mask is passed directly to the collision's associated vsIntersect 
// object.
// ------------------------------------------------------------------------
void vsCollision::setIntersectMask(unsigned int newMask)
{
    // Pass the mask to the vsIntersect object
    intersect->setMask(newMask);
}

// ------------------------------------------------------------------------
// Gets the intersection mask for the collision object
// ------------------------------------------------------------------------
unsigned int vsCollision::getIntersectMask()
{
    // Get the mask from the vsIntersect object
    return (intersect->getMask());
}

// ------------------------------------------------------------------------
// Sets the collision margin for the object. The margin is the closest the
// geometry governed by the collision object can approach any obstacle.
// ------------------------------------------------------------------------
void vsCollision::setMargin(double newMargin)
{
    wallMargin = newMargin;
}

// ------------------------------------------------------------------------
// Sets the collision margin for the object
// ------------------------------------------------------------------------
double vsCollision::getMargin()
{
    return wallMargin;
}

// ------------------------------------------------------------------------
// Updates the collision object by determining the direction of travel of
// the geometry, performing an intersection test to see if that direction
// of travel is clear, and adjusting the direction and/or speed if
// necessary.
// ------------------------------------------------------------------------
void vsCollision::update()
{
    vsComponent *objectComp;
    atMatrix globalXform;
    atVector currentVelocity;
    atVector currentDirection;
    double currentSpeed;

    int passCount = 0;
    atVector positionDelta;
    double distLeft;
    double distMoved;
    atVector collideNorm;
    
    double dotProd;
    atVector tempVec;
    atVector temp2;

    // If there aren't any key points defined, then there's nothing we can do
    if (offsetCount == 0)
        return;

    // Get the current velocity from the kinematics object; if it's zero,
    // then there's no work to do.
    currentVelocity = kinematics->getVelocity();
    if (currentVelocity.getMagnitude() < 1E-6)
        return;

    // Obtain the current local-to-global coordinate transform
    objectComp = kinematics->getComponent();
    globalXform = objectComp->getGlobalXform();

    // Break the current velocity into direction and speed
    currentDirection = currentVelocity.getNormalized();
    currentSpeed = currentVelocity.getMagnitude();

    // Compute actual distance travelled by factoring in the amount
    // of time passed
    distLeft = (currentSpeed * (vsTimer::getSystemTimer()->getInterval()));
    positionDelta.set(0.0, 0.0, 0.0);

    // Attempt to perform the desired movement
    while ((distLeft > 1E-6) && (passCount < VS_COLLISION_MAX_PASSES))
    {
        // Determine how far we are permitted to move by checking
        // for obstacles in our path
        distMoved = calcMoveAllowed(globalXform, positionDelta,
            currentDirection, distLeft, &collideNorm);
        distLeft -= distMoved;
        
        // Move the allowed distance
        positionDelta += (currentDirection.getScaled(distMoved));

        // If there is any distance left to move, then we didn't get to
        // go as far as we wanted, indicating that a collision occurred.
        // Alter the current position and/or velocity as indicated.
        if (distLeft > 1E-6)
            switch (collisionMode)
            {
                case VS_COLLISION_MODE_STOP:
                    distLeft = 0.0;
                    currentSpeed = 0.0;
                    break;

                case VS_COLLISION_MODE_SLIDE:
                    // Get the portion of the direction vector that is
                    // parallel to the normal of the wall that we just hit
                    dotProd = currentDirection.getDotProduct(collideNorm);
                    tempVec = collideNorm.getScaled(dotProd);

                    // For the sliding motion, the portion of the movement
                    // vector that is parallel to the wall normal is removed
                    currentDirection -= tempVec;

                    // Scale down the speed based on how much magnitude the
                    // direction vector lost
                    currentSpeed *= currentDirection.getMagnitude();
                    distLeft *= currentDirection.getMagnitude();

                    // Clean up the direction vector
                    currentDirection.normalize();
                    break;

                case VS_COLLISION_MODE_BOUNCE:
                    // Get the portion of the direction vector that is
                    // parallel to the normal of the wall that we just hit
                    dotProd = currentDirection.getDotProduct(collideNorm);
                    tempVec = collideNorm.getScaled(dotProd);

                    // For the bounce motion, the portion of the movement
                    // vector that is parallel to the wall normal is negated
                    currentDirection -= (tempVec * 2.0);

                    // Clean up the direction vector
                    currentDirection.normalize();
                    break;
            }

        // Mark that we've completed (another) pass
        passCount++;
    }
    
    if (passCount < VS_COLLISION_MAX_PASSES)
    {
        // Set our kinematics object's velocity to the computed direction
        // and speed
        tempVec = currentDirection.getScaled(currentSpeed);
        kinematics->setVelocity(tempVec);

        // Set the new location so that we end up at the positionDelta
        // location _after_ the current velocity is applied. If there was
        // no collision this frame, then these two calls should cancel out.
        kinematics->modifyPosition(positionDelta);
        temp2 = tempVec.getScaled(-(vsTimer::getSystemTimer()->getInterval()));
        kinematics->modifyPosition(temp2);
    }
    else
    {
        // Too many passes; either we're in a narrow area and bouncing
        // back and forth between the walls, or there's some other
        // unanticipated problem. In either case, give up and stop
        // moving completely.
        tempVec.set(0.0, 0.0, 0.0);
        kinematics->setVelocity(tempVec);
    }
}

// ------------------------------------------------------------------------
// Private function
// Utility function - calculates the raw distance between two points
// ------------------------------------------------------------------------
double vsCollision::distance(atVector start, atVector end)
{
    return sqrt(AT_SQR(start[0] - end[0]) +
                AT_SQR(start[1] - end[1]) +
                AT_SQR(start[2] - end[2]));
}

// ------------------------------------------------------------------------
// Private function
// Utility function - 'Fixes' the normal of an intersection hit by forcing
// the direction of the normal to be opposite the direction of the
// intersection ray. Used to correct for intersections with the back faces
// of geometry.
// ------------------------------------------------------------------------
atVector vsCollision::fixNormal(atVector sourcePt, atVector isectPt,
    atVector isectNorm)
{
    // The intersection 'ray' is the vector from the start point of the
    // intersection segment to the point of intersection for that segment.
    // For the normal to be opposite in direction from that vector, its dot
    // product with the vector should be negative. If it's not, then negate
    // the normal.
    if (isectNorm.getDotProduct(isectPt - sourcePt) > 0.0)
        return isectNorm.getScaled(-1.0);
    else
        return isectNorm;
}

// ------------------------------------------------------------------------
// Private function
// Uses the internal intersection object to determine the amount of
// movement in the desired direction is possible, given the presence or
// absence of any obstacles within the scene.
// ------------------------------------------------------------------------
double vsCollision::calcMoveAllowed(atMatrix globalXform, atVector posOffset,
                                    atVector moveDir, double maxMove,
                                    atVector *hitNorm)
{
    int loop;
    atVector startPoints[VS_COLLISION_POINTS_MAX];
    atVector hitPoints1[VS_COLLISION_POINTS_MAX];
    atVector normals[VS_COLLISION_POINTS_MAX];
    atVector hitPoints2[VS_COLLISION_POINTS_MAX];
    int valid1[VS_COLLISION_POINTS_MAX];
    int valid2[VS_COLLISION_POINTS_MAX];
    double resultDist, newDist;
    atVector segmentDir;
    atVector secondNormal;
    double dotProd;
    
    // Clear the reported intersection normal
    hitNorm->set(0.0, 0.0, 0.0);

    // The first intersection test consists of rays fired in the direction
    // of movement from each key point
    for (loop = 0; loop < offsetCount; loop++)
    {
        // Compute the world location of the hot point by transforming
        // the point into global coordinates, and adding on any specified
        // offset translation.
        startPoints[loop] = globalXform.getPointXform(offsetPoints[loop]);
        startPoints[loop] += posOffset;

        // Set the intersection segment for the point as starting at
        // the point's global position, and proceeding in the direction
        // of movement an arbitrarily long distance.
        intersect->setSeg(loop, startPoints[loop], moveDir, 10000.0);
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

            // Check to see if we hit the back side of a poly; if so, we
            // need to invert the normal
            normals[loop] = fixNormal(startPoints[loop], hitPoints1[loop],
                intersect->getIntersection(loop)->getNormal());

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

    // The second intersection test consists of rays still fired from
    // the key points, but in the directions of the walls found by the
    // first intersection. This is needed because we often approach walls
    // from an angle; the shortest distance from the plane of the wall
    // to the moving object (calculated in the direction of the wall's
    // normal) is needed to compute how far we can go before we're too
    // close to the wall, but we also need to know if the wall itself
    // extends out to that point of shortest distance to know if we should
    // be concerned about it.
    for (loop = 0; loop < offsetCount; loop++)
    {
        // Compute the world location of the hot point by transforming
        // the point into global coordinates, and adding on any specified
        // offset translation.
        startPoints[loop] = globalXform.getPointXform(offsetPoints[loop]);
        startPoints[loop] += posOffset;

        // If the first intersection worked, use the calculated normal;
        // if it failed, then use the direction of motion instead.
        if (valid1[loop])
            intersect->setSeg(loop, startPoints[loop],
                normals[loop].getScaled(-1.0), 10000.0);
        else
            intersect->setSeg(loop, startPoints[loop], moveDir, 10000.0);
    }

    // Run the intersection traversal
    intersect->intersect(scene);

    // For each point, figure out if and where an intersection occurred
    for (loop = 0; loop < offsetCount; loop++)
    {
        if (intersect->getIntersection(loop)->isValid())
        {
            // If the intersection hit something, record the point
            hitPoints2[loop] = intersect->getIntersection(loop)->getPoint();
            valid2[loop] = 1;
        }
        else
            valid2[loop] = 0;
    }

    // Take all of the data from all of the intersections and figure out
    // which result gives us the shortest distance
    resultDist = maxMove;

    for (loop = 0; loop < offsetCount; loop++)
    {
        // First intersection: straight distance
        if (valid1[loop])
        {
            // Figure out the distance from the hot point to the
            // intersection point, accounting for the margin distance
            newDist = distance(startPoints[loop], hitPoints1[loop]);
            newDist -= wallMargin;

            // Check if this is the shortest result so far
            if (newDist < resultDist)
            {
                resultDist = newDist;
                (*hitNorm) = normals[loop];
            }
        }

        // Second intersection: adjust for angle between movement direction
        // and segment direction
        if (valid2[loop])
        {
            // Get the intersection normal for this second intersection. This
            // data should still be in the intersection object.
            secondNormal = fixNormal(startPoints[loop], hitPoints2[loop],
                intersect->getIntersection(loop)->getNormal());

            // Compute the dot product of the travel direction and
            // intersection normal. If this product is positive or zero, then
            // the object that we intersected with isn't actually in our way;
            // don't bother computing how far away it is.
            dotProd = moveDir.getDotProduct(secondNormal);

            if (dotProd < -AT_DEFAULT_TOLERANCE)
            {
                // Figure out the distance from the hot point to the
                // intersection point, accounting for the margin distance
                newDist = distance(startPoints[loop], hitPoints2[loop]);
                newDist -= wallMargin;

                // Scale the distance by the inverse of the dot product of the
                // normal and movement direction vectors. This dot product is
                // always negative and so is negated before it is used so that
                // we end up with a positive value. This scaling is done to
                // take into account the angle of approach of the obstacle; we
                // can go farther towards something if we're not going along
                // the shortest (perpendicular) distance towards it.
                newDist /= (-dotProd);

                // Use this new distance if it is shorter than anything else
                // so far
                if (newDist < resultDist)
                {
                    // Store the shorter distance, and copy the intersection
                    // normal into the return data pointer location
                    resultDist = newDist;
                    (*hitNorm) = secondNormal;
                }
            }

        }
    }

    // Cap the lower bound of the result to zero; we don't want to back up
    // if we're too close to an object.
    if (resultDist < 0.0)
        return 0.0;

    // Return the closest distance
    return resultDist;
}
