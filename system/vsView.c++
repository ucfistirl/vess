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
    viewLocation.setSize(3);
    viewLocation.clear();
    viewRotation.setIdentity();
    
    viewAttribute = NULL;
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
    forwardVec.set(0.0, 1.0, 0.0);
    directionVec.clearCopy(direction);
    directionVec.setSize(3);
    directionVec.normalize();
    rotAxisVec = forwardVec.getCrossProduct(directionVec);
    if (fabs(rotAxisVec.getMagnitude()) < 1E-6)
        rotAxisVec.set(0.0, 0.0, 1.0);
    rotDegrees = forwardVec.getAngleBetween(directionVec);
    dirRotQuat.setAxisAngleRotation(rotAxisVec[VS_X], rotAxisVec[VS_Y],
        rotAxisVec[VS_Z], rotDegrees);
    
    // * Second, create a quaternion that rotates the up directions to match,
    // taking into account the first rotation
    upVec.set(0.0, 0.0, 1.0);
    upVec = dirRotQuat.rotatePoint(upVec);
    
    // Make sure that 'upDirection' is at a right angle to 'direction'
    tempVec = upDirection.getCrossProduct(directionVec);
    upDirectionVec = directionVec.getCrossProduct(tempVec);
    upDirectionVec.normalize();
    
    rotAxisVec = upVec.getCrossProduct(upDirectionVec);
    if (fabs(rotAxisVec.getMagnitude()) < 1E-6)
    {
        rotAxisVec.set(0.0, 1.0, 0.0);
        rotAxisVec = dirRotQuat.rotatePoint(rotAxisVec);
    }
    rotDegrees = upVec.getAngleBetween(upDirectionVec);
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

    viewRotation = rotMatrix;
    
    for (loop = 0; loop < 3; loop++)
    {
        viewRotation[loop][3] = 0.0;
        viewRotation[3][loop] = 0.0;
    }
    
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
    if (nearPlane)
        *nearPlane = nearClip;
    if (farPlane)
        *farPlane = farClip;
}    

// ------------------------------------------------------------------------
// Returns a vector indicating the current view direction
// ------------------------------------------------------------------------
vsVector vsView::getDirection()
{
    vsVector result;
    
    result.set(0.0, 1.0, 0.0);
    result = viewRotation.getVectorXform(result);
    
    return result;
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current perceived up direction
// ------------------------------------------------------------------------
vsVector vsView::getUpDirection()
{
    vsVector result;
    
    result.set(0.0, 0.0, 1.0);
    result = viewRotation.getVectorXform(result);
    
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
// Signals to this viewpoint object that its data is being controlled
// by the indicated viewpoint attribute.
// ------------------------------------------------------------------------
int vsView::attachViewAttribute(vsViewpointAttribute *theAttribute)
{
    if (viewAttribute)
    {
        printf("vsView::attachViewAttribute: View object is already "
            "controlled by a vsViewpointAttribute\n");
        return 0;
    }
    
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
    viewAttribute = NULL;
}

// ------------------------------------------------------------------------
// VESS internal function
// Commands the viewpoint object's viewpoint attribute to update the
// viewpoint object's current position and orientation data.
// ------------------------------------------------------------------------
void vsView::updateFromAttribute()
{
    if (viewAttribute)
        viewAttribute->update();
}
