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
//    VESS Module:  vsUnwinder.h++
//
//    Description:  Support for the Unwinder joystick box
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_UNWINDER_JOYSTICK_HPP
#define VS_UNWINDER_JOYSTICK_HPP

#include "vsJoystickBox.h++"
#include "vsSerialPort.h++"

#define VS_UW_MAX_JOYSTICKS 2

#define VS_UW_NUM_AXES      4
#define VS_UW_NUM_BUTTONS   4
#define VS_UW_AXIS_MIN      0
#define VS_UW_AXIS_MAX      4095

#define VS_UW_PACKET_SIZE   19

typedef struct
{
    unsigned char xMSB;
    unsigned char yMSB;
    unsigned char zMSB;
    unsigned char tMSB;
    unsigned char xyLSB;
    unsigned char ztLSB;
    unsigned char buttons;
    unsigned char hat;
} vsUnwinderData;

typedef struct 
{
    unsigned char                 status;
    unsigned char                 mode;
    vsUnwinderData                joyData[2];
    unsigned char                 checkSum;
} vsUnwinderPacket;

class vsUnwinder : public vsJoystickBox
{
protected:

    // Pointer and device name for the serial port object
    vsSerialPort    *port;
    char            portDevice[20];

    // The joysticks
    int             numJoysticks;
    vsJoystick      *joystick[VS_UW_MAX_JOYSTICKS];

    // Utility functions
    void            ping(void);
    int             isCheckSumOK(vsUnwinderPacket *packet);
    void            getReport(vsUnwinderPacket *packet);

public:

                          vsUnwinder(int portNumber, 
                                     int joy1, int joy2);
    virtual               ~vsUnwinder();

    virtual int           getNumJoysticks();
    virtual vsJoystick    *getJoystick();
    virtual vsJoystick    *getJoystick(int index);

    virtual int           isConnected(int index);

    virtual void          setIdlePosition();
    virtual void          update();
};

#endif
