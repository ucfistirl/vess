#include "vsTrackballMotion.h++"

// ------------------------------------------------------------------------
// Constructs a trackball motion model using a mouse and the default button
// configuration
// ------------------------------------------------------------------------
vsTrackballMotion::vsTrackballMotion(vsMouse *mouse)
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
}

// ------------------------------------------------------------------------
// Constructs a trackball motion model using a mouse and the given button
// configuration
// ------------------------------------------------------------------------
vsTrackballMotion::vsTrackballMotion(vsMouse *mouse, int xzTransButtonIndex, 
                                     int yTransButtonIndex, int rotButtonIndex)
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
}

// ------------------------------------------------------------------------
// Constructs a trackball motion model using the given axis and button 
// objects
// ------------------------------------------------------------------------
vsTrackballMotion::vsTrackballMotion(vsInputAxis *horizAxis, 
                                     vsInputAxis *vertAxis,
                                     vsInputButton *xzTransBtn,
                                     vsInputButton *yTransBtn,
                                     vsInputButton *rotBtn)
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
}

// ------------------------------------------------------------------------
// Destructor for vsTrackballMotion
// ------------------------------------------------------------------------
vsTrackballMotion::~vsTrackballMotion()
{

}

// ------------------------------------------------------------------------
// Updates the motion model
// ------------------------------------------------------------------------
vsMatrix vsTrackballMotion::update()
{
    vsVector  origin;
    vsVector  currentPos;
    double    dHoriz, dVert;
    vsQuat    rot1, rot2;
    vsMatrix  motion;

    // Initialize
    motion.setIdentity();

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

    // Next, calculate the amount of motion based on which button(s) 
    // is/are pressed
    if ((transXZButton != NULL) && (transXZButton->isPressed()))
    {
        // Translate in the XZ (screen) plane
        motion.setTranslation(dHoriz * VS_TBM_TRANSLATE_CONST, 0.0, 
            -dVert * VS_TBM_TRANSLATE_CONST);
    }
    else if ((rotButton != NULL) && (rotButton->isPressed()))
    {
        if ((transYButton != NULL) && (transYButton->isPressed()))
        {
            // Rotate about Y
            rot1.setAxisAngleRotation(0, 1, 0, 
                dHoriz * VS_TBM_ROTATE_CONST);
            rot2.setAxisAngleRotation(0, 1, 0,
                -dVert * VS_TBM_ROTATE_CONST);

            motion.setQuatRotation(rot2 * rot1);
        }
        else
        {
            // Rotate about X and/or Z
            rot1.setAxisAngleRotation(0, 0, 1, 
                dHoriz * VS_TBM_ROTATE_CONST);
            rot2.setAxisAngleRotation(1, 0, 0, 
                dVert * VS_TBM_ROTATE_CONST);

            motion.setQuatRotation(rot2 * rot1);
        }
    }
    else if ((transYButton != NULL) && (transYButton->isPressed()))
    {
        // Translate in the Y direction
        motion.setTranslation(0.0, -dVert * VS_TBM_TRANSLATE_CONST, 0.0);
    }

    return motion;
}
