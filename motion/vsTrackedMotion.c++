#include "vsTrackedMotion.h++"

// ------------------------------------------------------------------------
// Creates a vsTrackedMotion motion model using the given component and
// tracker
// ------------------------------------------------------------------------
vsTrackedMotion::vsTrackedMotion(vsMotionTracker *theTracker)
               : vsMotionModel()
{
     tracker = theTracker;   

     positionEnabled = VS_TRUE;
     orientationEnabled = VS_TRUE;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsTrackedMotion::~vsTrackedMotion()
{
}

// ------------------------------------------------------------------------
// Enables/Disables positional motion
// ------------------------------------------------------------------------
void vsTrackedMotion::enablePosition(int enabled)
{
    positionEnabled = enabled;
}

// ------------------------------------------------------------------------
// Enables/Disables rotational motion
// ------------------------------------------------------------------------
void vsTrackedMotion::enableOrientation(int enabled)
{
    orientationEnabled = enabled;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
vsVecQuat vsTrackedMotion::update()
{
    vsMatrix  currentTransform;
    vsVector  trackerPos;
    vsQuat    trackerOrn;
    vsQuat    temp;
    vsVector  dPos;
    vsQuat    dOrn;
    vsMatrix  newPosition;
    vsMatrix  newRotation;
    vsVector  origin;
    vsVector  currentPos;
    vsVecQuat motion;

    // Get tracker data
    trackerPos = tracker->getPositionVec();  
    trackerOrn = tracker->getOrientationQuat();

    // Figure differences
    dPos = trackerPos - lastTrackerPos;

    temp = lastTrackerOrn;
    lastTrackerOrn.conjugate();
    dOrn = trackerOrn * temp;

    if (orientationEnabled)
    {
        motion.quat = dOrn;
    }
    else
    {
        motion.quat.clear();
    }

    if (positionEnabled)
    {
        motion.vector = dPos;
    }
    else
    {
        motion.vector.setSize(3);
        motion.vector.clear();
    }

    lastTrackerPos = trackerPos;
    lastTrackerOrn = trackerOrn;

    return motion;
}
