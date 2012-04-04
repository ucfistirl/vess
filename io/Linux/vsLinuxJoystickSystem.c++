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
//    VESS Module:  vsLinuxJoystickSystem.h++
//
//    Description:  Support for Linux-based joysticks. See
//                  joystick-api.txt in the Linux kernel documentation for
//                  implementation details.
//
//    Author(s):    Carlos Rosas-Anderson
//
//------------------------------------------------------------------------

#include "vsLinuxJoystickSystem.h++"

#include "vsInputButton.h++"
#include "vsInputAxis.h++"

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Quick macro to perform bitwise tests on large buffers
#define TEST_BIT(bit, bitmask) \
   (((1 << ((bit) & 7)) & (((const unsigned char *) bitmask)[(bit) >> 3])) != 0)

// ------------------------------------------------------------------------
// Constructor. Creates a vsJoystick object from the specified port
// ------------------------------------------------------------------------
vsLinuxJoystickSystem::vsLinuxJoystickSystem(char *joystickPortName)
    : vsIOSystem()
{   
    int numAxes;
    int lastButton;
    int i;
    uint8_t absBits[ABS_MAX/8+1];
    uint8_t keyBits[KEY_MAX/8+1];
    struct input_absinfo axisInfo;
    vsInputAxis *axis;
    double travel;
    double idle;
    double thresh;

    // Initialize the joystick member to NULL
    joystick = NULL;

    // Copy the port name (can't use the typical port number interface
    // for this, since it could be any of several kinds of port)
    strcpy(portName, joystickPortName);

    // Open the joystick port in read only and non-blocking modes
    portFD = open(portName, O_RDONLY | O_NONBLOCK);

    // open() returns -1 if an error occurred trying to open a port
    if (portFD == -1)
    {
        printf("vsLinuxJoystickSystem::vsLinuxJoystickSystem: Unable to "
            "open joystick port\n");

        return;
    }

    // Get the axis configuration bitmask
    memset(absBits, 0, sizeof(absBits));
    if (ioctl(portFD, EVIOCGBIT(EV_ABS, sizeof(absBits)), &absBits) < 0)
    {
        printf("vsLinuxJoystickSystem::vsLinuxJoystickSystem: Unable to "
            "determine axis configuration\n");

        return;
    }

    // Get the key (button) configuration bitmask
    memset(keyBits, 0, sizeof(keyBits));
    if (ioctl(portFD, EVIOCGBIT(EV_KEY, sizeof(keyBits)), &keyBits) < 0)
    {
        printf("vsLinuxJoystickSystem::vsLinuxJoystickSystem: Unable to "
            "determine button configuration\n");

        return;
    }

    // Interpret the bitmasks, first the axes
    numAxes = 0;
    memset(axisMap, 0, sizeof(axisMap));
    for (i = 0; i < ABS_MAX; i++)
    {
        // Is this axis there?
        if (TEST_BIT(i, absBits))
        {
            // Map the axis index to the current axis count (so we get a
            // contiguous list of axis indices in our vsJoystick)
            axisMap[i] = numAxes;

            // Found another axis
            numAxes++;
        }
        else
           axisMap[i] = -1;
    }

    // Next, the buttons
    firstButton = -1;
    lastButton = -1;
    for (i = 0; i < KEY_MAX; i++)
    {
        // Is this button there?
        if (TEST_BIT(i, keyBits))
        {
            // Is this the first button we found?  Remember it if so
            if (firstButton < 0)
                firstButton = i;

            // Remember the last button that we find
            lastButton = i;
        }
    }

    // Create the joystick
    joystick = new vsJoystick(numAxes, lastButton - firstButton + 1);

    // Configure the axes
    for (i = 0; i < ABS_MAX; i++)
    {
        // Map the axis index
        if (axisMap[i] > -1)
        {
           // Fetch the configuration
           ioctl(portFD, EVIOCGABS(i), &axisInfo);

           // Get the axis from the joystick
           axis = joystick->getAxis(axisMap[i]);

           // Set the axis parameters
           travel = (double) axisInfo.maximum - (double) axisInfo.minimum;
           idle = travel / 2.0 + (double) axisInfo.minimum;
           thresh = (double) axisInfo.flat / travel;
           axis->setRange(axisInfo.minimum, axisInfo.maximum);
           axis->setIdlePosition(idle);
           axis->setThreshold(thresh);
           axis->setNormalized(true);
       }
    }

    // Update once to get the initial joystick state
    update();
}

// ------------------------------------------------------------------------
// Destructor. Closes the joystick port.
// ------------------------------------------------------------------------
vsLinuxJoystickSystem::~vsLinuxJoystickSystem()
{
    // Close the joystick port
    close(portFD);
    portFD = -1;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsLinuxJoystickSystem::getClassName()
{
    return "vsLinuxJoystickSystem";
}

// ------------------------------------------------------------------------
// Returns the port name specified in the constructor.
// ------------------------------------------------------------------------
char *vsLinuxJoystickSystem::getPortName()
{
    return portName;
}

// ------------------------------------------------------------------------
// Returns the vsJoystick object.
// ------------------------------------------------------------------------
vsJoystick *vsLinuxJoystickSystem::getJoystick()
{
    return joystick;
}

// ------------------------------------------------------------------------
// Updates the values for all the axes and buttons.
// ------------------------------------------------------------------------
void vsLinuxJoystickSystem::update()
{
    int axisNum;
    vsInputAxis *axis;
    vsInputButton *button;
    int bytesRead;
    int eventsRead;
    int buttonIndex;
    struct input_event events[64];
    int i;
    
    // Read all events on the driver queue (read() returns -1
    // when there are no events pending to be read on the queue)
    bytesRead = read(portFD, events, sizeof(events));
    if (bytesRead > 0)
    {
        eventsRead = bytesRead / sizeof(struct input_event);
        for (i = 0; i < eventsRead; i++)
        {
            switch (events[i].type)
            {
                // Handle axis events (joystick moved)
                case EV_ABS:

                    // Get the appropriate axis from the joystick
                    axis = joystick->getAxis(axisMap[events[i].code]);

                    // Set the axis's position
                    axis->setPosition(events[i].value);
                    break;

                // Handle button events (pressed or released)
                case EV_KEY:

                    // Translate the event code to a button index
                    buttonIndex = events[i].code - firstButton;

                    // Get the appropriate button from the joystick
                    button = joystick->getButton(buttonIndex);

                    // Set the button's state
                    if (events[i].value == 0)
                        button->setReleased();
                    else
                        button->setPressed();
                    break;

                default:
                    // Do nothing
                    break;
            }
        }
    }

    // If we have a valid joystick object, update it
    if (joystick != NULL)
        joystick->update();
}
