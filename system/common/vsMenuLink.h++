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
//    VESS Module:  vsMenuLink.h++
//
//    Description:  The vsMenuLink is an object whose sole purpose is to
//                  navigate to a different location within the menu tree.
//                  This location is represented by a frame and can be
//                  specified in relative or absolute terms.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_LINK_HPP
#define VS_MENU_LINK_HPP

#include "vsComponent.h++"
#include "vsGlobals.h++"
#include "vsKinematics.h++"
#include "vsMenuFrame.h++"
#include "vsMenuButton.h++"

enum VS_SYSTEM_DLL vsMenuLinkMode
{
    VS_MENU_LINK_MODE_RELATIVE,
    VS_MENU_LINK_MODE_ABSOLUTE
};

class VS_SYSTEM_DLL vsMenuLink : public vsMenuButton
{
private:

    vsMenuFrame       *destFrame;

    vsMenuLinkMode    linkMode;

VS_INTERNAL:

public:

                          vsMenuLink();
                          vsMenuLink(vsMenuObject *object);
                          vsMenuLink(vsComponent *component,
                              vsKinematics *kinematics);
    virtual               ~vsMenuLink();

    virtual const char    *getClassName();

    virtual void          update(vsMenuSignal signal, vsMenuFrame *frame);

    void                  setTarget(vsMenuFrame *frame, vsMenuLinkMode mode);
    vsMenuFrame           *getFrame();
    vsMenuLinkMode        getMode();
};

#endif
