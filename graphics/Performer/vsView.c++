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
//    VESS Module:  vsView.c++
//
//    Description:  Class for storing and maintaining the viewpoint of a
//                  vsPane
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#include "vsView.h++"

// ------------------------------------------------------------------------
// Constructor - Initializes the position and orientation
// ------------------------------------------------------------------------
vsView::vsView()
{
    // Initialize the viewpoint's position and orientation
    viewLocation.setSize(3);
    viewLocation.clear();
    viewRotation.setIdentity();
    
    // Start with no vsViewpointAttribute attached
    viewAttribute = NULL;
    
    // Initialize the clipping planes to default values
    nearClip = 0.1;
    farClip = 10000.0;
    
    // Initialize the projection settings to default values
    projMode = VS_VIEW_PROJMODE_PERSP;
    projHval = -1.0;
    projVval = -1.0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsView::~vsView()
{
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsView::getClassName()
{
    return "vsView";
}

// ------------------------------------------------------------------------
// Sets the current viewpoint
// ------------------------------------------------------------------------
void vsView::setViewpoint(double xPosition, double yPosition, double zPosition)
{
    // Set the viewpoint's position to the given values
    viewLocation.set(xPosition, yPosition, zPosition);
}

// ------------------------------------------------------------------------
// Sets the current viewpoint
// ------------------------------------------------------------------------
void vsView::setViewpoint(vsVector newPosition)
{
    // Set the viewpoint's position to the given vector
    viewLocation.clearCopy(newPosition);

    // Make sure the vector is the correct size
    viewLocation.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the current viewpoint. NULL pointers may be passed in for
// undesired values.
// ------------------------------------------------------------------------
void vsView::getViewpoint(double *xPosition, double *yPosition,
                          double *zPosition)
{
    // Return the corresponding position coordinate for each valid parameter
    if (xPosition)
        *xPosition = viewLocation[VS_X];
    if (yPosition)
        *yPosition = viewLocation[VS_Y];
    if (zPosition)
        *zPosition = viewLocation[VS_Z];
}

// ------------------------------------------------------------------------
// Retrieves the current viewpoint
// ------------------------------------------------------------------------
vsVector vsView::getViewpoint()
{
    return viewLocation;
}

// ------------------------------------------------------------------------
// Sets the current orientation, using a 'forward' vector and an 'up'
// vector.
// ------------------------------------------------------------------------
void vsView::setDirectionFromVector(vsVector direction, vsVector upDirection)
{
    vsVector directionVec, upDirectionVec;
    vsVector forwardVec, upVec, rotAxisVec;
    double rotDegrees;
    vsVector tempVec;
    vsQuat dirRotQuat, upRotQuat;

    // * First, create a quaternion that rotates the Performer basis
    // (Y-axis) to the desired direction

    // Set the forward vector to the positive Y-axis (Performer's "forward"
    // direction)
    forwardVec.set(0.0, 1.0, 0.0);

    // Copy and normalize the specified direction vector
    directionVec.clearCopy(direction);
    directionVec.setSize(3);
    directionVec.normalize();

    // Get the vector orthogonal to both the forward and direction vectors
    // (this is the rotation axis for these two vectors)
    rotAxisVec = forwardVec.getCrossProduct(directionVec);

    // If there is no valid rotation axis, use the positive Z axis as a
    // default
    if (fabs(rotAxisVec.getMagnitude()) < 1E-6)
        rotAxisVec.set(0.0, 0.0, 1.0);

    // Get the angle between the forward and direction vectors
    rotDegrees = forwardVec.getAngleBetween(directionVec);

    // Set up a quaternion using the rotation axis and angle we just
    // computed
    dirRotQuat.setAxisAngleRotation(rotAxisVec[VS_X], rotAxisVec[VS_Y],
        rotAxisVec[VS_Z], rotDegrees);
    
    // * Second, create a quaternion that rotates the up directions to match,
    // taking into account the first rotation

    // Set the up vector to be the positive Z-axis (Performer's "up" 
    // direction)
    upVec.set(0.0, 0.0, 1.0);

    // Rotate this up vector using the first quaternion we computed
    upVec = dirRotQuat.rotatePoint(upVec);
    
    // Make sure that 'upDirection' is at a right angle to 'direction'
    // First compute the cross product of the newly rotated 'upDirection'
    // vector and the 'direction' vector computed above.
    tempVec = upDirection.getCrossProduct(directionVec);

    // Next, find the new 'up' direction (which will be at a right angle
    // to 'direction') by computing the cross product of the direction
    // vector and the temporary vector we just computed above.
    upDirectionVec = directionVec.getCrossProduct(tempVec);

    // Normalize the new 'up' vector
    upDirectionVec.normalize();
    
    // Find the "twist" axis of rotation by taking the cross product
    // of the original up vector with the new up direction that we
    // just computed.
    rotAxisVec = upVec.getCrossProduct(upDirectionVec);

    // Use the Y-axis rotated by the first quaternion we computed if the 
    // cross product is near zero
    if (fabs(rotAxisVec.getMagnitude()) < 1E-6)
    {
        rotAxisVec.set(0.0, 1.0, 0.0);
        rotAxisVec = dirRotQuat.rotatePoint(rotAxisVec);
    }

    // Get the angle between the original up vector and the new up vector
    rotDegrees = upVec.getAngleBetween(upDirectionVec);

    // Set up the second quaternion, which aligns the up direction's of the
    // basis forward vector and the specified direction vector.  Use the
    // axis and angle we just computed to set this up
    upRotQuat.setAxisAngleRotation(rotAxisVec[VS_X], rotAxisVec[VS_Y],
        rotAxisVec[VS_Z], rotDegrees);

    // * Finally, set the view orientation matrix as a composition of the
    // two quaternions
    viewRotation.setQuatRotation(upRotQuat * dirRotQuat);
}

// ------------------------------------------------------------------------
// Sets the current orientation, using a 'target' location as the desired
// place to look in the direction of, and an 'up' direction vector.
// ------------------------------------------------------------------------
void vsView::lookAtPoint(vsVector targetPoint, vsVector upDirection)
{
    vsVector directionVec;
    
    // Compute the direction vector as the difference of the target from
    // the current viewpoint
    directionVec = targetPoint - viewLocation;
    
    // Call the setDirectionFromVector method using the direction vector
    // computed and the specified up direction
    setDirectionFromVector(directionVec, upDirection);
}

// ------------------------------------------------------------------------
// Sets the current orientation directly from a rotational quaternion
// ------------------------------------------------------------------------
void vsView::setDirectionFromRotation(vsQuat rotQuat)
{
    // Set the rotation matrix of the viewpoint using the given quaternion
    viewRotation.setQuatRotation(rotQuat);
}

// ------------------------------------------------------------------------
// Sets the current orientation directly from a rotation matrix. Removes
// any scaling and translation on the new matrix.
// ------------------------------------------------------------------------
void vsView::setDirectionFromRotation(vsMatrix rotMatrix)
{
    int loop;

    // Copy the rotation matrix
    viewRotation = rotMatrix;
    
    // Eliminate any translations from the matrix (we only want rotation 
    // here)
    for (loop = 0; loop < 3; loop++)
    {
        viewRotation[loop][3] = 0.0;
        viewRotation[3][loop] = 0.0;
    }
    
    // Eliminate any overall scale from the matrix (we only want rotation
    // here)
    viewRotation[3][3] = 1.0;
}

// ------------------------------------------------------------------------
// Sets the distances from the viewer of the near and far clipping planes
// ------------------------------------------------------------------------
void vsView::setClipDistances(double nearPlane, double farPlane)
{
    // Copy the clip plane values (these are used by the attached
    // vsPane's updateView() method)
    nearClip = nearPlane;
    farClip = farPlane;
}

// ------------------------------------------------------------------------
// Retrieves the distances from the viewer of the near and far clipping
// planes. NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsView::getClipDistances(double *nearPlane, double *farPlane)
{
    // For each valid parameter, return the corresponding clip plane
    // distance
    if (nearPlane)
        *nearPlane = nearClip;
    if (farPlane)
        *farPlane = farClip;
}    

// ------------------------------------------------------------------------
// Sets the projection mode of the viewpoint to a perspective projection
// with the given horizontal and vertical fields of view. If either of the
// parameters are zero or less, then the value for that parameter is
// calculated using the aspect ratio of the associated vsPane. If both
// parameters are zero or less, then default field-of-view values are used.
// ------------------------------------------------------------------------
void vsView::setPerspective(double horizFOV, double vertiFOV)
{
    // Set the projectsion to perspective
    projMode = VS_VIEW_PROJMODE_PERSP;

    // Set the field of view parameters to the specified values
    projHval = horizFOV;
    projVval = vertiFOV;
}

// ------------------------------------------------------------------------
// Sets the projection mode of the viewpoint to an orthogonal projection
// with the given values as the distances from the center point of the view
// to the sides of the viewing volume. If either of the parameters are zero
// or less, then the value for that parameter is calculated using the
// aspect ratio of the associated vsPane. If both parameters are zero or
// less, then default values are used.
// ------------------------------------------------------------------------
void vsView::setOrthographic(double horizSize, double vertiSize)
{
    // Set the projection to orthographic
    projMode = VS_VIEW_PROJMODE_ORTHO;

    // Set the width and height to the specified values
    projHval = horizSize;
    projVval = vertiSize;
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current view direction
// ------------------------------------------------------------------------
vsVector vsView::getDirection()
{
    vsVector result;
    
    // Set up a forward vector and rotate it by the current view rotation
    // to get the view direction vector
    result.set(0.0, 1.0, 0.0);
    result = viewRotation.getVectorXform(result);
    
    // Return the resulting vector
    return result;
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current perceived up direction
// ------------------------------------------------------------------------
vsVector vsView::getUpDirection()
{
    vsVector result;
    
    // Set up an up vector and rotate it by the current view rotation
    // to get the view's up vector
    result.set(0.0, 0.0, 1.0);
    result = viewRotation.getVectorXform(result);
    
    // Return the resulting vector
    return result;
}

// ------------------------------------------------------------------------
// Retrieves the current view rotation quaternion
// ------------------------------------------------------------------------
vsMatrix vsView::getRotationMat()
{
    return viewRotation;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the projection mode parameters
// ------------------------------------------------------------------------
void vsView::getProjectionData(int *mode, double *horizVal, double *vertiVal)
{
    // For each valid parameter given, return the corresponding projection 
    // settings
    if (mode != NULL)
        *mode = projMode;
    if (horizVal != NULL)
        *horizVal = projHval;
    if (vertiVal != NULL)
        *vertiVal = projVval;
}

// ------------------------------------------------------------------------
// Internal function
// Signals to this viewpoint object that its data is being controlled
// by the indicated viewpoint attribute.
// ------------------------------------------------------------------------
int vsView::attachViewAttribute(vsViewpointAttribute *theAttribute)
{
    // Fail if a viewpoint attribute is already attached
    if (viewAttribute)
    {
        printf("vsView::attachViewAttribute: View object is already "
            "controlled by a vsViewpointAttribute\n");
        return VS_FALSE;
    }
    
    // Keep track of the viewpoint attribute we're attaching to
    viewAttribute = theAttribute;
    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Internal function
// Signals to this viewpoint object that its data is no longer being
// controlled by any viewpoint attribute.
// ------------------------------------------------------------------------
void vsView::detachViewAttribute()
{
    // Let go of the viewpoint attribute we're attached to
    viewAttribute = NULL;
}

// ------------------------------------------------------------------------
// Internal function
// Commands the viewpoint object's viewpoint attribute to update the
// viewpoint object's current position and orientation data.
// ------------------------------------------------------------------------
void vsView::updateFromAttribute()
{
    // If we have a viewpoint attribute attached, call its update() method
    // which will update our viewpoint settings based on the component
    // it is attached to
    if (viewAttribute)
        viewAttribute->update();
}
