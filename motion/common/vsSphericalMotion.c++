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
//    VESS Module:  vsSphericalMotion.c++
//
//    Description:  Motion model that provides user-controlled spherical
//                  motion with respect to a point or another component.
//                  That is, the controlled component will orbit the
//                  target point/component on a circumscribed sphere.
//                  The radius of the sphere (how close the controlled
//                  component orbits w.r.t. the target point/component) is
//                  user-controlled as well.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsSphericalMotion.h++"
#include <stdio.h>
#include "vsMatrix.h++"
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Constructs a trackball motion model using a mouse and the default button
// configuration
// ------------------------------------------------------------------------
vsSphericalMotion::vsSphericalMotion(vsMouse *mouse, vsKinematics *kin)
                 : vsMotionModel()
{
     // Get the input axes
     horizontal = mouse->getAxis(0);
     vertical = mouse->getAxis(1);

     // Complain if any of the axes are not normalized
     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsSphericalMotion::vsSphericalMotion:  One or more axes "
             "are not normalized!\n");
     }

     // Save the kinematics
     kinematics = kin;

     // Initialize variables to defaults
     orbitButton = mouse->getButton(0);
     zoomButton = mouse->getButton(2);

     // Initialize the target point
     targetPoint.setSize(3);
     targetPoint.clear();
     targetComp = NULL;
     targetMode = VS_SPHM_TARGET_POINT;

     // Set motion defaults
     orbitConst = VS_SPHM_DEFAULT_ORBIT_CONST;
     zoomConst = VS_SPHM_DEFAULT_ZOOM_CONST;
     minRadius = VS_SPHM_DEFAULT_MIN_RADIUS;
}

// ------------------------------------------------------------------------
// Constructs a trackball motion model using a mouse and the given button
// configuration
// ------------------------------------------------------------------------
vsSphericalMotion::vsSphericalMotion(vsMouse *mouse, int orbitButtonIndex, 
                                     int zoomButtonIndex, vsKinematics *kin)
                 : vsMotionModel()
{
     // Get the input axes
     horizontal = mouse->getAxis(0);
     vertical = mouse->getAxis(1);

     // Complain if any of the axes are not normalized
     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsSphericalMotion::vsSphericalMotion:  One or more axes "
             "are not normalized!\n");
     }

     // Save the parameters
     orbitButton = mouse->getButton(orbitButtonIndex);
     zoomButton = mouse->getButton(zoomButtonIndex);
     kinematics = kin;

     // Set the motion defaults
     orbitConst = VS_SPHM_DEFAULT_ORBIT_CONST;
     zoomConst = VS_SPHM_DEFAULT_ZOOM_CONST;
     minRadius = VS_SPHM_DEFAULT_MIN_RADIUS;

     // Initialize the target point
     targetPoint.setSize(3);
     targetPoint.clear();
     targetComp = NULL;
     targetMode = VS_SPHM_TARGET_POINT;
}

// ------------------------------------------------------------------------
// Constructs a trackball motion model using the given axis and button 
// objects
// ------------------------------------------------------------------------
vsSphericalMotion::vsSphericalMotion(vsInputAxis *horizAxis, 
                                     vsInputAxis *vertAxis,
                                     vsInputButton *orbitBtn,
                                     vsInputButton *zoomBtn,
                                     vsKinematics *kin)
                 : vsMotionModel()
{
     // Get the axes
     horizontal = horizAxis;
     vertical = vertAxis;

     // Complain if any of the axes are not normalized
     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsSphericalMotion::vsSphericalMotion:  One or more axes "
             "are not normalized!\n");
     }

     // Save the parameters
     orbitButton = orbitBtn;
     zoomButton = zoomBtn;
     kinematics = kin;

     // Set the motion defaults
     orbitConst = VS_SPHM_DEFAULT_ORBIT_CONST;
     zoomConst = VS_SPHM_DEFAULT_ZOOM_CONST;
     minRadius = VS_SPHM_DEFAULT_MIN_RADIUS;

     // Initialize the target point
     targetPoint.setSize(3);
     targetPoint.clear();
     targetComp = NULL;
     targetMode = VS_SPHM_TARGET_POINT;
}

