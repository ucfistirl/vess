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
//    VESS Module:  vsRelativeObjectMotion.c++
//
//    Description:  A class to allow any kinematics (the object) to move
//                  based on the movements of a second kinematics (the
//                  manipulator).  Simple positional and rotational
//                  constraints are provided.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#include "vsRelativeObjectMotion.h++"

// ------------------------------------------------------------------------
// Constructs a relative object motion model with the given object and
// manipulator kinematics objects
// ------------------------------------------------------------------------
vsRelativeObjectMotion::vsRelativeObjectMotion(vsKinematics *objKin,
                                               vsKinematics *manipKin)
{
    // Save the kinematics
    objectKin = objKin;
    manipulatorKin = manipKin;

    // Set the default constraint modes to FREE
    translationMode = VS_ROM_TRANS_FREE;
    rotationMode = VS_ROM_ROT_FREE;

    // Start with the object detached
    attachedFlag = false;
}

// ------------------------------------------------------------------------
// Destructor.  Does nothing.
// ------------------------------------------------------------------------
vsRelativeObjectMotion::~vsRelativeObjectMotion()
{
}

// ------------------------------------------------------------------------
// Return the name of the class
// ------------------------------------------------------------------------
const char *vsRelativeObjectMotion::getClassName()
{
    return "vsRelativeObjectMotion";
}

// ------------------------------------------------------------------------
// Sets the translation constraint mode to locked (no translation allowed)
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::lockTranslation()
{
    // Set the new mode
    translationMode = VS_ROM_TRANS_LOCKED;
}

// ------------------------------------------------------------------------
// Sets the translation constraint mode to line (translation only allowed
// along the given line).  The axis must be specified in world coordinates.
// The line is assumed to originate at the object's current position.
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::constrainTranslationToLine(atVector axis)
{
    // Set the new mode
    translationMode = VS_ROM_TRANS_LINE;

    // Save the line constraint parameters
    transVector.clearCopy(axis);
    transVector.setSize(3);
    transVector.normalize();
}

// ------------------------------------------------------------------------
// Sets the translation constraint mode to plane (translation only allowed
// along the given plane).  The object is assumed to already be on the
// constraint plane, thus the plane originates at the object's current
// position.
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::constrainTranslationToPlane(atVector normal)
{
    // Set the new mode
    translationMode = VS_ROM_TRANS_PLANE;

    // Save the plane constraint parameters
    transVector.clearCopy(normal);
    transVector.setSize(3);
    transVector.normalize();
}

// ------------------------------------------------------------------------
// Sets the translation constraint mode to free (any translation allowed)
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::freeTranslation()
{
    // Set the new mode
    translationMode = VS_ROM_TRANS_FREE;
}

// ------------------------------------------------------------------------
// Sets the rotation constraint mode to locked (no rotation allowed).  Axis
// must be specified in world coordinates.
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::lockRotation()
{
    // Set the new mode
    rotationMode = VS_ROM_ROT_LOCKED;
}

// ------------------------------------------------------------------------
// Sets the rotation constraint mode to axis (rotation allowed only around
// the given axis)
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::constrainRotationToAxis(atVector axis)
{
    // Set the new mode
    rotationMode = VS_ROM_ROT_AXIS;

    // Save the rotation axis
    rotAxis.clearCopy(axis);
    rotAxis.setSize(3);
}

// ------------------------------------------------------------------------
// Sets the rotation constraint mode to free (any rotation allowed)
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::freeRotation()
{
    // Set the new mode
    rotationMode = VS_ROM_ROT_FREE;
}

// ------------------------------------------------------------------------
// Attach the object to the manipulator.  This causes the object to follow
// the manipulator's movements within the given constraints.
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::attachObject()
{
    atMatrix manipulatorMat, invManipulatorMat;
    atQuat invManipulatorQuat;
    atMatrix objectMat;
    atVector objectPos;
    atVector objectManipulatorPos;
    atQuat objectOrn;
    atQuat objectManipulatorOrn;

    // Get the global and inverse global transform for the manipulator
    manipulatorMat = manipulatorKin->getComponent()->getGlobalXform();
    invManipulatorMat = manipulatorMat.getInverse();
    invManipulatorQuat.setMatrixRotation(invManipulatorMat);
    
    // Get the object's position in manipulator coordinates
    objectMat = objectKin->getComponent()->getGlobalXform();
    objectPos = objectMat.getPointXform(atVector(0,0,0));
    objectManipulatorPos = invManipulatorMat.getPointXform(objectPos);

    // Get the object's orientation in manipulator coordinates
    objectOrn = objectKin->getOrientation();
    objectManipulatorOrn = invManipulatorQuat * objectOrn;

    // Remember the current manipulator-space position and orientation
    // as the offsets
    positionOffset = objectManipulatorPos;
    orientationOffset = objectManipulatorOrn;

    // Set the flag indicating the object is attached to the manipulator
    attachedFlag = true;
}

// ------------------------------------------------------------------------
// Detach the object from the manipulator.  This causes the object to stop
// following the manipulator's movements.
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::detachObject()
{
    // Set the attached flag to indicate the object is no longer attached
    attachedFlag = false;
}

