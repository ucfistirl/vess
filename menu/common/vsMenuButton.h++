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
//    VESS Module:  vsMenuButton.h++
//
//    Description:  The vsMenuButton is a clickable object. It has a state
//                  describing whether it is currently activated and may
//                  be set to un-press when idling or not.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_BUTTON_HPP
#define VS_MENU_BUTTON_HPP

#include "vsComponent.h++"
#include "vsGlobals.h++"
#include "vsKinematics.h++"
#include "vsMenuObject.h++"

class VESS_SYM vsMenuButton : public vsMenuObject
{
protected:

    bool              idleReverts;
    bool              idleState;

    bool              canRepeat;
    bool              previousState;

    bool              pressedState;

public:

                            vsMenuButton();
                            vsMenuButton(vsMenuObject *object);
                            vsMenuButton(vsComponent *component,
                                vsKinematics *kinematics);
    virtual                 ~vsMenuButton();

    virtual const char      *getClassName();

    virtual void            update(vsMenuSignal signal, vsMenuFrame *frame);

    void                    setRepeatable(bool repeat);
    bool                    isRepeatable();

    void                    setIdleReversion(bool reverts, bool state);
    bool                    revertsOnIdle();
    bool                    getRevertState();

    void                    setState(bool pressed);
    bool                    isPressed();
};

#endif
