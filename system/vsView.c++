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
    // Set the viewpoint location to the origin and the initial rotation
    // to an identity matrix
    viewLocation.setSize(3);
    viewLocation.clear();
    viewRotation.setIdentity();
    
    // Start off without a viewpoint attribute
    viewAttribute = NULL;
    
    // Set the near and far clipping plane distances to default values
    nearClip = 0.1;
    farClip = 10000.0;
    
    // Set the default projection mode to perspective projection, with a
    // default field-of-view
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
// Sets the current viewpoint
// ------------------------------------------------------------------------
void vsView::setViewpoint(double xPosition, double yPosition, double zPosition)
{
    viewLocation.set(xPosition, yPosition, zPosition);
}

// ------------------------------------------------------------------------
// Sets the current viewpoint
// ------------------------------------------------------------------------
void vsView::setViewpoint(vsVector newPosition)
{
    // Force the copied vector to have size 3
    viewLocation.clearCopy(newPosition);
    viewLocation.setSize(3);
}

// ------------------------------------------------------------------------
// Retrieves the current viewpoint. NULL pointers may be passed in for
// undesired values.
// ------------------------------------------------------------------------
void vsView::getViewpoint(double *xPosition, double *yPosition,
                          double *zPosition)
{
    // Return the desired elements of the current viewpoint position
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

    // Create a y-axis vector and clean up the direction vector
    forwardVec.set(0.0, 1.0, 0.0);
    directionVec.clearCopy(direction);
    directionVec.setSize(3);
    directionVec.normalize();

    // Compute the axis of rotation by taking the cross product of the
    // two vectors
    rotAxisVec = forwardVec.getCrossProduct(directionVec);
    if (fabs(rotAxisVec.getMagnitude()) < 1E-6)
        rotAxisVec.set(0.0, 0.0, 1.0);

    // Compute the amount of rotation by using the vsVector.getAngleBetween
    // function on the two vectors
    rotDegrees = forwardVec.getAngleBetween(directionVec);

    // Set the initial rotation from the axis and angle data
    dirRotQuat.setAxisAngleRotation(rotAxisVec[VS_X], rotAxisVec[VS_Y],
        rotAxisVec[VS_Z], rotDegrees);
    
    // * Second, create a quaternion that rotates the up directions to match,
    // taking into account the first rotation

    // Create a z-axis vector, and rotate it to compensate for the
    // rotation we've calculated so far
    upVec.set(0.0, 0.0, 1.0);
    upVec = dirRotQuat.rotatePoint(upVec);
    
    // Make sure that 'upDirection' is at a right angle to 'direction' by
    // taking the cross product of upDirection and direction to get a third
    // vector; this third vector, along with the direction vector, describe
    // a plane that the upDirection must be perpendicular to. Then find
    // a new upDirection perpendicular to that plane by taking another cross
    // product, this time of the direction vector and the third vector.
    tempVec = upDirection.getCrossProduct(directionVec);
    upDirectionVec = directionVec.getCrossProduct(tempVec);
    upDirectionVec.normalize();
    
    // Compute the axis to rotate around for the roll rotation by taking
    // the cross product of the starting and target up direction vectors
    rotAxisVec = upVec.getCrossProduct(upDirectionVec);
    if (fabs(rotAxisVec.getMagnitude()) < 1E-6)
    {
	// In the case that the cross product is zero (indicating that the
	// two up directions are parallel), use the y-axis as the rotation
	// axis instead, adjusted for the first rotation
        rotAxisVec.set(0.0, 1.0, 0.0);
        rotAxisVec = dirRotQuat.rotatePoint(rotAxisVec);
    }

    // Compute the amount of rotation by finding the angle between the
    // two up vectors
    rotDegrees = upVec.getAngleBetween(upDirectionVec);

    // Set the roll rotation from the axis and angle data
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
    
    // Cheat: Determine the view direction by calculating the vector from
    // the current viewpoint to the desired target location, and call the
    // setDirectionFromVector function to set the orientation.
    directionVec = targetPoint - viewLocation;
    setDirectionFromVector(directionVec, upDirection);
}

// ------------------------------------------------------------------------
// Sets the current orientation directly from a rotational quaternion
// ------------------------------------------------------------------------
void vsView::setDirectionFromRotation(vsQuat rotQuat)
{
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
    
    // Zero out the translation and non-uniform scale portions of
    // the matrix
    for (loop = 0; loop < 3; loop++)
    {
        viewRotation[loop][3] = 0.0;
        viewRotation[3][loop] = 0.0;
    }
    
    // Set the uniform scale of the matrix to one (identity)
    viewRotation[3][3] = 1.0;
}

// ------------------------------------------------------------------------
// Sets the distances from the viewer of the near and far clipping planes
// ------------------------------------------------------------------------
void vsView::setClipDistances(double nearPlane, double farPlane)
{
    nearClip = nearPlane;
    farClip = farPlane;
}

// ------------------------------------------------------------------------
// Retrieves the distances from the viewer of the near and far clipping
// planes. NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsView::getClipDistances(double *nearPlane, double *farPlane)
{
    // Return only the desired values
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
    // Set the projection mode to perspective and store the FOV values
    projMode = VS_VIEW_PROJMODE_PERSP;
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
    // Set the projection mode to orthogonal and store the size values
    projMode = VS_VIEW_PROJMODE_ORTHO;
    projHval = horizSize;
    projVval = vertiSize;
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current view direction
// ------------------------------------------------------------------------
vsVector vsView::getDirection()
{
    vsVector result;
    
    // Create a forward (y-axis) vector, and transform it by the current
    // view rotation
    result.set(0.0, 1.0, 0.0);
    result = viewRotation.getVectorXform(result);
    
    // Return the computed vector
    return result;
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current perceived up direction
// ------------------------------------------------------------------------
vsVector vsView::getUpDirection()
{
    vsVector result;
    
    // Create an up (z-axis) vector, and transform it by the current
    // view rotation
    result.set(0.0, 0.0, 1.0);
    result = viewRotation.getVectorXform(result);
    
    // Return the computed vector
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
// VESS internal function
// Retrieves the projection mode parameters
// ------------------------------------------------------------------------
void vsView::getProjectionData(int *mode, double *horizVal, double *vertiVal)
{
    // Return the projection data for this object; no NULL checking because
    // the function is internal.
    *mode = projMode;
    *horizVal = projHval;
    *vertiVal = projVval;
}

// ------------------------------------------------------------------------
// VESS internal function
// Signals to this viewpoint object that its data is being controlled
// by the indicated viewpoint attribute.
// ------------------------------------------------------------------------
int vsView::attachViewAttribute(vsViewpointAttribute *theAttribute)
{
    // Verify that we don't already have a viewpoint attribute
    if (viewAttribute)
    {
        printf("vsView::attachViewAttribute: View object is already "
            "controlled by a vsViewpointAttribute\n");
        return 0;
    }
    
    // Save the attribute pointer and return success
    viewAttribute = theAttribute;
    return 1;
}

// ------------------------------------------------------------------------
// VESS internal function
// Signals to this viewpoint object that its data is no longer being
// controlled by any viewpoint attribute.
// ------------------------------------------------------------------------
void vsView::detachViewAttribute()
{
    // Clear the attribute pointer
    viewAttribute = NULL;
}

// ------------------------------------------------------------------------
// VESS internal function
// Commands the viewpoint object's viewpoint attribute to update the
// viewpoint object's current position and orientation data.
// ------------------------------------------------------------------------
void vsView::updateFromAttribute()
{
    // Give an update call to our associated viewpoint attribute, if there
    // is one.
    if (viewAttribute)
        viewAttribute->update();
}
