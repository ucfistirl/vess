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
//    VESS Module:  vsRelativeObjectMotion.h++
//
//    Description:  A class to allow any kinematics (the object) to move
//                  based on the movements of a second kinematics (the
//                  manipulator).  Simple positional and rotational
//                  constraints are provided.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_RELATIVE_OBJECT_MOTION
#define VS_RELATIVE_OBJECT_MOTION

#include "vsMotionModel.h++"
#include "vsKinematics.h++"

enum
{
    VS_ROM_TRANS_LOCKED,
    VS_ROM_TRANS_LINE,
    VS_ROM_TRANS_PLANE,
    VS_ROM_TRANS_FREE
};

enum
{
    VS_ROM_ROT_LOCKED,
    VS_ROM_ROT_AXIS,
    VS_ROM_ROT_FREE
};

class VS_MOTION_DLL vsRelativeObjectMotion : public vsMotionModel
{
protected:

    vsKinematics    *objectKin;
    vsKinematics    *manipulatorKin;

    int             translationMode;
    int             rotationMode;

    vsVector        transVector;
    vsVector        rotAxis;

    bool            attachedFlag;

    vsVector        positionOffset;
    vsQuat          orientationOffset;

public:

                    vsRelativeObjectMotion(vsKinematics *objKin,
                                           vsKinematics *manipKin);
                    ~vsRelativeObjectMotion(); 

    const char *    getClassName();

    void            lockTranslation();
    void            constrainTranslationToLine(vsVector axis);
    void            constrainTranslationToPlane(vsVector normal);
    void            freeTranslation();

    void            lockRotation();
    void            constrainRotationToAxis(vsVector axis);
    void            freeRotation();

    void            attachObject();
    void            detachObject();
    bool            isObjectAttached();

    virtual void    update();
};

#endif
