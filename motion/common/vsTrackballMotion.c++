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
//    VESS Module:  vsTrackballMotion.c++
//
//    Description:  Motion model that translates and rotates a component
//		    with the motion of a trackball (or mouse, joystick,
//		    etc. acting as a trackball)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsTrackballMotion.h++"
#include <stdio.h>
#include "vsMatrix.h++"
#include "vsTimer.h++"

// ------------------------------------------------------------------------
// Constructs a trackball motion model using a mouse and the default button
// configuration
// ------------------------------------------------------------------------
vsTrackballMotion::vsTrackballMotion(vsMouse *mouse, vsKinematics *kin)
                 : vsMotionModel()
{
     // Get the input axes
     horizontal = mouse->getAxis(0);
     vertical = mouse->getAxis(1);

     // Complain if any of the axes are not normalized
     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsTrackballMotion::vsTrackballMotion:  One or more axes "
                "are not normalized!\n");
     }

     // Get the control buttons and kinematics
     transXZButton = mouse->getButton(0);
     transYButton = mouse->getButton(2);
     rotButton = mouse->getButton(1);
     kinematics = kin;

     // Set the motion defaults
     transConst = VS_TBM_DEFAULT_TRANSLATE_CONST;
     rotConst = VS_TBM_DEFAULT_ROTATE_CONST;
}

// ------------------------------------------------------------------------
// Constructs a trackball motion model using a mouse and the given button
// configuration
// ------------------------------------------------------------------------
vsTrackballMotion::vsTrackballMotion(vsMouse *mouse, int xzTransButtonIndex, 
                                     int yTransButtonIndex, int rotButtonIndex,
                                     vsKinematics *kin)
                 : vsMotionModel()
{
     // Get the input axes
     horizontal = mouse->getAxis(0);
     vertical = mouse->getAxis(1);

     // Complain if any of the axes are not normalized
     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsTrackballMotion::vsTrackballMotion:  One or more axes "
                "are not normalized!\n");
     }

     // Get the control buttons and kinematics
     transXZButton = mouse->getButton(xzTransButtonIndex);
     transYButton = mouse->getButton(yTransButtonIndex);
     rotButton = mouse->getButton(rotButtonIndex);
     kinematics = kin;

     // Set the motion defaults
     transConst = VS_TBM_DEFAULT_TRANSLATE_CONST;
     rotConst = VS_TBM_DEFAULT_ROTATE_CONST;
}

// ------------------------------------------------------------------------
// Constructs a trackball motion model using the given axis and button 
// objects
// ------------------------------------------------------------------------
vsTrackballMotion::vsTrackballMotion(vsInputAxis *horizAxis, 
                                     vsInputAxis *vertAxis,
                                     vsInputButton *xzTransBtn,
                                     vsInputButton *yTransBtn,
                                     vsInputButton *rotBtn,
                                     vsKinematics *kin)
                 : vsMotionModel()
{
     // Get the input axes
     horizontal = horizAxis;
     vertical = vertAxis;

     // Complain if any of the axes are not normalized
     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsTrackballMotion::vsTrackballMotion:  One or more axes "
                "are not normalized!\n");
     }

     // Get the control buttons and kinematics
     transXZButton = xzTransBtn;
     transYButton = yTransBtn;
     rotButton = rotBtn;
     kinematics = kin;

     // Set the motion defaults
     transConst = VS_TBM_DEFAULT_TRANSLATE_CONST;
     rotConst = VS_TBM_DEFAULT_ROTATE_CONST;
}

// ------------------------------------------------------------------------
// Destructor for vsTrackballMotion
// ------------------------------------------------------------------------
vsTrackballMotion::~vsTrackballMotion()
{

}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsTrackballMotion::getClassName()
{
    return "vsTrackballMotion";
}

// ------------------------------------------------------------------------
// Sets the translation constant.  The translation constant specifies how 
// far the geometry moves per unit of input.  
// ------------------------------------------------------------------------
void vsTrackballMotion::setTranslationConstant(double newConst)
{
    transConst = newConst;
}

// ------------------------------------------------------------------------
// Returns the translation constant.
// ------------------------------------------------------------------------
double vsTrackballMotion::getTranslationConstant()
{
    return transConst;
}

// ------------------------------------------------------------------------
// Sets the rotation constant.  The rotation constant specifies how far the 
// geometry rotates per unit of input.  
// ------------------------------------------------------------------------
void vsTrackballMotion::setRotationConstant(double newConst)
{
    rotConst = newConst;
}

