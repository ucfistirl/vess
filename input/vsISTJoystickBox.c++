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

#ifdef IRIX
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef IRIX64
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef __linux__
    sprintf(portDevice, "/dev/ttyS%d", portNumber - 1);
#endif

    // Create a 2-axis 2-button joystick in normalized axis mode
    joystick = new vsJoystick(VS_ISTJS_NUM_AXES, VS_ISTJS_NUM_BUTTONS,
                              VS_ISTJS_AXIS_MIN, VS_ISTJS_AXIS_MAX);

    // Open serial port
    port = new vsSerialPort(portDevice, 9600, 8, 'N', 1);

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
        delete port;
}

// ------------------------------------------------------------------------
// Requests or "pings" the joystick box to send an update
// packet (see the getReport() function for the packet format)
// ------------------------------------------------------------------------
void vsISTJoystickBox::ping(void)
{
    unsigned char buf;
 

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

    result = port->readPacket((unsigned char *)buf, 11);
   
    if (result != 11)
    {
        printf("vsISTJoystickBox::getReport()  Error reading joystick"
            "(%d of 11 bytes)\n", result);

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

    for (i = 0; i < 2; i++)
    {
        if ((hexString[i] >= '0') && (hexString[i] <= '9'))
            nybble[i] = hexString[i] - '0';
        else if ((hexString[i] >= 'a') && (hexString[i] <= 'f'))
            nybble[i] = hexString[i] - 'a' + 10;
        else if ((hexString[i] >= 'A') && (hexString[i] <= 'F'))
            nybble[i] = hexString[i] - 'A' + 10;
        else 
            nybble[i] = 0;
    }

    return (unsigned char)((nybble[0] * 16) + nybble[1]);
}

// ------------------------------------------------------------------------
// Returns the number of joysticks connected to this box (always 1)
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
// Returns the specified joystick object (only 0 accepted)
// ------------------------------------------------------------------------
vsJoystick *vsISTJoystickBox::getJoystick(int index)
{
    if (index == 0)
    {
        return joystick;
    }

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
}
