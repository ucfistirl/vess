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
//    VESS Module:  vsArticulatedCollision.c++
//
//    Description:  Class for performing collision detection and handling
//                  on an articulated object
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsArticulatedCollision.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsArticulatedCollision::vsArticulatedCollision(vsInverseKinematics *invkin,
    vsNode *theScene)
{
    // Store and reference the inverse kinematics object
    invKinematics = invkin;
    invKinematics->ref();

    // Store and reference the scene to intersect against
    scene = theScene;
    scene->ref();

    // Create a vsIntersect object and configure it
    intersect = new vsIntersect();
    intersect->ref();
    intersect->setSegListSize(VS_ARTCOL_SEGMENT_COUNT);
    intersect->disablePaths();

    // Set default parameters
    segmentRadius = 1.0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsArticulatedCollision::~vsArticulatedCollision()
{
    // Release the objects associated with this one
    vsObject::unrefDelete(invKinematics);
    vsObject::unrefDelete(scene);
    vsObject::unrefDelete(intersect);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsArticulatedCollision::getClassName()
{
    return "vsArticulatedCollision";
}

// ------------------------------------------------------------------------
// Performs the intersection detection and handling routine
// ------------------------------------------------------------------------
void vsArticulatedCollision::update()
{
    bool hitSomething;
    atVector collisionPoint;
    atMatrix tempMat;
    int loop, sloop;
    atVector startPt, endPt;
    atVector segmentVec;
    double dotProd;
    int chainSize;
    vsKinematics *jointKin;
    double angle;
    atVector upVec, rightVec;
    atVector segOffsetVec;
    double isectDist, tempDist;
    atVector isectPt, tempPt;
    int isectIdx;
    bool done;

    // For each segment of the articulated object, determine if that segment
    // intersects anything in the associated scene.
    hitSomething = false;
    chainSize = invKinematics->getKinematicsChainSize();

    // Start with the first segment and work our way outward
    for (loop = 0; loop < chainSize; loop++)
    {
        // Compute the starting location of the segment
        jointKin = invKinematics->getKinematicsObject(loop);
        tempMat = jointKin->getComponent()->getParent(0)->getGlobalXform();
        startPt = tempMat.getPointXform(jointKin->getCenterOfMass());

        // Compute the ending location of the segment
        if (loop == (chainSize - 1))
        {
            // If this is the last joint, then the endpoint of this segment
            // will be determined by the offset from the last joint to the
            // kinematics chain's end effector
            jointKin = invKinematics->getKinematicsObject(loop);
            tempMat = jointKin->getComponent()->getGlobalXform();
            endPt = tempMat.getPointXform(invKinematics->getEndpointOffset() +
                jointKin->getCenterOfMass());
        }
        else
        {
            // If this isn't the last joint, then there should be a 'next'
            // joint to pull the ending location from
            jointKin = invKinematics->getKinematicsObject(loop + 1);
            tempMat = jointKin->getComponent()->getParent(0)->getGlobalXform();
            endPt = tempMat.getPointXform(jointKin->getCenterOfMass());
        }

        // Compute two vectors that are perpendicular to this joint segment

        // - Construct the vector that represents this segment's direction
        segmentVec = (endPt - startPt).getNormalized();

        // - Create a vector that isn't parallel to the segment vector
        upVec.set(0.0, 0.0, 1.0);
        dotProd = segmentVec.getDotProduct(upVec);
        if (AT_EQUAL(fabs(dotProd), 1.0))
        {
            upVec.set(0.0, 1.0, 0.0);
            dotProd = segmentVec.getDotProduct(upVec);
        }

        // - Project the non-parallel vector into the perpendicular plane
        upVec -= segmentVec.getScaled(dotProd);
        upVec.normalize();

        // - Compute a second perpendicular vector by taking the cross product
        // of the segment vector and the first perpendicular vector
        rightVec = segmentVec.getCrossProduct(upVec);

        // Compute the segments that form the cylinder of intersection around
        // the joint segment
        for (sloop = 0; sloop < VS_ARTCOL_SEGMENT_COUNT; sloop++)
        {
            // Compute the offset from the joint segment center to the
            // intersection segment
            angle = (double)sloop  * (360.0 / (double)(VS_ARTCOL_SEGMENT_COUNT));
            segOffsetVec = rightVec * cos(AT_DEG2RAD(angle));
            segOffsetVec += upVec * sin(AT_DEG2RAD(angle));
            segOffsetVec *= segmentRadius;

            // Set the segment in the intersection object
            intersect->setSeg(sloop, startPt + segOffsetVec,
                endPt + segOffsetVec);
        }

        // Run the intersection traversal, and check for any intersections
        intersect->intersect(scene);

        // Check each intersection segment for an intersection
        for (sloop = 0; sloop < VS_ARTCOL_SEGMENT_COUNT; sloop++)
        {
            // If there's an intersection, figure out how far away it is from
            // the beginning of the joint segment
            if (intersect->getIsectValid(sloop))
            {
                tempPt = intersect->getIsectPoint(sloop);
                tempDist = (tempPt - startPt).getMagnitude();

                // Is this our first such intersection?
                if (hitSomething)
                {
                    // This isn't our first intersection; only record the point
                    // and the distance if this intersection is closer than any
                    // previous one
                    if (tempDist < isectDist)
                    {
                        isectDist = tempDist;
                        isectPt = tempPt;
                        isectIdx = sloop;
                    }
                }
                else
                {
                    // This is our first intersection, record the point of
                    // intersection and the distance
                    isectDist = tempDist;
                    isectPt = tempPt;
                    isectIdx = sloop;
                    hitSomething = true;
                }
            }
        }

        // If there were any intersections, then we've collided with something
        if (hitSomething)
        {
            // Handle the collision by calling the collision handler function
            done = processCollision(isectPt, loop, isectIdx);

            // If the collision was processed successfully, then we're done
            if (done)
                return;
        }
    }
}

// ------------------------------------------------------------------------
// Sets the radius of the intersection cylinders for the kinematics chain
// segments
// ------------------------------------------------------------------------
void vsArticulatedCollision::setSegmentRadius(double radius)
{
    segmentRadius = radius;
}

// ------------------------------------------------------------------------
// Gets the radius of the intersection cylinders for the kinematics chain
// segments
// ------------------------------------------------------------------------
double vsArticulatedCollision::getSegmentRadius()
{
    return segmentRadius;
}

// ------------------------------------------------------------------------
// Gets the inverse kinematics object associated with this object
// ------------------------------------------------------------------------
vsInverseKinematics *vsArticulatedCollision::getInverseKinematics()
{
    return invKinematics;
}

// ------------------------------------------------------------------------
// Gets the intersection object associated with this object
// ------------------------------------------------------------------------
vsIntersect *vsArticulatedCollision::getIntersectionObject()
{
    return intersect;
}

// ------------------------------------------------------------------------
// Protected virtual function
// Process a collision between the articulated object and the surrounding
// environment
// ------------------------------------------------------------------------
bool vsArticulatedCollision::processCollision(atVector collisionPoint,
    int jointSegmentIdx, int isectSegmentIdx)
{
    // Call the interverse kinematics object to reposition the end effector of
    // the kinematics chain to the point of intersection
    invKinematics->reachForPoint(collisionPoint);

    // Return that the collision was processed successfully
    return true;
}
