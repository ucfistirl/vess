#ifndef VS_UNWINDER_JOYSTICK_HPP
#define VS_UNWINDER_JOYSTICK_HPP

// Support for the Unwinder joystick box
//
// This class supports the Technology Playgroup Unwinder joystick box.
// It supports up to four axes, four buttons on one or two joysticks.
//
// NOTE:  This class does NOT support the Unwinder's MIDI features or
//        digital joystick mode


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
