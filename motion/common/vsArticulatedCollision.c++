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
    intersect->setSegListSize(16);
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
    vsVector collisionPoint;
    vsMatrix tempMat;
    int loop, sloop;
    vsVector startPt, endPt;
    int chainSize;
    vsKinematics *jointKin;
    double angle;
    vsVector upVec, rightVec;
    vsVector segOffsetVec;
    double isectDist, tempDist;
    vsVector isectPt, tempPt;
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

        // Figure out which directions are 'up' and 'right' in the coordinate
        // system of this joint, so that we can compute the segment offsets
        jointKin = invKinematics->getKinematicsObject(loop);
        tempMat = jointKin->getComponent()->getGlobalXform();
        upVec = tempMat.getVectorXform(vsVector(0.0, 0.0, 1.0));
        rightVec = tempMat.getVectorXform(vsVector(1.0, 0.0, 0.0));

        // Compute the sixteen segments that form the cylinder of intersection
        // around the joint segment
        for (sloop = 0; sloop < 16; sloop++)
        {
            // Compute the offset from the joint segment center to the
            // intersection segment
            angle = (double)sloop / 360.0;
            segOffsetVec = rightVec * cos(VS_DEG2RAD(angle));
            segOffsetVec += upVec * sin(VS_DEG2RAD(angle));
            segOffsetVec *= segmentRadius;

            // Set the segment in the intersection object
            intersect->setSeg(sloop, startPt + segOffsetVec,
                endPt + segOffsetVec);
        }

        // Run the intersection traversal, and check for any intersections
        intersect->intersect(scene);

        // Check each intersection segment for an intersection
        for (sloop = 0; sloop < 16; sloop++)
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
bool vsArticulatedCollision::processCollision(vsVector collisionPoint,
    int jointSegmentIdx, int isectSegmentIdx)
{
    // Call the interverse kinematics object to reposition the end effector of
    // the kinematics chain to the point of intersection
    invKinematics->reachForPoint(collisionPoint);

    // Return that the collision was processed successfully
    return true;
}
