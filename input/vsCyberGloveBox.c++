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
//    VESS Module:  vsCyberGloveBox.c++
//
//    Description:  Input system class supporting the VTI CyberGlove
//                  system
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsCyberGloveBox.h++"
#include <stdio.h>

// ------------------------------------------------------------------------
// Constructor.  Sets up the serial port connection and creates the 
// vsArticulationGlove object
// ------------------------------------------------------------------------
vsCyberGloveBox::vsCyberGloveBox(int portNum, long baud, int nSensors)
{
    char portDevice[50];

    // Figure out which serial port device to use
#ifdef IRIX
    sprintf(portDevice, "/dev/ttyd%d", portNum);
#endif

#ifdef IRIX64
    sprintf(portDevice, "/dev/ttyd%d", portNum);
#endif

#ifdef __linux__
    sprintf(portDevice, "/dev/ttyS%d", portNum - 1);
#endif

    // Open the serial port
    port = new vsSerialPort(portDevice, baud, 8, 'N', 1);

    // Initialize variables
    numSensors = 0;
    touchInstalled = VS_FALSE;

    // Initialize the glove box
    if (port)
    {
        // Initialize hardware
        initialize();

        // Check the number of sensors specified with the number found
        // on the glove
        if ((nSensors < numSensors) && (nSensors != 0))
        {
            printf("vsCyberGloveBox::vsCyberGloveBox:  WARNING: Only using "
                   "%d sensors (%d available)\n", nSensors, numSensors);
    
            numSensors = nSensors;
        }
        else if (nSensors > numSensors)
        {
            printf("vsCyberGloveBox::vsCyberGloveBox:  WARNING: %d sensors "
                   "requested, but only %d available\n", nSensors, numSensors);
        }

        // The number should be 18 or 22, otherwise we can't be sure of
        // the sensor-to-joint mapping
        if ((numSensors != 18) && (numSensors != 22))
        {
            printf("vsCyberGloveBox::vsCyberGloveBox:\n");
            printf("    WARNING: Expected either 18 or 22 sensors\n");
            printf("    Sensor values may not be matched with the proper "
                   "joints\n");
        }

        // If we have less than 22 sensors, we don't have distal joint
        // information, so configure the vsArticulationGlove to estimate
        // them
        if (numSensors < 22)
        {
            // Estimate the distal interphalangial joints
            glove = new vsArticulationGlove(VS_TRUE);
        }
        else
        {
            // Use the distal interphalangial joint sensors (don't estimate)
            glove = new vsArticulationGlove(VS_FALSE);
        }

        // Request the first update
        ping();
    }
}

// ------------------------------------------------------------------------
// Destructor.  Closes the serial port and frees the vsArticulationGlove
// object
// ------------------------------------------------------------------------
vsCyberGloveBox::~vsCyberGloveBox()
{
    // Turn off the CyberTouch motors (if installed)
    stopAllFeedback();

    // Close the serial port
    delete port;

    // Destroy the vsArticulationGlove
    delete glove;
}