// ------------------------------------------------------------------------
// Destructor for vsSphericalMotion
// ------------------------------------------------------------------------
vsSphericalMotion::~vsSphericalMotion()
{

}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSphericalMotion::getClassName()
{
    return "vsSphericalMotion";
}

// ------------------------------------------------------------------------
// Sets the target of the spherical motion to be the given point
// ------------------------------------------------------------------------
void vsSphericalMotion::setTargetPoint(vsVector targetPt)
{
    // Set the target point to the point passed in
    targetPoint = targetPt;

    // Set the target mode to point (as opposed to component mode)
    targetMode = VS_SPHM_TARGET_POINT;

    // Set the target component to NULL (we're using point mode now)
    targetComp = NULL;
}

// ------------------------------------------------------------------------
// Retrieves the target point of the spherical motion.  Returns a zero
// vector if not currently in VS_SPHM_TARGET_POINT targeting mode.
// ------------------------------------------------------------------------
vsVector vsSphericalMotion::getTargetPoint()
{
    // Check the target mode to see if we're in point mode
    if (targetMode == VS_SPHM_TARGET_POINT)
    {
        // We're in point mode, return the current target point
        return targetPoint;
    }
    else
    {
        // We're in component mode, return a zero vector for the target
        // point
        return vsVector(0.0, 0.0, 0.0);
    }
}

// ------------------------------------------------------------------------
// Sets the target of the spherical motion to be the given component
// ------------------------------------------------------------------------
void vsSphericalMotion::setTargetComponent(vsComponent *targetCmp)
{
    // Make sure the component exists
    if (targetComp != NULL)
    {
        // Set the target component and mode
        targetComp = targetCmp;
        targetMode = VS_SPHM_TARGET_COMPONENT;

        // Set the target point to a zero vector
        targetPoint.clear();
    }
    else
    {
        printf("vsSphericalMotion::setTargetComponent:  "
            "New target is NULL,  keeping old target.\n");
    }
}

// ------------------------------------------------------------------------
// Retrieves the target component of the spherical motion.  Returns NULL
// if not currently in VS_SPHM_TARGET_COMPONENT targeting mode.
// ------------------------------------------------------------------------
vsComponent *vsSphericalMotion::getTargetComponent()
{
    // Make sure were in component targeting mode
    if (targetMode == VS_SPHM_TARGET_COMPONENT)
    {
        // Return the target component
        return targetComp;
    }
    else
    {
        // Not in component mode, return NULL for the target
        return NULL;
    }
}

// ------------------------------------------------------------------------
// Returns the current target mode
// ------------------------------------------------------------------------
vsSphericalMotionTargetMode vsSphericalMotion::getTargetMode()
{
    return targetMode;
}


// ------------------------------------------------------------------------
// Sets the orbit constant.  The orbit constant specifies how many degrees
// the component moves along the orbit sphere per unit of input.  
// ------------------------------------------------------------------------
void vsSphericalMotion::setOrbitConstant(double newConst)
{
    orbitConst = newConst;
}

// ------------------------------------------------------------------------
// Returns the translation constant.
// ------------------------------------------------------------------------
double vsSphericalMotion::getOrbitConstant()
{
    return orbitConst;
}

// ------------------------------------------------------------------------
// Sets the zoom constant.  The zoom constant specifies how much the
// orbit sphere's radius changes per unit of input.  
// ------------------------------------------------------------------------
void vsSphericalMotion::setZoomConstant(double newConst)
{
    zoomConst = newConst;
}

// ------------------------------------------------------------------------
// Returns the zoom constant.
// ------------------------------------------------------------------------
double vsSphericalMotion::getZoomConstant()
{
    return zoomConst;
}

