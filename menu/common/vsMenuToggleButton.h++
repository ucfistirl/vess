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
//    VESS Module:  vsMenuToggleButton.h++
//
//    Description:  This sub-class of the vsMenuButton toggles its press
//                  state when activated.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_TOGGLE_BUTTON_HPP
#define VS_MENU_TOGGLE_BUTTON_HPP

#include "vsComponent.h++"
#include "vsGlobals.h++"
#include "vsKinematics.h++"
#include "vsMenuButton.h++"

class VESS_SYM vsMenuToggleButton : public vsMenuButton
{
public:

                            vsMenuToggleButton();
                            vsMenuToggleButton(vsMenuObject *object);
                            vsMenuToggleButton(vsComponent *component,
                                vsKinematics *kinematics);
    virtual                 ~vsMenuToggleButton();

    virtual const char      *getClassName();

    virtual void            update(vsMenuSignal signal, vsMenuFrame *frame);
};

#endif
