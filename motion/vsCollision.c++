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
//		    any object. Works by taking a set of designated 'hot'
//		    points on an object and making sure that none of those
//		    points pass through a solid object.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsCollision.h++"

vsCollision::vsCollision(vsKinematics *objectKin, vsNode *theScene)
{
    kinematics = objectKin;
    scene = theScene;

    offsetCount = 0;
    collisionMode = VS_COLLISION_MODE_STOP;

    intersect = new vsIntersect();
    intersect->setSegListSize(0);
    intersect->setMask(0xffffffff);
    
    wallMargin = VS_COLLISION_DEFAULT_MARGIN;
}

vsCollision::~vsCollision()
{
    delete intersect;
}

void vsCollision::setPointCount(int numPoints)
{
    if ((numPoints < 0) || (numPoints > VS_COLLISION_POINTS_MAX))
    {
        printf("vsCollision::setPointCount: Parameter out of bounds\n");
        return;
    }
    
    offsetCount = numPoints;
    intersect->setSegListSize(numPoints);
}

int vsCollision::getPointCount()
{
    return offsetCount;
}

void vsCollision::setPoint(int index, vsVector newOffset)
{
    if ((index < 0) || (index >= VS_COLLISION_POINTS_MAX))
    {
        printf("vsCollision::setPointCount: Index out of bounds\n");
        return;
    }
    
    (offsetPoints[index]).clearCopy(newOffset);
    (offsetPoints[index]).setSize(3);
}

vsVector vsCollision::getPoint(int index)
{
    vsVector zero(0.0, 0.0, 0.0);

    if ((index < 0) || (index >= VS_COLLISION_POINTS_MAX))
    {
        printf("vsCollision::setPointCount: Index out of bounds\n");
        return zero;
    }
    
    return (offsetPoints[index]);
}

void vsCollision::setCollisionMode(int newMode)
{
    collisionMode = newMode;
}

int vsCollision::getCollisionMode()
{
    return collisionMode;
}

void vsCollision::setIntersectMask(unsigned int newMask)
{
    intersect->setMask(newMask);
}

unsigned int vsCollision::getIntersectMask()
{
    return (intersect->getMask());
}

void vsCollision::setMargin(double newMargin)
{
    wallMargin = newMargin;
}

double vsCollision::getMargin()
{
    return wallMargin;
}

void vsCollision::update()
{
    vsComponent *objectComp;
    vsMatrix globalXform;
    vsVector currentVelocity;
    vsVector currentDirection;
    double currentSpeed;

    int passCount = 0;
    vsVector positionDelta;
    double distLeft;
    double distMoved;
    vsVector collideNorm;
    
    double dotProd;
    vsVector tempVec;
    vsVector temp2;

    // If there aren't any key points defined, then there's nothing we can do
    if (offsetCount == 0)
        return;

    // Get the current velocity from the kinematics object; if it's zero,
    // then there's no work to do.
    currentVelocity = kinematics->getVelocity();
    if (currentVelocity.getMagnitude() < 1E-6)
        return;

    // Calculate the starting point for each segment by taking the
    // point in local coordinates and transforming it by the current
    // local-to-global rotation
    objectComp = kinematics->getComponent();
    globalXform = objectComp->getGlobalXform();

    currentDirection = currentVelocity.getNormalized();
    currentSpeed = currentVelocity.getMagnitude();

    distLeft = (currentSpeed * (vsSystem::systemObject->getFrameTime()));
    positionDelta.set(0.0, 0.0, 0.0);

    while ((distLeft > 1E-6) && (passCount < VS_COLLISION_MAX_PASSES))
    {
        distMoved = calcMoveAllowed(globalXform, positionDelta,
            currentDirection, distLeft, &collideNorm);
        distLeft -= distMoved;
        
        // Move the allowed distance
        positionDelta += (currentDirection.getScaled(distMoved));

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

        passCount++;
    }
    
    if (passCount < VS_COLLISION_MAX_PASSES)
    {
        tempVec = currentDirection.getScaled(currentSpeed);
        kinematics->setVelocity(tempVec);
        // Set the new location so that we end up at the positionDelta
        // location _after_ the current velocity is applied. If there was
        // no collision this frame, then these two calls should cancel out.
        kinematics->modifyPosition(positionDelta);
        temp2 = tempVec.getScaled(-(vsSystem::systemObject->getFrameTime()));
        kinematics->modifyPosition(temp2);
    }
    else
    {
        tempVec.set(0.0, 0.0, 0.0);
        kinematics->setVelocity(tempVec);
    }
}

