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
    // Create the osg::Camera to manage the viewpoint
    osgCamera = new osg::Camera();
    osgCamera->ref();

    // Set the view to a default perspective projection
    setPerspective(0.0, 0.0);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsView::~vsView()
{
    osgCamera->unref();
}

// ------------------------------------------------------------------------
// Sets the current viewpoint
// ------------------------------------------------------------------------
void vsView::setViewpoint(double xPosition, double yPosition, double zPosition)
{
    vsVector position;

    // Create a vsVector with the new position
    position.set(xPosition, yPosition, zPosition);

    // Set the new position using the other setViewpoint() method
    setViewpoint(position);
}

// ------------------------------------------------------------------------
// Sets the current viewpoint
// ------------------------------------------------------------------------
void vsView::setViewpoint(vsVector newPosition)
{
    osg::Vec3 oldPos;
    osg::Vec3 newPos;
    osg::Vec3 transVec;
    osg::Vec3 centerPoint;

    // Translate the vsVector to an OSG::Vec3
    newPos.set(newPosition[VS_X], newPosition[VS_Y], newPosition[VS_Z]);

    // OSG only allows you to specify orientation using a "lookAt" 
    // mechanism.  In order to maintain the current orientation while
    // moving the viewpoint, we need to translate the center point
    // the same amount as the viewpoint is moving.  First, get the old
    // position.
    oldPos = osgCamera->getEyePoint();

    // Calculate the translation vector (the amount the viewpoint is
    // moving).
    transVec = newPos - oldPos;

    // Get the current center point, and apply the same translation to
    // it
    centerPoint = osgCamera->getCenterPoint();
    centerPoint += transVec;

    // Set the new position and new center point.  The up vector can
    // stay the same
    osgCamera->setLookAt(newPos, centerPoint, osgCamera->getUpVector());
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
        *xPosition = (double)(osgCamera->getEyePoint().x());
    if (yPosition)
        *yPosition = (double)(osgCamera->getEyePoint().y());
    if (zPosition)
        *zPosition = (double)(osgCamera->getEyePoint().z());
}

// ------------------------------------------------------------------------
// Retrieves the current viewpoint
// ------------------------------------------------------------------------
vsVector vsView::getViewpoint()
{
    osg::Vec3 eyePoint;
    vsVector viewLocation;

    // Get the eye point of the camera
    eyePoint = osgCamera->getEyePoint();

    // Convert to a vsVector
    viewLocation.set(eyePoint[0], eyePoint[1], eyePoint[2]);

    // Return the vector
    return viewLocation;
}

// ------------------------------------------------------------------------
// Sets the current orientation, using a 'forward' vector and an 'up'
// vector.
// ------------------------------------------------------------------------
void vsView::setDirectionFromVector(vsVector direction, vsVector upDirection)
{
    vsVector eyePoint;
    vsVector targetPoint;

    // Get the current viewpoint
    eyePoint = getViewpoint();

    // Compute a target point based on the given direction
    targetPoint = eyePoint + direction;

    // Set the camera
    lookAtPoint(targetPoint, upDirection);
}

// ------------------------------------------------------------------------
// Sets the current orientation, using a 'target' location as the desired
// place to look in the direction of, and an 'up' direction vector.
// ------------------------------------------------------------------------
void vsView::lookAtPoint(vsVector targetPoint, vsVector upDirection)
{
    osg::Vec3 eyePoint;
    osg::Vec3 center;
    osg::Vec3 up;

    // Get the eye point from the camera
    eyePoint = osgCamera->getEyePoint();

    // Convert the target point to an osg::Vec3
    center.set(targetPoint[VS_X], targetPoint[VS_Y], targetPoint[VS_Z]);

    // Convert the up direction to an osg::Vec3
    up.set(upDirection[VS_X], upDirection[VS_Y], upDirection[VS_Z]);

    // Set the camera
    osgCamera->setLookAt(eyePoint, center, up);
}

// ------------------------------------------------------------------------
// Sets the current orientation directly from a rotational quaternion
// ------------------------------------------------------------------------
void vsView::setDirectionFromRotation(vsQuat rotQuat)
{
    vsVector forward, up;

    // Set up two base vectors
    forward.set(0.0, 1.0, 0.0);
    up.set(0.0, 0.0, 1.0);

    // Rotate the vectors by the given quaternion
    forward = rotQuat.rotatePoint(forward);
    up = rotQuat.rotatePoint(up);

    // Set the camera
    setDirectionFromVector(forward, up);
}

// ------------------------------------------------------------------------
// Sets the current orientation directly from a rotation matrix. Removes
// any scaling and translation on the new matrix.
// ------------------------------------------------------------------------
void vsView::setDirectionFromRotation(vsMatrix rotMatrix)
{
    vsVector forward, up;

    // Set up two base vectors
    forward.set(0.0, 1.0, 0.0);
    up.set(0.0, 0.0, 1.0);

    // Rotate the vectors by the given matrix
    forward = rotMatrix.getVectorXform(forward);
    up = rotMatrix.getVectorXform(up);

    // Set the camera
    setDirectionFromVector(forward, up);
}

// ------------------------------------------------------------------------
// Sets the distances from the viewer of the near and far clipping planes
// ------------------------------------------------------------------------
void vsView::setClipDistances(double nearPlane, double farPlane)
{
    // Pass the new near and far values to the osg::Camera
    osgCamera->setNearFar((float)nearPlane, (float)farPlane);
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
        *nearPlane = osgCamera->zNear();
    if (farPlane)
        *farPlane = osgCamera->zFar();
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
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current view direction
// ------------------------------------------------------------------------
vsVector vsView::getDirection()
{
    osg::Vec3 direction;
    vsVector result;
    
    // Get camera direction
    direction = osgCamera->getLookVector();

    // Convert to a vsVector
    result.set(direction[0], direction[1], direction[2]);
    
    return result;
}

// ------------------------------------------------------------------------
// Returns a vector indicating the current perceived up direction
// ------------------------------------------------------------------------
vsVector vsView::getUpDirection()
{
    osg::Vec3 upDirection;
    vsVector result;

    // Get camera up vector
    upDirection = osgCamera->getUpVector();
    
    // Convert to a vsVector
    result.set(upDirection[0], upDirection[1], upDirection[2]);
    
    return result;
}

// ------------------------------------------------------------------------
// Retrieves the current view rotation quaternion
// ------------------------------------------------------------------------
vsMatrix vsView::getRotationMat()
{
    osg::Matrix modelView;
    vsMatrix rotationMat;
    int i, j;

    // Get the modelview matrix from the camera
    modelView = osgCamera->getModelViewMatrix();

    // Convert to a vsMatrix
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            rotationMat[i][j] = modelView(j, i);
        }
    }

    return rotationMat;
}

// ------------------------------------------------------------------------
// Returns the underlying osg::Camera
// ------------------------------------------------------------------------
osg::Camera *vsView::getBaseLibraryObject()
{
    // Return the osg::Camera we created
    return osgCamera;
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
