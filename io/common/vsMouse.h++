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
//    VESS Module:  vsMouse.h++
//
//    Description:  Class to handle the state of the mouse
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_MOUSE_HPP
#define VS_MOUSE_HPP

// This class is generally updated by the window system input object
// (e.g: vsWSInput)

#include "vsInputDevice.h++"

#define VS_MOUSE_MAX_AXES    3
#define VS_MOUSE_MAX_BUTTONS 5

class vsMouse : public vsInputDevice
{
protected:

    // Mouse axes
    int              numAxes;
    vsInputAxis      *axis[VS_MOUSE_MAX_AXES];

    // Mouse buttons
    int              numButtons;
    vsInputButton    *button[VS_MOUSE_MAX_BUTTONS];

VS_INTERNAL:

    // Convenience method
    void    moveTo(int xPos, int yPos);

public:

                     vsMouse(int nAxes, int nButtons);
                     vsMouse(int nAxes, int nButtons, int xSize, int ySize);
                     ~vsMouse();

    virtual const char    *getClassName();

    virtual int      getNumAxes();
    virtual int      getNumButtons();

    vsInputAxis      *getAxis(int index);
    vsInputButton    *getButton(int index);
};

#endif