// ------------------------------------------------------------------------
// Set the minimum orbit radius.
// ------------------------------------------------------------------------
void vsSphericalMotion::setMinimumRadius(double newMin)
{
    minRadius = newMin;
}

// ------------------------------------------------------------------------
// Returns the minimum orbit radius.
// ------------------------------------------------------------------------
double vsSphericalMotion::getMinimumRadius()
{
    return minRadius;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsSphericalMotion::update()
{
    double    interval;
    vsVector  zero;
    vsVector  targetPos;
    vsMatrix  targetXform;
    vsVector  targetVec;
    vsVector  currentPos;
    vsVector  projectedVec;
    double    radius;
    double    azimuth, elevation;
    vsQuat    azimuthQuat, elevationQuat;
    vsQuat    rotationQuat;
    vsVector  tempVec;
    vsVector  newPos;
    double    dHoriz, dVert;
    vsVector  dPos;
    double    dRadius;

    // Get the interval of elapsed time
    interval = vsSystem::systemObject->getFrameTime();

    // Make sure interval is valid
    if (interval <= 0.0)
        return;

    // Initialize the horizontal and vertical delta variables
    dHoriz = 0.0;
    dVert = 0.0;

    // Check the horizontal axis (skip horizontal if the axis doesn't exist)
    if (horizontal != NULL)
    {
        // Subtract the current horizontal axis position from the previous
        // one to get the amount of axis movement
        dHoriz = horizontal->getPosition() - lastHorizontal;

        // Save the current axis position for next frame
        lastHorizontal = horizontal->getPosition();
    }

    // Check the vertical axis (skip vertical if the axis doesn't exist)
    if (vertical != NULL)
    {
        // Subtract the current vertical axis position from the previous
        // one to get the amount of axis movement
        dVert = vertical->getPosition() - lastVertical;

        // Save the current axis position for next frame
        lastVertical = vertical->getPosition();
    }

    // Get the position of the target.  The procedure to do this depends
    // on the current targeting mode
    if (targetMode == VS_SPHM_TARGET_POINT)
    {
        // Point mode, simply return the target point
        targetPos = targetPoint;
    }
    else
    {
        // Component mode.  Start with a target point at the origin
        targetPos.set(0.0, 0.0, 0.0);

        // Get the global transform of the target component
        targetXform = targetComp->getGlobalXform();

        // Transform the target point by the global transform to obtain
        // the target position in world space
        targetPos = targetXform.getPointXform(targetPos);
    }

    // Get a vector from the target to the viewpoint
    targetVec = kinematics->getPosition() - targetPos;

    // Get the current radius of the orbit sphere (the distance from the
    // current position to the target
    radius = targetVec.getMagnitude();

    // If the radius is very small, adjust the target vector and radius
    // to the minimum radius based on the current orientation
    if (fabs(radius) < 1.0E-6)
    {
        // Create the new target vector (vector from the target to the 
        // viewpoint) and scale it to the minimum radius
        targetVec.set(0.0, -1.0, 0.0);
        targetVec.scale(minRadius);

        // Rotate the vector by the current orientation
        rotationQuat = kinematics->getOrientation();
        targetVec = rotationQuat.rotatePoint(targetVec);
        radius = minRadius;
    }

    // Determine the azimuth and elevation of the viewpoint on the
    // sphere
    if ((targetVec.isEqual(vsVector(0, 0, 1))) || 
        (targetVec.isEqual(vsVector(0, 0, -1))))
    {
        // If the vector is straight up or down, set the azimuth to 0
        // and the elevation accordingly
        azimuth = 0.0;
        elevation = 90.0 * targetVec[VS_Z];
    }
    else
    {
        // Project the target vector on the XY plane
        projectedVec.clearCopy(targetVec);
        projectedVec.setSize(2);
      
        // Normalize the projected vector
        projectedVec.normalize();

        // Calculate the azimuth from the projected vector
        azimuth = VS_RAD2DEG(atan2(projectedVec[VS_Y], projectedVec[VS_X]));
        
        // Correct for VESS coordinates and make sure 0 <= azimuth < 360
        azimuth -= 90.0;
        if (azimuth < 0.0)
            azimuth += 360.0;

        // Rotate the target vector so that it lines up with the Y axis
        azimuthQuat.setAxisAngleRotation(0, 0, 1, -azimuth);
        tempVec = azimuthQuat.rotatePoint(targetVec);

        // Calculate the angle between the Y axis and the target vector,
        // which is the elevation.
        elevation = tempVec.getAngleBetween(vsVector(0, 1, 0));
        
        // Find the sign of the elevation
        if (tempVec[VS_Z] < 0.0)
            elevation = -elevation;
    }

    // If any button is pressed, set all velocities to zero.  This is done
    // in case the kinematics object has inertia enabled.  Otherwise, it
    // would be difficult to control the motion.
    if (((orbitButton != NULL) && (orbitButton->isPressed())) ||
        ((zoomButton != NULL) && (zoomButton->isPressed()))) 
    {
        zero.set(0.0, 0.0, 0.0);
        kinematics->setVelocity(zero);
        kinematics->setAngularVelocity(zero, 0.0);
    }

    // Initialize the new position
    newPos = kinematics->getPosition();

    // Next, calculate the amount of motion based on which button(s) 
    // is/are pressed
    if ((orbitButton != NULL) && (orbitButton->isPressed()))
    {
        // Orbit the target by changing the azimuth and elevation
        // according to the axis movements
        azimuth   += dHoriz * orbitConst;
        elevation += dVert *  orbitConst;

        // Clamp elevation to -89 - 89 degrees to avoid angle ambiguities
        if (elevation > 89.0)
            elevation = 89.0;
        if (elevation < -89.0)
            elevation = -89.0;

        // Compute the new viewpoint given the radius and new
        // azimuth and elevation
        azimuthQuat.setAxisAngleRotation(0, 0, 1, azimuth);
        elevationQuat.setAxisAngleRotation(1, 0, 0, elevation);
        rotationQuat = azimuthQuat * elevationQuat;
        tempVec.set(0.0, radius, 0.0);
        newPos = rotationQuat.rotatePoint(tempVec);
        newPos += targetPos;

        // Compute the velocity vector of the change in position
        tempVec = newPos - kinematics->getPosition();
        tempVec.scale(1.0/interval);

        // Adjust the kinematics
        kinematics->setVelocity(tempVec);
    }
    else if ((zoomButton != NULL) && (zoomButton->isPressed()))
    {
        // Compute a zoom direction vector
        dPos = kinematics->getPosition() - targetPos;

        // If the current position is very close to the target point, the
        // zoom direction can be hard to determine, causing erratic behavior.
        // See if we need to handle this special case.
        if (dPos.getMagnitude() < 1.0E-6)
        {
            // Use the current orientation as the zoom direction
            dPos.set(0.0, -1.0, 0.0);
            rotationQuat = kinematics->getOrientation();
            dPos = rotationQuat.rotatePoint(dPos);
        }

        // Compute the change in radius
        dRadius = -dVert * zoomConst;
        if ((radius + dRadius) < minRadius)
        {
            dRadius = minRadius - radius;
        }

        // Scale the zoom amount by change in radius
        dPos.normalize();
        dPos.scale(dRadius);

        // Adjust the kinematics
        kinematics->setVelocity(dPos.getScaled(1.0/interval));
    }

    // Compute the orientation, get a direction vector from the new position
    // to the target point or component
    tempVec = targetPos - newPos;

    // Only change the orientation if the new direction vector has a 
    // significant magnitude
    if (tempVec.getMagnitude() > 1.0E-6)
    {
        // Compute the rotation quaternion from the direction vector
        rotationQuat.setVecsRotation(vsVector(0, 1, 0), vsVector(0, 0, 1),
            tempVec, vsVector(0, 0, 1));

        // Adjust the kinematics
        kinematics->setOrientation(rotationQuat);
    }
}