double vsCollision::distance(vsVector start, vsVector end)
{
    return sqrt(VS_SQR(start[0] - end[0]) +
                VS_SQR(start[1] - end[1]) +
                VS_SQR(start[2] - end[2]));
}

double vsCollision::calcMoveAllowed(vsMatrix globalXform, vsVector posOffset,
                                    vsVector moveDir, double maxMove,
                                    vsVector *hitNorm)
{
    int loop;
    vsVector startPoints[VS_COLLISION_POINTS_MAX];
    vsVector hitPoints1[VS_COLLISION_POINTS_MAX];
    vsVector normals[VS_COLLISION_POINTS_MAX];
    vsVector hitPoints2[VS_COLLISION_POINTS_MAX];
    int valid1[VS_COLLISION_POINTS_MAX];
    int valid2[VS_COLLISION_POINTS_MAX];
    double resultDist, newDist;

    // The first intersection test consists of rays fired in the direction
    // of movement from each key point
    for (loop = 0; loop < offsetCount; loop++)
    {
        startPoints[loop] = globalXform.getPointXform(offsetPoints[loop]);
        startPoints[loop] += posOffset;
        intersect->setSeg(loop, startPoints[loop], moveDir, 10000.0);
    }

    intersect->intersect(scene);

    for (loop = 0; loop < offsetCount; loop++)
    {
        if (intersect->getIsectValid(loop))
        {
            hitPoints1[loop] = intersect->getIsectPoint(loop);
            normals[loop] = intersect->getIsectNorm(loop);
            // Check to see if we hit the back side of a poly; if so, we
            // need to invert the normal
            if (moveDir.getDotProduct(normals[loop]) > 0.0)
                normals[loop].scale(-1.0);
            valid1[loop] = 1;
        }
        else
        {
            normals[loop].clear();
            valid1[loop] = 0;
        }
    }
    
    // The second intersection test consists of rays still fired from
    // the key points, but in the directions of the walls found by the
    // first intersection.
    for (loop = 0; loop < offsetCount; loop++)
    {
        startPoints[loop] = globalXform.getPointXform(offsetPoints[loop]);
        startPoints[loop] += posOffset;
        if (valid2[loop])
            intersect->setSeg(loop, startPoints[loop],
                normals[loop].getScaled(-1.0), 10000.0);
        else
            intersect->setSeg(loop, startPoints[loop], moveDir, 10000.0);
    }
    
    intersect->intersect(scene);

    for (loop = 0; loop < offsetCount; loop++)
    {
        if (intersect->getIsectValid(loop))
        {
            hitPoints2[loop] = intersect->getIsectPoint(loop);
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
            newDist = distance(startPoints[loop], hitPoints1[loop]);
            newDist -= wallMargin;
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
            newDist = distance(startPoints[loop], hitPoints2[loop]);
            newDist -= wallMargin;
            // Scale the distance by the inverse of the dot product of the
            // normal and movement direction vectors. This dot product is
            // always negative and so is negated before it is used so that we
            // end up with a positive value.
            newDist /= (-(moveDir.getDotProduct(normals[loop])));
            if (newDist < resultDist)
            {
                resultDist = newDist;
                (*hitNorm) = intersect->getIsectNorm(loop);
            }
        }
    }
    
    return resultDist;
}