// ------------------------------------------------------------------------
// Establishes communication with the glove box and initializes the 
// hardware
// ------------------------------------------------------------------------
void vsCyberGloveBox::initialize()
{
    unsigned char buf[20];

    // Flush the serial port
    port->flushPort();

    // Print status information as we go
    printf("vsCyberGloveBox::initialize:\n");

    // Check to see if the glove is connected
    buf[0] = VS_CYG_CMD_QUERY;
    buf[1] = VS_CYG_CMD_GLOVE_STATUS;
    port->writePacket(buf, 2);

    // Expecting a value of 3 (connected and properly initialized) in the 
    // third byte of the response
    port->readPacket(buf, 4);
    if (buf[2] != 3)
    {
        printf("    ERROR: Glove not connected or not properly initialized\n");
    }
    else
    {
        // Request the number of sensors in the glove
        buf[0] = VS_CYG_CMD_QUERY;
        buf[1] = VS_CYG_CMD_NUM_HW_SENSORS;
        port->writePacket(buf, 2);

        // Read and report the result
        port->readPacket(buf, 4);
        numSensors = (int)buf[2];
        printf("    Glove has %d sensors\n", numSensors);
    
        // Set the number of sensors sampled
        buf[0] = VS_CYG_CMD_NUM_SENSORS;
        buf[1] = (unsigned char)numSensors;
        port->writePacket(buf, 2);

        // Read the acknowledgement
        port->readPacket(buf, 2);

        // Set the sensor mask to all F's (all sensors)
        buf[0] = VS_CYG_CMD_SENSOR_MASK;
        buf[1] = 0xFF;
        buf[2] = 0xFF;
        buf[3] = 0xFF;

        // Read the acknowledgement
        port->readPacket(buf, 2);

        // Request the handedness of the glove
        buf[0] = VS_CYG_CMD_QUERY;
        buf[1] = VS_CYG_CMD_RIGHT_HANDED;
        port->writePacket(buf, 2);

        // Read and report the result
        port->readPacket(buf, 4);
        if (buf[2] == 1)
            printf("    Glove is right-handed\n");
        else
            printf("    Glove is left-handed\n");

        // Check for CyberTouch option
        buf[0] = VS_CYG_CMD_QUERY;
        buf[1] = VS_CYG_CMD_PARAM_FLAGS;
        port->writePacket(buf, 2);

        // Read the result
        port->readPacket(buf, 6);

        // Configure the CyberTouch option if installed
        if (buf[4] & VS_CYG_PARAM_CYBERTOUCH)
        {
            printf("    CyberTouch option present\n");
            touchInstalled = VS_TRUE;
        }
        else
        {
            printf("    CyberTouch option not installed\n");
            touchInstalled = VS_FALSE;
        }
    }
}

// ------------------------------------------------------------------------
// Requests a new data record from the CyberGlove box
// ------------------------------------------------------------------------
void vsCyberGloveBox::ping()
{
    unsigned char buf;

    // Send the ping command to the CyberGlove box
    buf = VS_CYG_CMD_PING;
    port->writePacket(&buf, 1);
}

// ------------------------------------------------------------------------
// Returns the vsArticulationGlove object
// ------------------------------------------------------------------------
vsArticulationGlove *vsCyberGloveBox::getGlove()
{
    return glove;
}

// ------------------------------------------------------------------------
// Starts a CyberTouch actuator vibrating at the specified amplitude
// ------------------------------------------------------------------------
void vsCyberGloveBox::startFeedback(int index, int amplitude)
{
    unsigned char buf[10];

    // Validate the index value
    if (index >= VS_CYG_NUM_ACTUATORS)
    {
        printf("vsCyberGloveBox::startFeedback:  Invalid actuator "
               "specified\n");
        return;
    }

    // Validate the amplitude value
    if ((amplitude < 0) || (amplitude > 255))
    {
        printf("vsCyberGloveBox::startFeedback:  Invalid amplitude "
               "specified\n");
        return;
    }

    // Send the command and appropriate values to the glove box
    buf[0] = VS_CYG_CMD_CYBERTOUCH;
    buf[1] = 1;
    buf[2] = index;
    buf[3] = amplitude;
    port->writePacket(buf, 4);
}

// ------------------------------------------------------------------------
// Stops a CyberTouch actuator from vibrating
// ------------------------------------------------------------------------
void vsCyberGloveBox::stopFeedback(int index)
{
    unsigned char buf[10];

    // Validate the index value
    if (index >= VS_CYG_NUM_ACTUATORS)
    {
        printf("vsCyberGloveBox::startFeedback:  Invalid actuator "
               "specified\n");
        return;
    }

    // Send the command and appropriate values to the glove box
    buf[0] = VS_CYG_CMD_CYBERTOUCH;
    buf[1] = 1;
    buf[2] = index;
    buf[3] = 0;
    port->writePacket(buf, 4);
}

