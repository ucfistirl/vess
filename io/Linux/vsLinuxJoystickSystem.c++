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

// ------------------------------------------------------------------------
// Constructor. Creates a vsJoystick object from the specified port
// ------------------------------------------------------------------------
vsLinuxJoystickSystem::vsLinuxJoystickSystem(char *joystickPortName)
    : vsInputSystem()
{   
    char totalAxes;
    char totalButtons;

    // Copy the port name (can't use the typical port number interface
    // for this, since it could be any of several kinds of port)
    strcpy(portName, joystickPortName);

    // Open the joystick port in read only and non-blocking modes
    portFileDescriptor = open(portName, O_RDONLY | O_NONBLOCK);

    // open() returns -1 if an error occurred trying to open a port
    if (portFileDescriptor == -1)
    {
        printf("vsLinuxJoystickSystem::vsLinuxJoystickSystem: Unable to "
            "open joystick port\n");

        return;
    }

    // Read the number of axes and buttons
    ioctl(portFileDescriptor, JSIOCGAXES,    &totalAxes);
    ioctl(portFileDescriptor, JSIOCGBUTTONS, &totalButtons);

    // Create the joystick
    joystick = new vsJoystick(int(totalAxes), int(totalButtons),
        VS_LINUX_JS_AXIS_MIN, VS_LINUX_JS_AXIS_MAX);
}

// ------------------------------------------------------------------------
// Destructor. Closes the joystick port.
// ------------------------------------------------------------------------
vsLinuxJoystickSystem::~vsLinuxJoystickSystem()
{
    // Close the joystick port
    close(portFileDescriptor);
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
    vsInputAxis   *axis;
    vsInputButton *button;

    // Read all events on the driver queue (read() returns -1
    // when there are no events pending to be read on the queue)
    while (read(portFileDescriptor, &joystickEvent,
        sizeof(struct js_event)) > 0)
    {
        // Do not differentiate between init events and real events
        joystickEvent.type &= ~JS_EVENT_INIT;

        switch (joystickEvent.type)
        {
            // Handle axis events (joystick moved)
            case JS_EVENT_AXIS:
                // Get the appropriate axis from the joystick
                axis = joystick->getAxis(joystickEvent.number);

                // Set the axis's position
                axis->setPosition(joystickEvent.value);
                break;

            // Handle button events (pressed or released)
            case JS_EVENT_BUTTON:
                // Get the appropriate button from the joystick
                button = joystick->getButton(joystickEvent.number);

                // Set the button's state
                if (joystickEvent.value == VS_LINUX_JS_BUTTON_PRESSED)
                    button->setPressed();
                else if (joystickEvent.value == VS_LINUX_JS_BUTTON_RELEASED)
                    button->setReleased();
                break;

            default:
                // Do nothing
                break;
        }
    }
}
