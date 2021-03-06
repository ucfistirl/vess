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
//    VESS Module:  vsISTJoystickBox.c++
//
//    Description:  Support for the IST joystick box
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsISTJoystickBox.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Constructor.  Sets up a vsISTJoystickBox on the specified serial port
// ------------------------------------------------------------------------
vsISTJoystickBox::vsISTJoystickBox(int portNumber)
             : vsJoystickBox()
{
    // Initialize variables
    joystick = NULL;
    port = NULL;

    // Determine the platform-dependent serial device
    // name
#ifdef __linux__
    sprintf(portDevice, "/dev/ttyS%d", portNumber - 1);
#endif

#ifdef WIN32
    sprintf(portDevice, "COM%d", portNumber);
#endif

    // Create a 2-axis 2-button joystick in normalized axis mode
    joystick = new vsJoystick(VS_ISTJS_NUM_AXES, VS_ISTJS_NUM_BUTTONS,
                              VS_ISTJS_AXIS_MIN, VS_ISTJS_AXIS_MAX);
    joystick->ref();

    // Open serial port
    port = new vsSerialPort(portDevice, 9600, 8, 'N', 1);
    port->ref();

    // Ping the box to get the first packet ready
    ping();
}

// ------------------------------------------------------------------------
// Constructor.  Sets up a vsISTJoystickBox on the specified serial port
// ------------------------------------------------------------------------
vsISTJoystickBox::vsISTJoystickBox(char *portDev)
             : vsJoystickBox()
{
    // Initialize variables
    joystick = NULL;
    port = NULL;

    // Determine the platform-dependent serial device
    // name
    memset(portDevice, 0, sizeof(portDevice));
    strncpy(portDevice, portDev, sizeof(portDevice-1));

    // Create a 2-axis 2-button joystick in normalized axis mode
    joystick = new vsJoystick(VS_ISTJS_NUM_AXES, VS_ISTJS_NUM_BUTTONS,
                              VS_ISTJS_AXIS_MIN, VS_ISTJS_AXIS_MAX);
    joystick->ref();

    // Open serial port
    port = new vsSerialPort(portDevice, 9600, 8, 'N', 1);
    port->ref();

    // Ping the box to get the first packet ready
    ping();
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsISTJoystickBox::~vsISTJoystickBox(void)
{
    // Close the serial port
    if (port)
        vsObject::unrefDelete(port);

    // Delete the joystick
    if (joystick)
        vsObject::unrefDelete(joystick);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsISTJoystickBox::getClassName()
{
    return "vsISTJoystickBox";
}

// ------------------------------------------------------------------------
// Requests or "pings" the joystick box to send an update
// packet (see the getReport() function for the packet format)
// ------------------------------------------------------------------------
void vsISTJoystickBox::ping(void)
{
    unsigned char buf;
 
    // Send the ping command ('p') to the joystick box
    buf = 'p';
    port->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Sets the parameters to the values corresponding with 
// the packet obtained from the joystick box
// ------------------------------------------------------------------------
void vsISTJoystickBox::getReport(unsigned char* x, unsigned char* y,
                              unsigned char* b1, unsigned char* b2)
{
    int  result;
    char buf[15];

    // Report packet format:
    // 
    // Size   = 11 bytes
    // Format = 'xx yy bb\n\r'
    // 
    // Where:
    //   xx = X axis value in hex (00 to FF)
    //   xx = Y axis value in hex (00 to FF)
    //   bb = Button status in hex (00 to 03)

    // Read the packet
    result = port->readPacket((unsigned char *)buf, 11);
   
    // Check the size of the packet
    if (result != 11)
    {
        printf("vsISTJoystickBox::getReport()  Error reading joystick"
            "(%d of 11 bytes)\n", result);

        // Return default values (axis centered, buttons released)
        if (x != NULL) 
            *x = 128;
        if (y != NULL) 
            *y = 128;
        if (b1 != NULL) 
            *b1 = 0;
        if (b2 != NULL) 
            *b2 = 0;
    }
    else
    {
        // Return the data in the packet.  For each element, we need to
        // terminate the string fragment, then convert the two characters 
        // to a byte.
        if (x != NULL)
        {
            buf[2] = '\0';
            *x = stringToByte(&buf[0]);
        }

        if (y != NULL)
        {
            buf[5] = '\0';
            *y = stringToByte(&buf[3]);
        }

        if (b1 != NULL)
        {
            *b1 = !((buf[7] - '0') & 0x1);
        }

        if (b2 != NULL)
        {
            *b2 = !((buf[7] - '0') & 0x2);
        }   
    }
}

// ------------------------------------------------------------------------
// Converts the parameter hexString (a 2-character string 
// representing a hexadecimal number) to a byte
// ------------------------------------------------------------------------
unsigned char vsISTJoystickBox::stringToByte(char *hexString)
{
    unsigned char nybble[2];
    int           i;

    // Once for each char
    for (i = 0; i < 2; i++)
    {
        if ((hexString[i] >= '0') && (hexString[i] <= '9'))
        {
            // Numeric character
            nybble[i] = hexString[i] - '0';
        }
        else if ((hexString[i] >= 'a') && (hexString[i] <= 'f'))
        {
            // Lower-case a-f
            nybble[i] = hexString[i] - 'a' + 10;
        }
        else if ((hexString[i] >= 'A') && (hexString[i] <= 'F'))
        {
            // Upper-case A-F
            nybble[i] = hexString[i] - 'A' + 10;
        }
        else 
        {
            // Invalid character
            nybble[i] = 0;
        }
    }

    // Compose the byte and return it
    return (unsigned char)((nybble[0] * 16) + nybble[1]);
}

// ------------------------------------------------------------------------
// Returns the number of joysticks connected to this box.  The IST joystick
// box supports only one joystick, so this is always 1.
// ------------------------------------------------------------------------
int vsISTJoystickBox::getNumJoysticks()
{
    return 1;
}

// ------------------------------------------------------------------------
// Returns this box's joystick object
// ------------------------------------------------------------------------
vsJoystick *vsISTJoystickBox::getJoystick()
{
    return joystick;
}

// ------------------------------------------------------------------------
// Returns the specified joystick object
// ------------------------------------------------------------------------
vsJoystick *vsISTJoystickBox::getJoystick(int index)
{
    // Only index 0 is valid, since there can be only one joystick 
    // connected
    if (index == 0)
    {
        return joystick;
    }

    // The index is invalid, so return NULL
    return NULL;
}

// ------------------------------------------------------------------------
// Set the idle position of the axes, usually in the center
// position (minimum position for throttles).  This sets
// the offset member of each vsInputAxis.  The axis
// objects subtract this value when reporting the
// current position of the axis.
// ------------------------------------------------------------------------
void vsISTJoystickBox::setIdlePosition(void)
{
    unsigned char xAxis, yAxis;

    // Get an update from the box
    getReport(&xAxis, &yAxis, NULL, NULL);

    // Set positions of each axis to the values returned
    joystick->getAxis(VS_JS_X_AXIS)->setPosition((double)xAxis);
    joystick->getAxis(VS_JS_Y_AXIS)->setPosition((double)yAxis);

    // Make the new values the idle positions
    joystick->setIdlePosition();
 
    // Request the next packet
    ping();
}

// ------------------------------------------------------------------------
// Update the values for all the axes and buttons
// ------------------------------------------------------------------------
void vsISTJoystickBox::update(void)
{
    unsigned char xAxis, yAxis;
    unsigned char btn1, btn2;

    // Get an update from the box
    getReport(&xAxis, &yAxis, &btn1, &btn2);

    // Update the axis positions
    joystick->getAxis(VS_JS_X_AXIS)->setPosition(xAxis);
    joystick->getAxis(VS_JS_Y_AXIS)->setPosition(yAxis);

    // Update the button states
    if (btn1)
        joystick->getButton(0)->setPressed();
    else
        joystick->getButton(0)->setReleased();

    if (btn2)
        joystick->getButton(1)->setPressed();
    else
        joystick->getButton(1)->setReleased();

    // Request the next packet
    ping();

    // Update all the axes and buttons
    joystick->update();
}