// ------------------------------------------------------------------------
// Starts all CyberTouch actuators vibrating at the specified amplitude
// ------------------------------------------------------------------------
void vsCyberGloveBox::startAllFeedback(int amplitude)
{
    unsigned char buf[10];

    // Validate the amplitude value
    if ((amplitude < 0) || (amplitude > 255))
    {
        printf("vsCyberGloveBox::startFeedback:  Invalid amplitude "
               "specified\n");
        return;
    }

    // Send the command and appropriate values to the glove box
    buf[0] = VS_CYG_CMD_CYBERTOUCH;
    buf[1] = 255;
    buf[2] = amplitude;
    buf[3] = amplitude;
    buf[4] = amplitude;
    buf[5] = amplitude;
    buf[6] = amplitude;
    buf[7] = amplitude;
    port->writePacket(buf, 8);
}

// ------------------------------------------------------------------------
// Stops all CyberTouch actuators from vibrating
// ------------------------------------------------------------------------
void vsCyberGloveBox::stopAllFeedback()
{
    unsigned char buf[10];

    // Send the command and appropriate values to the glove box
    buf[0] = VS_CYG_CMD_CYBERTOUCH;
    buf[1] = 255;
    buf[2] = 0;
    buf[3] = 0;
    buf[4] = 0;
    buf[5] = 0;
    buf[6] = 0;
    buf[7] = 0;
    port->writePacket(buf, 8);
}

// ------------------------------------------------------------------------
// Updates the glove with fresh data
// ------------------------------------------------------------------------
void vsCyberGloveBox::update()
{
    unsigned char buf[50];
    vsInputAxis   *axis;
    int           bytesRead;
    int           stat;
    int           retry;
    int           sensor, bufPos;

    // Initialize the communications variables
    buf[0] = 0;
    bytesRead = 0;
    retry = 10;

    // Read until we get a 'G' response byte and we haven't exhausted
    // our number of retries
    while ((buf[0] != 'G') && (retry > 0))
    {   
        // Read one byte from the port
        stat = port->readPacket(buf, 1);

        // Re-ping if we need to
        if (stat == 0)
        {
            retry--;
            ping();
        }
    }

    // Signal an error and bail out if we can't get valid data
    if (retry == 0)
    {
        printf("vsCyberGloveBox::update:  Unable to communicate with the "
               "CyberGlove box!\n");
        return;
    }

    // Increment bytesRead (we've read one byte so far)
    bytesRead++;

    // Read the rest of the packet.  All CyberGlove packets are terminated 
    // with NULL, so this makes a good stop condition for the read loop.
    while (buf[bytesRead-1] != 0)
    {
        port->readPacket(&buf[bytesRead], 1);
        bytesRead++;
    }

    // Initialize sensor and buffer position counters
    sensor = 0;
    bufPos = 1;

    // Set the axis position for each sensor
    while ((bufPos < bytesRead) && (sensor < VS_AG_NUM_SENSORS))
    {
        // Set the axis position for this sensor
        axis = glove->getAxis(sensor);
        axis->setPosition((double)buf[bufPos]);

        // Increment both counters
        bufPos++;
        sensor++;

        // Skip over the distal sensors unless we have them installed in
        // our glove
        if (((sensor == VS_AG_SENSOR_INDEX_DIJ) ||
             (sensor == VS_AG_SENSOR_MIDDLE_DIJ) ||
             (sensor == VS_AG_SENSOR_RING_DIJ) ||
             (sensor == VS_AG_SENSOR_PINKY_DIJ)) &&
            (numSensors < 22))
        {
            // Set the position to 0
            glove->getAxis(sensor)->setPosition(0);

            // Increment the sensor counter only (this data won't be
            // in the buffer at all)
            sensor++;
        }

        // Skip the index absolute abduction sensor, as it is "not yet
        // implemented" (according to the manual)
        if (sensor == VS_AG_SENSOR_INDEX_ABD)
        {
            sensor++;
        }
    }

    // Update the joint angles on the vsArticulationGlove
    glove->update();

    // Request the next update
    ping();
}