// ------------------------------------------------------------------------
// Returns the rotation constant.
// ------------------------------------------------------------------------
double vsTrackballMotion::getRotationConstant()
{
    return rotConst;
}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
void vsTrackballMotion::update()
{
    double    interval;
    vsVector  zero;
    vsVector  origin;
    vsVector  currentPos;
    vsQuat    currentRot, invRot;
    vsQuat    coordQuat;
    double    dHoriz, dVert;
    vsQuat    rot1, rot2, totalRot;
    vsVector  rotAxis;
    double    rotAngle;
    vsVector  dPos;

    // Get the interval of elapsed time
    interval = vsTimer::getSystemTimer()->getInterval();

    // Make sure interval is valid
    if (interval <= 0.0)
        return;

    // Get current values
    currentRot = kinematics->getOrientation();
    invRot = currentRot;
    invRot.conjugate();

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

    // If any button is pressed, set all velocities to zero.  This is done
    // in case inertia is enabled on the kinematics.  Otherwise it would
    // be difficult to control the constant accumulation of velocities.
    if (((transXZButton != NULL) && (transXZButton->isPressed())) ||
        ((transYButton != NULL) && (transYButton->isPressed())) ||
        ((rotButton != NULL) && (rotButton->isPressed())))
    {
        zero.set(0.0, 0.0, 0.0);
        kinematics->setVelocity(zero);
        kinematics->setAngularVelocity(zero, 0.0);
    }

    // Next, calculate the amount of motion based on which button(s) 
    // is/are pressed

    // Check for the XZ-translation button
    if ((transXZButton != NULL) && (transXZButton->isPressed()))
    {
        // Translate in the XZ (screen) plane.  Set the delta position
        // vector based on the amount of horizontal and vertical axis
        // movement and the current translation constant
        dPos.set(dHoriz * transConst, 0.0, 
            -dVert * transConst);
        
        // Convert the change in position to velocity by dividing by time.
        // Use this to set the kinematics' linear velocity.
        kinematics->setVelocity(dPos.getScaled(1/interval));
    }
    else if ((rotButton != NULL) && (rotButton->isPressed()))
    {
        // The rotation button is pressed, check if the Y-translation
        // button is also pressed.  If so, we want to rotate around the
        // Y axis instead of the X and Z axes.
        if ((transYButton != NULL) && (transYButton->isPressed()))
        {
            // Rotate about Y.  Create two quaternions, one handling Y-
            // axis rotation based on horizontal axis movement, and the
            // other also handling Y-axis rotation, but based on vertical
            // axis movement.  Scale the axis movement by the current
            // rotation constant.
            rot1.setAxisAngleRotation(0, 1, 0, 
                dHoriz * rotConst);
            rot2.setAxisAngleRotation(0, 1, 0,
                -dVert * rotConst);

            // Combine the quaternions to get an overall rotation
            totalRot = rot2 * rot1;

            // Transform the scene's local coordinate system to the viewpoint
            // coordinate system
            coordQuat = invRot * totalRot * currentRot;

            // Get the rotation axis and amount of rotation
            coordQuat.getAxisAngleRotation(&rotAxis[VS_X], &rotAxis[VS_Y],
                &rotAxis[VS_Z], &rotAngle);

            // Convert the rotation to angular velocity by dividing by
            // time.  Use this to set the kinematics' angular velocity.
            kinematics->setAngularVelocity(rotAxis, rotAngle / interval);
        }
        else
        {
            // The Y-translate button is not pressed, so rotate about X 
            // and/or Z.  Set up two quaternions, one to handle Z-axis
            // rotation (using the horizontal input axis movement), and the
            // other to handle X-axis rotation (using the vertical input
            // axis movement).  Scale the amount of rotation by the current
            // rotation constant.
            rot1.setAxisAngleRotation(0, 0, 1, 
                dHoriz * rotConst);
            rot2.setAxisAngleRotation(1, 0, 0, 
                dVert * rotConst);

            // Combine the quaternions to get an overall rotation
            totalRot = rot2 * rot1;

            // Transform the scene's local coordinate system to the viewpoint
            // coordinate system
            coordQuat = invRot * totalRot * currentRot;

            // Get the rotation axis and the amount of rotation
            coordQuat.getAxisAngleRotation(&rotAxis[VS_X], &rotAxis[VS_Y],
                &rotAxis[VS_Z], &rotAngle);

            // Convert the rotation to angular velocity by dividing by time.
            // Use this to set the kinematics' angular velocity.
            kinematics->setAngularVelocity(rotAxis, rotAngle / interval);
        }
    }
    else if ((transYButton != NULL) && (transYButton->isPressed()))
    {
        // The Y-translate (zoom) button is being pressed, so translate 
        // in the Y direction.  Scale the vertical axis movement by
        // the current translation constant to get the amount of Y-axis
        // translation.
        dPos.set(0.0, -dVert * transConst, 0.0);

        // Convert the change in position to velocity by dividing by time.
        // Use this to set the kinematics' linear velocity.
        kinematics->setVelocity(dPos.getScaled(1/interval));
    }
}