// ------------------------------------------------------------------------
// Returns whether or not the object is currently attached to the 
// manipulator
// ------------------------------------------------------------------------
bool vsRelativeObjectMotion::isObjectAttached()
{
    return attachedFlag;
}

// ------------------------------------------------------------------------
// Update function.  If the object is attached, this function computes and
// applies the change in position and orientation that the object must 
// undergo to "keep up" with the manipulator.
// ------------------------------------------------------------------------
void vsRelativeObjectMotion::update()
{
    atMatrix manipulatorMat, invManipulatorMat;
    atQuat manipulatorQuat, invManipulatorQuat;
    atMatrix objectMat;
    atVector objectPos;
    atVector objectManipulatorPos;
    atQuat objectOrn;
    atQuat objectManipulatorOrn;
    atQuat invObjectManipulatorOrn;
    atVector deltaPos;
    atQuat deltaOrn;
    atVector orthoVec;
    atVector deltaAxis;
    double deltaAngle;
    double rotationDot;

    // We don't need to do anything if the object isn't attached or both
    // translation and rotation are locked
    if ((attachedFlag) && ((translationMode != VS_ROM_TRANS_LOCKED) ||
        (rotationMode != VS_ROM_ROT_LOCKED)))
    {
        // Get the global and inverse global transform for the manipulator
        manipulatorMat = manipulatorKin->getComponent()->getGlobalXform();
        invManipulatorMat = manipulatorMat.getInverse();
        manipulatorQuat.setMatrixRotation(manipulatorMat);
        invManipulatorQuat.setMatrixRotation(invManipulatorMat);
    
        // Get the object's position in manipulator coordinates
        objectMat = objectKin->getComponent()->getGlobalXform();
        objectPos = objectMat.getPointXform(atVector(0,0,0));
        objectManipulatorPos = invManipulatorMat.getPointXform(objectPos);

        // Get the object's orientation in manipulator coordinates
        objectOrn = objectKin->getOrientation();
        objectManipulatorOrn = invManipulatorQuat * objectOrn;

        // Calculate the desired change in position
        deltaPos = positionOffset - objectManipulatorPos;

        // Calculate the desired change in orientation
        invObjectManipulatorOrn = objectManipulatorOrn.getInverse();
        deltaOrn = orientationOffset * invObjectManipulatorOrn;

        // Transform these delta values to world coordinates
        deltaPos = manipulatorMat.getVectorXform(deltaPos);
        deltaOrn = manipulatorQuat * deltaOrn * invManipulatorQuat;

        // Handle translation constraints
        if (translationMode == VS_ROM_TRANS_LOCKED)
        {
            // Translation is locked, so no movement
            deltaPos.clear();
        }
        else if (translationMode == VS_ROM_TRANS_LINE)
        {
            // Project the deltaPos vector onto the translation constraint
            // vector
            deltaPos = transVector * deltaPos.getDotProduct(transVector);
        }
        else if (translationMode == VS_ROM_TRANS_PLANE)
        {
            // Project the deltaPos vector onto the constraint plane
            // First get a vector that is orthogonal to both the normal
            // and delta vectors
            orthoVec = deltaPos.getCrossProduct(transVector);

            // Check the magnitude of the cross product to see if it is
            // large enough to consider
            if (orthoVec.getMagnitude() > 1.0e-6)
            {
                // Now, get a vector that is orthogonal to the first orthogonal
                // vector and the normal vector.  This vector will be in the
                // constraint plane.  Also, since the normal vector 
                // (transVector) is unit length, it will be the correct 
                // magnitude for the projection.
                deltaPos = transVector.getCrossProduct(orthoVec);
            }
            else
            {
                // The projection is not of a significant length, constrain
                // the movement to zero
                deltaPos.set(0.0, 0.0, 0.0);
            }
        }

        // Handle rotation constraints
        if (rotationMode == VS_ROM_ROT_LOCKED)
        {
            // Rotation is locked, so no movement
            deltaOrn.set(0.0, 0.0, 0.0, 1.0);
        }
        else if (rotationMode == VS_ROM_ROT_AXIS)
        {
            // Get the axis and angle of the delta rotation
            deltaAxis.setSize(3);
            deltaOrn.getAxisAngleRotation(&deltaAxis[AT_X], &deltaAxis[AT_Y], 
                &deltaAxis[AT_Z], &deltaAngle);

            // Make sure the axis is normalized
            deltaAxis.normalize();

            // Get the dot product of the rotation axis with the 
            // constraint axis
            rotationDot = deltaAxis.getDotProduct(rotAxis);

            // Scale the rotation angle by the dot product
            deltaAngle *= rotationDot;

            // Create a new delta rotation using the constraint axis and
            // the scaled rotation angle
            deltaOrn.setAxisAngleRotation(rotAxis[AT_X], rotAxis[AT_Y],
                rotAxis[AT_Z], deltaAngle);
        }

        // Modify the object's kinematics
        objectKin->modifyPosition(deltaPos);
        objectKin->preModifyOrientation(deltaOrn);
    }
}
