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
//    VESS Module:  vsMenuSwitchButton.h++
//
//    Description:  This sub-class of the vsMenuButton moves through a
//                  series of graphically different states when activated.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_SWITCH_BUTTON_HPP
#define VS_MENU_SWITCH_BUTTON_HPP

#include "vsComponent.h++"
#include "vsGlobals.h++"
#include "vsKinematics.h++"
#include "vsMenuButton.h++"
#include "vsSwitchAttribute.h++"

class VS_SYSTEM_DLL vsMenuSwitchButton : public vsMenuButton
{
private:

    int                  switchState;
    vsSwitchAttribute    *switchAttr;

public:

                            vsMenuSwitchButton();
    virtual                 ~vsMenuSwitchButton();

    virtual const char      *getClassName();

    void                    addChild(vsComponent *newChild);

    void                    setSwitchState(int state);
    int                     getSwitchState();

    virtual void            update(vsMenuSignal signal, vsMenuFrame *frame);
};

#endif
