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
//    VESS Module:  vsUnwinder.c++
//
//    Description:  Support for the Unwinder joystick box
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsUnwinder.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Constructor.  Sets up a vsUnwinder on the specified serial port
// ------------------------------------------------------------------------
vsUnwinder::vsUnwinder(int portNumber, int joy1, int joy2)
          : vsJoystickBox()
{
    int           i;
    unsigned char buf;

    // Initialize variables
    numJoysticks = 0;
    for (i = 0; i < VS_UW_MAX_JOYSTICKS; i++)
    {
        joystick[i] = NULL;
    }

    // Construct joysticks
    if (joy1) 
    {
        numJoysticks++;
        joystick[0] = 
            new vsJoystick(VS_UW_NUM_AXES, VS_UW_NUM_BUTTONS,
                           VS_UW_AXIS_MIN, VS_UW_AXIS_MAX);
    }
    if (joy2) 
    {
        numJoysticks++;
        joystick[1] = 
            new vsJoystick(VS_UW_NUM_AXES, VS_UW_NUM_BUTTONS,
                           VS_UW_AXIS_MIN, VS_UW_AXIS_MAX);
    }


    // Determine the serial device name

#ifdef IRIX
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef IRIX64
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef __linux__
    sprintf(portDevice, "/dev/ttyS%d", portNumber - 1);
#endif

    // Open serial port
    port = new vsSerialPort(portDevice, 9600, 8, 'N', 1);

    // Set Unwinder to 38400 baud
    buf = '7';
    port->writePacket(&buf, 1);
    usleep(20000);
    
    // Adjust serial port to match the new baud rate
    port->setBaudRate(38400);

    // Set to polled mode, and normal binary output
    buf = 'p';
    port->writePacket(&buf, 1);
    usleep(20000);
    buf = 'X';
    port->writePacket(&buf, 1);
    usleep(20000);
    buf = 'n';
    port->writePacket(&buf, 1);
    usleep(20000);

    printf("vsUnwinder::vsUnwinder: Unwinder created on port %s\n", portDevice);
    printf("vsUnwinder::vsUnwinder:   with %d joystick(s)\n", numJoysticks);

    // Ping for the first update packet
    ping();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsUnwinder::~vsUnwinder(void)
{
    unsigned char buf;

    if (port)
    {
        // Reset baud to 9600
        buf = '5';
        port->writePacket(&buf, 1);

        // Close serial port
        if (port)
            delete port;
    }
}

// ------------------------------------------------------------------------
// Requests or "pings" the joystick box to send an update packet.  The 
// Unwinder takes either a '1', '2', or '3', depending on which joystick(s)
// should be updated ('3' means both)
// ------------------------------------------------------------------------
void vsUnwinder::ping(void)
{
    unsigned char buf;
    int pingNumber;
 
    // Figure out which joystick(s) to ping for
    pingNumber = 0;
    if (isConnected(0))
        pingNumber += 1;
    if (isConnected(1))
        pingNumber += 2;

    // Convert the ping number and send it
    buf = pingNumber + '0';
    port->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Calculate a checksum from an Unwinder data packet and compares it with
// the checkSum value reported by the Unwinder
// ------------------------------------------------------------------------
int vsUnwinder::isCheckSumOK(vsUnwinderPacket *packet)
{
    unsigned char sum;

    sum = 0;

    sum += packet->status;
    sum += packet->mode;

    // If data reported for Joystick 0, add those bytes
    if (packet->status & 0x40)
    {
        sum += packet->joyData[0].xMSB;
        sum += packet->joyData[0].yMSB;
        sum += packet->joyData[0].zMSB;
        sum += packet->joyData[0].tMSB;
        sum += packet->joyData[0].xyLSB;
        sum += packet->joyData[0].ztLSB;
        sum += packet->joyData[0].buttons;
        sum += packet->joyData[0].hat;
    }
    // If data reported for Joystick 1, add those bytes
    if (packet->status & 0x80)
    {
        sum += packet->joyData[1].xMSB;
        sum += packet->joyData[1].yMSB;
        sum += packet->joyData[1].zMSB;
        sum += packet->joyData[1].tMSB;
        sum += packet->joyData[1].xyLSB;
        sum += packet->joyData[1].ztLSB;
        sum += packet->joyData[1].buttons;
        sum += packet->joyData[1].hat;
    }

#ifdef VS_UW_DEBUG
    printf("vsUnwinder::isCheckSumOK: "
        "read: %02X calculated: %02X\n", sum, packet->checkSum);
#endif

    // Check for equality
    if (sum == packet->checkSum)
        return VS_TRUE;
    else
        return VS_FALSE;
}

// ------------------------------------------------------------------------
// Receives a packet from the Unwinder
// ------------------------------------------------------------------------
void vsUnwinder::getReport(vsUnwinderPacket *packet)
{
    int           result;
    int           numBytes;
    int           error;

    // Read status and mode bytes
    result = port->readPacket(&(packet->status), 2);
    
    if (result == 2)
    {

        numBytes = 2; 
        error = VS_FALSE;

#ifdef VS_UW_DEBUG
        printf("vsUnwinder::getReport: "
            "Status byte is %02X\n", packet->status);
#endif

        // Check the status byte to see if Joystick 0 data is present
        if (packet->status & 0x40)
        {
            // Read the bytes for Joystick 0
            result = port->readPacket(&(packet->joyData[0].xMSB), 8);

#ifdef VS_UW_DEBUG
            printf("vsUnwinder::getReport: "
                "Joystick 0 bytes: %02X%02X%02X%02X%02X%02X%02X%02X\n",
                packet->joyData[0].xMSB,
                packet->joyData[0].yMSB,
                packet->joyData[0].zMSB,
                packet->joyData[0].tMSB,
                packet->joyData[0].xyLSB,
                packet->joyData[0].ztLSB,
                packet->joyData[0].buttons,
                packet->joyData[0].hat);
#endif

            // Check the error bit for Joystick 0
            if (packet->status & 0x20)
                error = VS_TRUE;

            // Check for serial errors
            if (result != 8)
                error = VS_TRUE;

            numBytes += 8;
        }
        else 
        {
            if (isConnected(0))
                error = VS_TRUE;
        }

        if (error)
        {
            printf("vsUnwinder::getReport: "
                "Error reading data for Joystick 1\n");
        }

        error = VS_FALSE;

        // Check the status byte to see if Joystick 1 data is present
        if (packet->status & 0x80)
        {
            // Read the bytes for Joystick 1
            result = port->readPacket(&(packet->joyData[1].xMSB), 8);

#ifdef VS_UW_DEBUG
            printf("vsUnwinder::getReport: "
                "Joystick 1 bytes: %02X%02X%02X%02X%02X%02X%02X%02X\n",
                packet->joyData[1].xMSB,
                packet->joyData[1].yMSB,
                packet->joyData[1].zMSB,
                packet->joyData[1].tMSB,
                packet->joyData[1].xyLSB,
                packet->joyData[1].ztLSB,
                packet->joyData[1].buttons,
                packet->joyData[1].hat);
#endif

            // Check the error bit for Joystick 1
            if (packet->status & 0x10)
                error = VS_TRUE;

            // Check for serial errors
            if (result != 8)
                error = VS_TRUE;

            numBytes += 8;
        }
        else
        {
            if (isConnected(1))
                error = VS_TRUE;
        }

        if (error) 
        {
            printf("vsUnwinder::getReport: "
                "Error reading data for Joystick 2\n");
        }

        // Read the checksum byte
        result = port->readPacket(&(packet->checkSum), 1);
        numBytes += 1;

        // Compute the sum of the bytes and warn if it doesn't match
        // the Unwinder's checksum
        if (!isCheckSumOK(packet))
        {
            printf("vsUnwinder::getReport: WARNING -- Bad checksum\n");
        }
    }
    else
    {
        printf("vsUnwinder::getReport:  No response from Unwinder\n");
    }
}

// ------------------------------------------------------------------------
// Return the number of joysticks connected
// ------------------------------------------------------------------------
int vsUnwinder::getNumJoysticks()
{
    return numJoysticks;
}

// ------------------------------------------------------------------------
// Return the first available joystick in the joystick array
// ------------------------------------------------------------------------
vsJoystick *vsUnwinder::getJoystick()
{
    int i;

    for (i = 0; i < VS_UW_MAX_JOYSTICKS; i++)
    {
        if (isConnected(i))
        {
            return joystick[i];
        }
    }

    return NULL;
}

// ------------------------------------------------------------------------
// Return the specified joystick if it exists
// ------------------------------------------------------------------------
vsJoystick *vsUnwinder::getJoystick(int index)
{
    if (isConnected(index))
    {
        return joystick[index];
    }

    return NULL;
}

// ------------------------------------------------------------------------
// Return VS_TRUE if the given joystick is connected to the joystick box
// ------------------------------------------------------------------------
int vsUnwinder::isConnected(int index)
{
    if (joystick[index] != NULL)
        return VS_TRUE;
    else
        return VS_FALSE;
}

// ------------------------------------------------------------------------
// Set the idle position of the axes, usually in the center position.  This 
// sets the offset member of each vsInputAxis.  The axis objects subtract 
// this value when reporting the current position of the axis.
// ------------------------------------------------------------------------
void vsUnwinder::setIdlePosition(void)
{
    vsUnwinderPacket packet;
    int              i;
    int              axisVal;

    // Get an update packet from the Unwinder
    getReport(&packet);

    // Set the idle positions for each axis
    for (i = 0; i < VS_UW_MAX_JOYSTICKS; i++)
    {
        if (isConnected(i))
        {
            axisVal = packet.joyData[i].xMSB << 4;
            axisVal |= (packet.joyData[i].xyLSB & 0xF0) >> 4;
            joystick[i]->getAxis(0)->setIdlePosition((double)axisVal);
    
            axisVal = packet.joyData[i].yMSB << 4;
            axisVal |= packet.joyData[i].xyLSB & 0x0F;
            joystick[i]->getAxis(1)->setIdlePosition((double)axisVal);
    
            axisVal = packet.joyData[i].zMSB << 4;
            axisVal |= (packet.joyData[i].ztLSB & 0xF0) >> 4;
            joystick[i]->getAxis(2)->setIdlePosition((double)axisVal);
    
            axisVal = packet.joyData[i].tMSB << 4;
            axisVal |= packet.joyData[i].ztLSB & 0x0F;
            joystick[i]->getAxis(3)->setIdlePosition((double)axisVal);
        }
    }

    // Ping for the next update
    ping();
}

// ------------------------------------------------------------------------
// Update the values for all the axes and buttons
// ------------------------------------------------------------------------
void vsUnwinder::update(void)
{
    int i;
    int axisVal;

    vsUnwinderPacket packet;

    // Get an update packet from the Unwinder
    getReport(&packet);

    // Set each axis's position and button's state
    for (i = 0; i < VS_UW_MAX_JOYSTICKS; i++)
    {
        if (isConnected(i))
        {
            axisVal = packet.joyData[i].xMSB << 4;
            axisVal |= (packet.joyData[i].xyLSB & 0xF0) >> 4;
            joystick[i]->getAxis(0)->setPosition((double)axisVal);
    
            axisVal = packet.joyData[i].yMSB << 4;
            axisVal |= packet.joyData[i].xyLSB & 0x0F;
            joystick[i]->getAxis(1)->setPosition((double)axisVal);
    
            axisVal = packet.joyData[i].zMSB << 4;
            axisVal |= (packet.joyData[i].ztLSB & 0xF0) >> 4;
            joystick[i]->getAxis(2)->setPosition((double)axisVal);
    
            axisVal = packet.joyData[i].tMSB << 4;
            axisVal |= packet.joyData[i].ztLSB & 0x0F;
            joystick[i]->getAxis(3)->setPosition((double)axisVal);
   
            if (packet.joyData[i].buttons & 0x01)
                joystick[i]->getButton(0)->setPressed();
            else
                joystick[i]->getButton(0)->setReleased();
    
            if (packet.joyData[i].buttons & 0x02)
                joystick[i]->getButton(1)->setPressed();
            else
                joystick[i]->getButton(1)->setReleased();
    
            if (packet.joyData[i].buttons & 0x04)
                joystick[i]->getButton(2)->setPressed();
            else
                joystick[i]->getButton(2)->setReleased();
    
            if (packet.joyData[i].buttons & 0x08)
                (joystick[i])->getButton(3)->setPressed();
            else
                (joystick[i])->getButton(3)->setReleased();
        }
    }

    ping();
}
