#include "vsTrackedMotion.h++"

// ------------------------------------------------------------------------
// Creates a vsTrackedMotion motion model using the given component and
// tracker
// ------------------------------------------------------------------------
vsTrackedMotion::vsTrackedMotion(vsMotionTracker *theTracker,
    vsKinematics *kinObject)
{
    tracker = theTracker;   
    kinematics = kinObject;

    positionEnabled = VS_TRUE;
    orientationEnabled = VS_TRUE;
    
    positionOffset.set(0.0, 0.0, 0.0);
    orientationOffset.set(0.0, 0.0, 0.0, 1.0);

    resetPosition.set(0.0, 0.0, 0.0);
    resetOrientation.set(0.0, 0.0, 0.0, 1.0);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTrackedMotion::~vsTrackedMotion()
{
}

// ------------------------------------------------------------------------
// Enables positional motion
// ------------------------------------------------------------------------
void vsTrackedMotion::enablePositionTracking()
{
    positionEnabled = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables positional motion
// ------------------------------------------------------------------------
void vsTrackedMotion::disablePositionTracking()
{
    positionEnabled = VS_FALSE;
}

// ------------------------------------------------------------------------
// Enables rotational motion
// ------------------------------------------------------------------------
void vsTrackedMotion::enableOrientationTracking()
{
    orientationEnabled = VS_TRUE;
}

// ------------------------------------------------------------------------
// Disables rotational motion
// ------------------------------------------------------------------------
void vsTrackedMotion::disableOrientationTracking()
{
    orientationEnabled = VS_FALSE;
}

// ------------------------------------------------------------------------
// Sets the position offset
// ------------------------------------------------------------------------
void vsTrackedMotion::setPositionOffset(vsVector newOffset)
{
    positionOffset.clearCopy(newOffset);
    positionOffset.setSize(3);
}

// ------------------------------------------------------------------------
// Gets the position offset
// ------------------------------------------------------------------------
vsVector vsTrackedMotion::getPositionOffset()
{
    return positionOffset;
}

// ------------------------------------------------------------------------
// Sets the orientation post-offset
// ------------------------------------------------------------------------
void vsTrackedMotion::setOrientationOffset(vsQuat newOffset)
{
    orientationOffset = newOffset;
}

// ------------------------------------------------------------------------
// Gets the orientation post-offset
// ------------------------------------------------------------------------
vsQuat vsTrackedMotion::getOrientationOffset()
{
    return orientationOffset;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsTrackedMotion::update()
{
    vsVector trackerPos;
    vsQuat trackerOrn;

    // Get tracker data
    trackerPos = tracker->getPositionVec();  
    trackerOrn = tracker->getOrientationQuat();
    
    // Factor in reset position and offsets
    trackerPos += resetPosition;
    trackerOrn = resetOrientation * trackerOrn;
    
    // Factor in specified offsets
    trackerPos += positionOffset;
    trackerOrn = orientationOffset * trackerOrn;
    
    // Apply the data to the kinematics object
    kinematics->setPosition(trackerPos);
    kinematics->setOrientation(trackerOrn);
}

// ------------------------------------------------------------------------
// Sets the reset position and orientation of the motion model to the
// current position and orientation
// ------------------------------------------------------------------------
void vsTrackedMotion::reset()
{
    vsVector trackerPos;
    vsQuat trackerOrn;

    // Get tracker data
    trackerPos = tracker->getPositionVec();  
    trackerOrn = tracker->getOrientationQuat();

    // Set the reset data
    resetPosition = trackerPos * -1.0;
    resetOrientation = trackerOrn.getConjugate();
}
