#include "vsTrackedMotion.h++"

// ------------------------------------------------------------------------
// Creates a vsTrackedMotion motion model using the given component and
// tracker
// ------------------------------------------------------------------------
vsTrackedMotion::vsTrackedMotion(vsMotionTracker *theTracker)
               : vsMotionModel()
{
    tracker = theTracker;   

    if (tracker == NULL)
    {
        printf("vsTrackedMotion::vsTrackedMotion:  WARNING --"
            " NULL motion tracker!\n");
    }

    positionEnabled = VS_TRUE;
    orientationEnabled = VS_TRUE;

    lastTrackerPos.set(0.0, 0.0, 0.0);
    lastTrackerOrn.set(0.0, 0.0, 0.0, 1.0);
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
vsMatrix vsTrackedMotion::update()
{
    vsMatrix  currentTransform;
    vsVector  trackerPos;
    vsQuat    trackerOrn;
    vQuat    temp;
    vsVector  dPos;
    vsQuat    dOrn;
    vsMatrix  newTranslation;
    vsMatrix  newRotation;

    // Get tracker data
    trackerPos = tracker->getPositionVec();  
    trackerOrn = tracker->getOrientationQuat();

    // Figure differences
    dPos = trackerPos - lastTrackerPos;

    temp = lastTrackerOrn;
    temp.conjugate();
    dOrn = trackerOrn * temp;

    newRotation.setIdentity();
    newTranslation.setIdentity();
    if (orientationEnabled)
    {
        newRotation.setQuatRotation(dOrn);
    }

    if (positionEnabled)
    {
        newTranslation.setTranslation(dPos[VS_X], dPos[VS_Y], dPos[VS_Z]);
    }

    lastTrackerPos = trackerPos;
    lastTrackerOrn = trackerOrn;

    tempMat = newTranslation * newRotation;
    
    return newTranslation * newRotation;
}
