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
//    VESS Module:  vsWSSpaceball.h++
//
//    Description:  A class for handling spaceball input from the window
//                  system.  This implementation is for X Window systems
//                  using the X11 input extension.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_WS_SPACEBALL_HPP
#define VS_WS_SPACEBALL_HPP

#include "vsIOSystem.h++"
#include "vsSpaceball.h++"
#include "vsWindowSystem.h++"
#include <X11/extensions/XI.h>
#include <X11/extensions/XInput.h>
#undef index

#define VS_SB_MAX_BUTTONS 9

class vsWSSpaceball : public vsIOSystem
{
protected:

    // Handles to the window system display and window
    Display          *display;
    Window           window;

    // The spaceball input device
    vsSpaceball      *spaceball;

    // The spaceball X device
    XDevice          *sbDevice;

    // Extension event types
    int              sbMotion;
    int              sbButtonPress;
    int              sbButtonRelease;

    bool             initializeSpaceball();

public:

                          vsWSSpaceball(vsWindowSystem *ws, int nButtons);
    virtual               ~vsWSSpaceball(void);

    virtual const char    *getClassName();

    vsSpaceball           *getSpaceball(void);

    virtual void          update(void);
};

#endif
