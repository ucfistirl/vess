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
//    Author(s):    Bryan Kline, Jason Daly
//
//------------------------------------------------------------------------

#include "vsView.h++"
#include <osg/Vec3>
#include <osg/Matrix>

// ------------------------------------------------------------------------
// Constructor - Initializes the position and orientation
// ------------------------------------------------------------------------
vsView::vsView()
{
    // Initialize to defaults
    viewpoint.set(0.0, 0.0, 0.0);
    forwardDir.set(0.0, 1.0, 0.0);
    upDir.set(0.0, 0.0, 1.0);
    nearClipDist = 0.1;
    farClipDist = 10000.0;

    // Set the view to a default perspective projection
    setPerspective(0.0, 0.0);

    // Initialize the change counter
    changeNum = 0;
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
    // Set the viewpoint
    viewpoint.set(xPosition, yPosition, zPosition);

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Sets the current viewpoint
// ------------------------------------------------------------------------
void vsView::setViewpoint(atVector newPosition)
{
    // Set the viewpoint, forcing the vector size to 3
    viewpoint.clearCopy(newPosition);

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Retrieves the current viewpoint. NULL pointers may be passed in for
// undesired values.
// ------------------------------------------------------------------------
void vsView::getViewpoint(double *xPosition, double *yPosition,
                          double *zPosition)
{
    // Return each eye point coordinate if the corresponding parameter 
    // is valid
    if (xPosition)
        *xPosition = viewpoint[0];
    if (yPosition)
        *yPosition = viewpoint[1];
    if (zPosition)
        *zPosition = viewpoint[2];
}

// ------------------------------------------------------------------------
// Retrieves the current viewpoint
// ------------------------------------------------------------------------
atVector vsView::getViewpoint()
{
    return viewpoint;
}

// ------------------------------------------------------------------------
// Sets the current orientation, using a 'forward' vector and an 'up'
// vector.
// ------------------------------------------------------------------------
void vsView::setDirectionFromVector(atVector direction, atVector upDirection)
{
    // Copy the forward direction, forcing its size to 3 and normalizing it
    forwardDir.clearCopy(direction);
    forwardDir.normalize();

    // Copy the up direction, forcing its size to 3 and normalizing it
    upDir.clearCopy(upDirection);
    upDir.normalize();

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Sets the current orientation, using a 'target' location as the desired
// place to look in the direction of, and an 'up' direction vector.
// ------------------------------------------------------------------------
void vsView::lookAtPoint(atVector targetPoint, atVector upDirection)
{
    atVector dir;

    // Calculate the direction of view as the difference of the current
    // viewpoint and the target point
    dir.setSize(3);
    dir.clearCopy(targetPoint);
    dir = dir - viewpoint;
    forwardDir = dir.getNormalized();

    // Copy the up direction, forcing its size to 3 and normalizing it
    upDir.clearCopy(upDirection);
    upDir.normalize();

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Sets the current orientation directly from a rotational quaternion
// ------------------------------------------------------------------------
void vsView::setDirectionFromRotation(atQuat rotQuat)
{
    atVector forward, up;

    // Set up two base vectors
    forward.set(0.0, 1.0, 0.0);
    up.set(0.0, 0.0, 1.0);

    // Rotate the vectors by the given quaternion
    forward = rotQuat.rotatePoint(forward);
    up = rotQuat.rotatePoint(up);

    // Set the forward and up directions, and make sure they're normalized
    forwardDir = forward.getNormalized();
    upDir = up.getNormalized();

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Sets the current orientation directly from a rotation matrix. Removes
// any scaling and translation on the new matrix.
// ------------------------------------------------------------------------
void vsView::setDirectionFromRotation(atMatrix rotMatrix)
{
    atVector forward, up;

    // Set up two base vectors
    forward.set(0.0, 1.0, 0.0);
    up.set(0.0, 0.0, 1.0);

    // Rotate the vectors by the given matrix
    forward = rotMatrix.getVectorXform(forward);
    up = rotMatrix.getVectorXform(up);

    // Set the forward and up directions, and make sure they're normalized
    forwardDir = forward.getNormalized();
    upDir = up.getNormalized();

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Sets the distances from the viewer of the near and far clipping planes
// ------------------------------------------------------------------------
void vsView::setClipDistances(double nearPlane, double farPlane)
{
    // Copy the near and far plane values
    nearClipDist = nearPlane;
    farClipDist = farPlane;

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Retrieves the distances from the viewer of the near and far clipping
// planes. NULL pointers may be passed in for undesired values.
// ------------------------------------------------------------------------
void vsView::getClipDistances(double *nearPlane, double *farPlane)
{
    // Return each plane's distance, if the corresponding parameter is 
    // valid
    if (nearPlane)
        *nearPlane = nearClipDist;
    if (farPlane)
        *farPlane = farClipDist;
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
    // Camera manipulation is deferred for projection functions, since
    // we can't determine the size of the pane here. See 
    // vsPane::updateView(), where these values are retrieved and 
    // utilized.
    projMode = VS_VIEW_PROJMODE_PERSP;
    projHval = horizFOV;
    projVval = vertiFOV;

    // Mark that a change was made
    changeNum++;
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
    // Camera manipulation is deferred for projection functions, since
    // we can't determine the size of the pane here. See 
    // vsPane::updateView(), where these values are retrieved and 
    // utilized.
    projMode = VS_VIEW_PROJMODE_ORTHO;
    projHval = horizSize;
    projVval = vertiSize;

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Sets the projection mode of the viewpoint to an off-axis perspective 
// projection with the given values as the distances from the center point 
// of the view to the sides of the viewing volume.  All values must be
// specified explicitly.
// ------------------------------------------------------------------------
void vsView::setOffAxisPerspective(double left, double right, double bottom, 
                                   double top)
{
    // Camera manipulation is deferred for projection functions, since
    // we can't determine the size of the pane here. See 
    // vsPane::updateView(), where these values are retrieved and 
    // utilized.
    projMode = VS_VIEW_PROJMODE_OFFAXIS_PERSP;
    projLeft = left;
    projRight = right;
    projBottom = bottom;
    projTop = top;

    // Mark that a change was made
    changeNum++;
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current view direction
// ------------------------------------------------------------------------
atVector vsView::getDirection()
{
    return forwardDir;
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current perceived up direction
// ------------------------------------------------------------------------
atVector vsView::getUpDirection()
{
    return upDir;
}

// ------------------------------------------------------------------------
// Retrieves the current view rotation quaternion
// ------------------------------------------------------------------------
atMatrix vsView::getRotationMat()
{
    atQuat rotationQuat;
    atMatrix result;

    // Find the quaternion that rotates the origin directions to the
    // current directions
    rotationQuat.setVecsRotation(atVector(0.0, 1.0, 0.0),
                                 atVector(0.0, 0.0, 1.0),
                                 forwardDir,
                                 upDir);

    // Create a rotation matrix from the quaternion and return it
    result.setQuatRotation(rotationQuat);

    return result;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the projection mode parameters.
// ------------------------------------------------------------------------
void vsView::getProjectionData(int *mode, double *horizVal, double *vertiVal)
{
    // Return the projection values
    *mode = projMode;
    *horizVal = projHval;
    *vertiVal = projVval;
}

// ------------------------------------------------------------------------
// Internal function
// Retrieves the data for off-axis projections
// ------------------------------------------------------------------------
void vsView::getOffAxisProjectionData(double *left, double *right, 
                                      double *bottom, double *top)
{
    // Return the projection values
    *left = projLeft;
    *right = projRight;
    *bottom = projBottom;
    *top = projTop;
}

// ------------------------------------------------------------------------
// Internal function
// Gets the "change number" for this object. The change number is a value
// that is incremented every time some parameter of the view is modified.
// ------------------------------------------------------------------------
int vsView::getChangeNum()
{
    return changeNum;
}
