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
//    VESS Module:  vsISTJoystickBox.h++
//
//    Description:  Support for the IST joystick box
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_IST_JOYSTICK_HPP
#define VS_IST_JOYSTICK_HPP

// This class supports the older 68HC11-based boxes that
// translate 2 axes and 2 buttons, as well as the more
// recent BASIC Stamp II-based boxes that support only the 2
// buttons.  If the BASIC Stamp boxes are used, the axes can 
// be queried, but they will always be reported as centered.

#include "vsJoystick.h++"
#include "vsJoystickBox.h++"
#include "vsSerialPort.h++"

#define VS_ISTJS_NUM_AXES    2
#define VS_ISTJS_NUM_BUTTONS 2
#define VS_ISTJS_AXIS_MIN    0
#define VS_ISTJS_AXIS_MAX    255

class VESS_SYM vsISTJoystickBox : public vsJoystickBox
{
protected:

    // Pointer and device name for the serial port object
    vsSerialPort     *port;
    char             portDevice[20];

    // The joystick
    vsJoystick       *joystick;

    // Hardware communications functions
    void             ping(void);
    void             getReport(unsigned char *x, unsigned char *y,
                               unsigned char *b1, unsigned char *b2);

    // Utility function
    unsigned char    stringToByte(char *string); 

public:

                          vsISTJoystickBox(int portNumber);
    virtual               ~vsISTJoystickBox();

    virtual const char    *getClassName();

    virtual int           getNumJoysticks();
    virtual vsJoystick    *getJoystick();
    virtual vsJoystick    *getJoystick(int index);

    virtual void          setIdlePosition();
    virtual void          update();
};

#endif
