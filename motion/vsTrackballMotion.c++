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
#include "vsSystem.h++"

// ------------------------------------------------------------------------
// Constructs a trackball motion model using a mouse and the default button
// configuration
// ------------------------------------------------------------------------
vsTrackballMotion::vsTrackballMotion(vsMouse *mouse, vsKinematics *kin)
                 : vsMotionModel()
{
     horizontal = mouse->getAxis(0);
     vertical = mouse->getAxis(1);

     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsTrackballMotion::vsTrackballMotion:  One or more axes "
                "are not normalized!\n");
     }

     transXZButton = mouse->getButton(0);
     transYButton = mouse->getButton(2);
     rotButton = mouse->getButton(1);
     kinematics = kin;

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
     horizontal = mouse->getAxis(0);
     vertical = mouse->getAxis(1);

     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsTrackballMotion::vsTrackballMotion:  One or more axes "
                "are not normalized!\n");
     }

     transXZButton = mouse->getButton(xzTransButtonIndex);
     transYButton = mouse->getButton(yTransButtonIndex);
     rotButton = mouse->getButton(rotButtonIndex);
     kinematics = kin;

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
     horizontal = horizAxis;
     vertical = vertAxis;

     if (((horizontal != NULL) && (!horizontal->isNormalized())) ||
         ((vertical != NULL) && (!vertical->isNormalized())))
     {
         printf("vsTrackballMotion::vsTrackballMotion:  One or more axes "
                "are not normalized!\n");
     }

     transXZButton = xzTransBtn;
     transYButton = yTransBtn;
     rotButton = rotBtn;
     kinematics = kin;

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
    interval = vsSystem::systemObject->getFrameTime();

    // Make sure interval is valid
    if (interval <= 0.0)
        return;

    // Get current values
    currentRot = kinematics->getOrientation();
    invRot = currentRot;
    invRot.conjugate();

    // Next, get the amount of axis movement
    dHoriz = 0.0;
    dVert = 0.0;

    if (horizontal != NULL)
    {
        dHoriz = horizontal->getPosition() - lastHorizontal;
        lastHorizontal = horizontal->getPosition();
    }
    if (vertical != NULL)
    {
        dVert = vertical->getPosition() - lastVertical;
        lastVertical = vertical->getPosition();
    }

    // If any button is pressed, set all velocities to zero
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
    if ((transXZButton != NULL) && (transXZButton->isPressed()))
    {
        // Translate in the XZ (screen) plane
        dPos.set(dHoriz * transConst, 0.0, 
            -dVert * transConst);
        
        kinematics->setVelocity(dPos.getScaled(1/interval));
    }
    else if ((rotButton != NULL) && (rotButton->isPressed()))
    {
        if ((transYButton != NULL) && (transYButton->isPressed()))
        {
            // Rotate about Y
            rot1.setAxisAngleRotation(0, 1, 0, 
                dHoriz * rotConst);
            rot2.setAxisAngleRotation(0, 1, 0,
                -dVert * rotConst);

            // The total rotation
            totalRot = rot2 * rot1;

            // Transform the scene's local coordinate system to the viewpoint
            // coordinate system
            coordQuat = invRot * totalRot * currentRot;

            coordQuat.getAxisAngleRotation(&rotAxis[VS_X], &rotAxis[VS_Y],
                &rotAxis[VS_Z], &rotAngle);

            // Divide the angle measurement by time to get a velocity
            kinematics->setAngularVelocity(rotAxis, rotAngle / interval);
        }
        else
        {
            // Rotate about X and/or Z
            rot1.setAxisAngleRotation(0, 0, 1, 
                dHoriz * rotConst);
            rot2.setAxisAngleRotation(1, 0, 0, 
                dVert * rotConst);

            // The total rotation
            totalRot = rot2 * rot1;

            // Transform the scene's local coordinate system to the viewpoint
            // coordinate system
            coordQuat = invRot * totalRot * currentRot;

            coordQuat.getAxisAngleRotation(&rotAxis[VS_X], &rotAxis[VS_Y],
                &rotAxis[VS_Z], &rotAngle);

            // Divide the angle measurement by time to get a velocity
            kinematics->setAngularVelocity(rotAxis, rotAngle / interval);
        }
    }
    else if ((transYButton != NULL) && (transYButton->isPressed()))
    {
        // Translate in the Y direction
        dPos.set(0.0, -dVert * transConst, 0.0);

        kinematics->setVelocity(dPos.getScaled(1/interval));
    }
}
