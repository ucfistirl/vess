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
//    VESS Module:  vsMenuObject.h++
//
//    Description:  This is the base class for all of the objects that
//                  the vsMenuSystem visually represents. It derives from
//                  vsObject for its reference counting features.
//
//    Author(s):    Casey Thurston
//
//------------------------------------------------------------------------

#ifndef VS_MENU_OBJECT_HPP
#define VS_MENU_OBJECT_HPP

#include "vsComponent.h++"
#include "vsGlobals.h++"
#include "vsInputButton.h++"
#include "vsKinematics.h++"
#include "vsMenuFrame.h++"
#include "vsObject.h++"

enum VS_SYSTEM_DLL vsMenuSignal
{
    VS_MENU_SIGNAL_IDLE,
    VS_MENU_SIGNAL_ACTIVATE,
    VS_MENU_SIGNAL_INCREASE,
    VS_MENU_SIGNAL_DECREASE
};

class VS_SYSTEM_DLL vsMenuObject : public vsObject
{
private:

    char             *objectName;
    vsInputButton    *inputAccel;
    bool             selectable;
    bool             enabled;

protected:

    vsComponent     *menuComponent;
    vsKinematics    *menuKinematics;

public:

                          vsMenuObject();
                          vsMenuObject(vsComponent *component,
                              vsKinematics *kinematics);
    virtual               ~vsMenuObject();

    virtual const char    *getClassName();

    virtual void          update(vsMenuSignal signal, vsMenuFrame *frame);

    vsComponent           *getComponent();
    vsKinematics          *getKinematics();

    void                  setName(char *name);
    char                  *getName();

    void                  setAccelerator(vsInputButton *accelerator);
    vsInputButton         *getAccelerator();

    void                  setSelectable(bool canSelect);
    bool                  isSelectable();

    void                  setEnabled(bool state);
    bool                  isEnabled();
};

#endif
