#include "vsWSSpaceball.h++"

// ------------------------------------------------------------------------
// Find and initialize the Spaceball on the given window
// ------------------------------------------------------------------------
vsWSSpaceball::vsWSSpaceball(vsWindowSystem *ws, int nButtons)
{
    int result;

    // Get the display and window
    display = ws->getDisplay();
    window = ws->getWindow();

    // Initialize variables
    spaceball = NULL;
    sbDevice = NULL;

    spaceball = new vsSpaceball(nButtons);

    // Initialize the spaceball events and such
    result = initializeSpaceball();
    if (result)
    {
        printf("vsWSSpaceball::vsWSSpaceball:  Spaceball input initialized\n");
    }
    else
    {
        printf("vsWSSpaceball::vsWSSpaceball:  Error initializing"
               " Spaceball input\n");
    }
}

// ------------------------------------------------------------------------
// Release the spaceball device 
// ------------------------------------------------------------------------
vsWSSpaceball::~vsWSSpaceball(void)
{
    // Close the spaceball
    if (sbDevice)
        XCloseDevice(display, sbDevice);

    // Destroy the spaceball data handler
    if (spaceball)
        delete spaceball;
}

// ------------------------------------------------------------------------
// Use the X11 Input Extension to find and communicate with the spaceball
// ------------------------------------------------------------------------
int vsWSSpaceball::initializeSpaceball()
{
    XDeviceInfoPtr deviceInfo;
    XEventClass    eventClasses[3];
    int            sbMotionClass, sbButtonPressClass, sbButtonReleaseClass;
    int            i;
    int            numDevices;

    // Get the list of extension devices
    deviceInfo = XListInputDevices(display, &numDevices);

    // Search the list for a device called "spaceball"
    for (i = 0; i < numDevices; i++)
    {
        printf("Found device %s\n", deviceInfo[i].name);
        if ((strcmp(deviceInfo[i].name, XI_SPACEBALL) == 0) ||
            (strcmp(deviceInfo[i].name, "spaceball") == 0))
        {
            // Open the spaceball device
            sbDevice = XOpenDevice(display, deviceInfo[i].id);
        }
    }

    if (!sbDevice)
    {
        // Oops, not there
        return VS_FALSE;
    }

    // Generate the spaceball event classes
    DeviceMotionNotify(sbDevice, sbMotion, sbMotionClass);
    eventClasses[0] = sbMotionClass;

    DeviceButtonPress(sbDevice, sbButtonPress, sbButtonPressClass);
    eventClasses[1] = sbButtonPressClass;

    DeviceButtonRelease(sbDevice, sbButtonRelease, sbButtonReleaseClass);
    eventClasses[2] = sbButtonReleaseClass;

    // Select the events for receiving
    XSelectExtensionEvent(display, window, eventClasses, 3);

    return VS_TRUE;
}

// ------------------------------------------------------------------------
// Returns the vsSpaceball object owned by this object
// ------------------------------------------------------------------------
vsSpaceball *vsWSSpaceball::getSpaceball()
{
    return spaceball;
}

// ------------------------------------------------------------------------
// Update function.  Processes X Extension events relating to the spaceball
// ------------------------------------------------------------------------
void vsWSSpaceball::update()
{
    XEvent             event;
    XDeviceMotionEvent *motionEvent;
    XDeviceButtonEvent *buttonEvent;
    int                i;
    int                sbData[6];
    vsVector           tempVec;
    int                buttonNumber;

    vsInputButton      *button;

    // Check for and process any motion events
    while (XCheckTypedWindowEvent(display, window, sbMotion, &event))
    {
        motionEvent = (XDeviceMotionEvent *)&event;

        if (motionEvent->deviceid = sbDevice->device_id)
        {
            if (motionEvent->axes_count != 1)
            {
                for (i = 0; i < motionEvent->axes_count; i++)
                {
                    sbData[motionEvent->first_axis + i] = 
                        motionEvent->axis_data[i];
                }

                // Process position
                tempVec.setSize(3);
                tempVec[VS_X] = (float)sbData[0] / 32767.0f;
                tempVec[VS_Y] = (float)sbData[1] / 32767.0f;
                tempVec[VS_Z] = (float)sbData[2] / 32767.0f;

                spaceball->setPosition(tempVec);

                // Process orientation
                tempVec[VS_H] = (float)sbData[3] / 32767.0f;
                tempVec[VS_P] = (float)sbData[4] / 32767.0f;
                tempVec[VS_R] = (float)sbData[5] / 32767.0f;

                spaceball->setOrientation(tempVec);
            }
        }
    }

    // Check for and process any button press events
    while (XCheckTypedWindowEvent(display, window, sbButtonPress, &event))
    {
        buttonEvent = (XDeviceButtonEvent *)&event;

        buttonNumber = buttonEvent->button;

        // Button 9 is the PICK button (the one on the ball itself)
        // We reposition this at index 0 in the button array
        if (buttonNumber == 9)
        {
            button = spaceball->getButton(0);
            if (button)
                button->setReleased();
        }
        else 
        {
            button = spaceball->getButton(buttonNumber);
            if (button)
                button->setReleased();
        }
    }

    // Check for and process any button release events
    while (XCheckTypedWindowEvent(display, window, sbButtonRelease, &event))
    {
        buttonEvent = (XDeviceButtonEvent *)&event;

        buttonNumber = buttonEvent->button;

        // Button 9 is the PICK button (the one on the ball itself)
        // We reposition this at index 0 in the button array
        if (buttonNumber == 9)
        {
            button = spaceball->getButton(0);
            if (button)
                button->setReleased();
        }
        else 
        {
            button = spaceball->getButton(buttonNumber);
            if (button)
                button->setReleased();
        }
    }
}
