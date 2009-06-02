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
//    VESS Module:  vsTerrainFollow.c++
//
//    Description:  Motion model for forcing an object to stay in contact
//		    with the ground
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsTerrainFollow.h++"

// ------------------------------------------------------------------------
// Constructor - Stores the given pointers, creates an intersection object,
// and sets initial values with defaults.
// ------------------------------------------------------------------------
vsTerrainFollow::vsTerrainFollow(vsKinematics *objectKin, vsNode *theScene)
{
    // Store the kinematics and scene graph pointers
    kinematics = objectKin;
    scene = theScene;

    // Initialize the key point offset and step height to defaults
    pointOffset.set(0.0, 0.0, 0.0);
    stepHeight = VS_TFOLLOW_DEFAULT_HEIGHT;

    // Set up the intersection object
    intersect = new vsIntersect();
    intersect->setSegListSize(1);
    intersect->setMask(0xffffffff);
}

// ------------------------------------------------------------------------
// Destructor - Destroys the intersection object
// ------------------------------------------------------------------------
vsTerrainFollow::~vsTerrainFollow()
{
    delete intersect;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTerrainFollow::getClassName()
{
    return "vsTerrainFollow";
}

// ------------------------------------------------------------------------
// Sets the base offset vector. This value indicates the vector offset
// between the origin of the object to be terrain followed and the point
// on the object which should be touching the ground.
// ------------------------------------------------------------------------
void vsTerrainFollow::setBaseOffset(atVector newOffset)
{
    // Copy the offset value to our offset variable, forcing it to be
    // of size 3.
    pointOffset.clearCopy(newOffset);
    pointOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the base offset vector
// ------------------------------------------------------------------------
atVector vsTerrainFollow::getBaseOffset()
{
    return pointOffset;
}

// ------------------------------------------------------------------------
// Sets the maximum 'step up' height. This value indicates the height above
// the base point at which the intersection test begins. The terrain
// following algorithm will ignore objects that are above this height.
// ------------------------------------------------------------------------
void vsTerrainFollow::setStepHeight(double newHeight)
{
    stepHeight = newHeight;
}

// ------------------------------------------------------------------------
// Retrieves the maximum step height
// ------------------------------------------------------------------------
double vsTerrainFollow::getStepHeight()
{
    return stepHeight;
}

// ------------------------------------------------------------------------
// Sets the intersection mask for the terrain following intersection.
// During an intersection pass, this mask is bitwise AND'ed with the
// intersection value of each node; if the result of this AND is zero,
// the node and its children are ignored.
// ------------------------------------------------------------------------
void vsTerrainFollow::setIntersectMask(unsigned int newMask)
{
    // Pass the intersection mask to our vsIntersect object
    intersect->setMask(newMask);
}

// ------------------------------------------------------------------------
// Retrieves the intersection mask
// ------------------------------------------------------------------------
unsigned int vsTerrainFollow::getIntersectMask()
{
    // Retrieve the intersection mask from our vsIntersect object
    return (intersect->getMask());
}

// ------------------------------------------------------------------------
// Updates the motion model by calculating the distance between the scene's
// 'ground' and the bottom point of the geometry controlled by this motion
// model, as indicated by the base offset. The location of the model is
// adjusted as to appear to be in contact with the ground. Any vertical
// component of the velocity is removed.
// ------------------------------------------------------------------------
void vsTerrainFollow::update()
{
    atVector basePoint, topPoint;
    atVector downVec;
    atMatrix globalXform;
    vsComponent *objectComp;
    atVector hitPoint, groundOffset;
    atVector velocity;
    
    // Calculate the 'base' point; the point on the object (in global
    // coordinate space) which we want to be touching the ground at all times.
    objectComp = kinematics->getComponent();
    globalXform = objectComp->getGlobalXform();
    basePoint = globalXform.getPointXform(pointOffset);
    
    // Create a segment, starting at 'stepHeight' above the base point,
    // and going straight down a long way.
    topPoint = basePoint;
    topPoint[2] += stepHeight;
    downVec.set(0.0, 0.0, -1.0);
    intersect->setSeg(0, topPoint, downVec, 10000.0);
    
    // Run the intersection traversal
    intersect->intersect(scene);
    
    // If we intersected something, then modify the model's position such
    // that the given point is on the ground. (Actually, it's set to a
    // very tiny amount above it.)
    if (intersect->getIntersection(0)->isValid())
    {
        // Obtain the point of intersection
        hitPoint = intersect->getIntersection(0)->getPoint();

        // Modify the z-coordinate of the intersection point to force the
	// object to float very slightly off of the ground
        hitPoint[2] += VS_TFOLLOW_FLOAT_HEIGHT;

        // Determine the distance between the bottom of the object
	// and the ground
        groundOffset = hitPoint - basePoint;

        // Move the object so that the bottom of it is in contact with
	// the ground
        kinematics->modifyPosition(groundOffset);
    }
    
    // Remove the vertical component of the velocity
    velocity = kinematics->getVelocity();
    velocity[2] = 0.0;
    kinematics->setVelocity(velocity);
}
