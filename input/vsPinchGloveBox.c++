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
//    VESS Module:  vsPinchGloveBox.c++
//
//    Description:  Input system class supporting the Fakespace PINCH
//                  glove system
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "vsPinchGloveBox.h++"

// ------------------------------------------------------------------------
// Constructor.  Opens a PINCH glove box on the given serial port and 
// gets its information.
// ------------------------------------------------------------------------
vsPinchGloveBox::vsPinchGloveBox(int portNumber, long baud)
               : vsInputSystem()
{
    char          portDevice[20];
    unsigned char buf[100];
    int           size;


    port = NULL;
    gloves = NULL;

#ifdef IRIX
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef IRIX64
    sprintf(portDevice, "/dev/ttyd%d", portNumber);
#endif

#ifdef __linux__
    sprintf(portDevice, "/dev/ttyS%d", portNumber - 1);
#endif

    // Open the serial port
    port = new vsSerialPort(portDevice, baud, 8, 'N', 1);

    if (port)
    {
        printf("Fakespace PINCH glove system opened on %s\n", portDevice);

        // Send bytes until we get a '?' back from the PINCH box
        // This will synchronize the driver with the box's 2-byte command
        // format
        buf[1] = 0x00;
        while (buf[1] != '?')
        {
            buf[0] = 'A';
            port->writePacket(buf, 1);
            usleep(100000);
            memset(buf, 0, 3);
            port->readPacket(buf, 3);
        }

        // Flush the port
        port->flushPort();

        // Turn off time stamps
        buf[0] = VS_PG_CMD_TIMESTAMP;
        port->writePacket(buf, 1);
        usleep(1000);
        buf[0] = '0';
        port->writePacket(buf, 1);
        usleep(1000);
        readPacket(buf, 100, VS_PG_RESPONSE_PACKET);
        usleep(1000);

        // Set protocol version to 1
        buf[0] = VS_PG_CMD_VERSION;
        port->writePacket(buf, 1);
        usleep(1000);
        buf[0] = '1';
        port->writePacket(buf, 1);
        usleep(1000);
        readPacket(buf, 100, VS_PG_RESPONSE_PACKET);
        usleep(1000);

        // Get the device information
        printf("Revision   :  ");
        buf[0] = VS_PG_CMD_CONFIG;
        port->writePacket(buf, 1);
        usleep(1000);
        buf[0] = VS_PG_CONFIG_CPU;
        port->writePacket(buf, 1);
        usleep(1000);
        size = readPacket(buf, 100, VS_PG_RESPONSE_PACKET);
        buf[size] = 0;
        printf("%s\n", (const char *) &buf[1]);
        usleep(1000);

        printf("Left Glove :  ");
        buf[0] = VS_PG_CMD_CONFIG;
        port->writePacket(buf, 1);
        usleep(1000);
        buf[0] = VS_PG_CONFIG_LEFT;
        port->writePacket(buf, 1);
        usleep(1000);
        size = readPacket(buf, 100, VS_PG_RESPONSE_PACKET);
        buf[size] = 0;
        printf("%s\n", (const char *) &buf[1]);
        usleep(1000);

        printf("Right Glove:  ");
        buf[0] = VS_PG_CMD_CONFIG;
        port->writePacket(buf, 1);
        usleep(1000);
        buf[0] = VS_PG_CONFIG_RIGHT;
        port->writePacket(buf, 1);
        usleep(1000);
        size = readPacket(buf, 100, VS_PG_RESPONSE_PACKET);
        buf[size] = 0;
        printf("%s\n", (const char *) &buf[1]);
        usleep(1000);

        gloves = new vsChordGloves();
        gloves->clearContacts();
    }
}

// ------------------------------------------------------------------------
// Destructor.  Closes the serial port and deletes the glove object
// ------------------------------------------------------------------------
vsPinchGloveBox::~vsPinchGloveBox()
{
    if (gloves)
        delete gloves;

    if (port)
        delete port;
}

// ------------------------------------------------------------------------
// Utility function (protected).  Reads a packet from the glove box in 
// byte-by-byte fashion.  This is the most effective method for this 
// device.  The parameter specified which byte in the buffer will indicate 
// the start of the packet.  This header may or may not already be in the
// buffer (in the first position).
// ------------------------------------------------------------------------
int vsPinchGloveBox::readPacket(unsigned char *buffer,
                                int bufferSize,
                                unsigned char packetHeader)
{
    int i;
    int timeout;
    int goodByte;

    timeout = 10;

    // Look for the packet header in the buffer.  If it's not already there
    // poll the serial port
    while ((buffer[0] != packetHeader) && (timeout > 0))
    {
        goodByte = port->readPacket(&buffer[0], 1);

        if ((!goodByte) || (buffer[0] != packetHeader))
            timeout--;
    }

    if (timeout <= 0)
    {
        printf("vsPinchGloveBox::readPacket:  Error reading packet from glove"
            " box!\n");

        return 0;
    }

    i = 0;
    // Read and store bytes until the end of packet byte is encountered, or
    // the buffer length is reached
    while ((buffer[i] != VS_PG_END_PACKET) && 
           (i < (bufferSize-1)) && (timeout > 0))
    {
        i++;
        goodByte = port->readPacket(&buffer[i], 1);

        if (!goodByte)  
            timeout--;
    }

    if (timeout <= 0)
    {
        printf("vsPinchGloveBox::readPacket:  Error reading packet from glove"
            " box!\n");

        return 0;
    }

    return i;
}

// ------------------------------------------------------------------------
// Returns the vsChordGloves object created by this input system
// ------------------------------------------------------------------------
vsChordGloves *vsPinchGloveBox::getGloves()
{
    return gloves;
}

// ------------------------------------------------------------------------
// Updates the glove box
// ------------------------------------------------------------------------
void vsPinchGloveBox::update()
{
    unsigned char buf[VS_PG_MAX_DATA_SIZE];
    int ch;
    int size, i, j;
    int contacts[20];
    int numContacts;

    // Initialize buffer and retry counter
    memset(buf, 0, sizeof(buf));

    // Look for the start packet byte
    ch = port->readCharacter();

    // Return immediately if no data
    if (ch < 0)
        return;

    if (((unsigned char)ch) != VS_PG_DATA_PACKET)
        return;

    buf[0] = (unsigned char)ch;

    // Read the remainder of the packet
    size = readPacket(buf, sizeof(buf), VS_PG_DATA_PACKET);

    // Check for a read error
    if (size <= 0)
    {
        // Flush the serial port and return
        port->flushPort();
        return;
    }

    // Process the packet and update the state of the gloves
    gloves->clearContacts();

    // Prepare an array for the list of contacts in a single group
    numContacts = 0;
    memset(contacts, 0, sizeof(contacts));

    i = 1; 
    while ((i < size) && (buf[i] != VS_PG_END_PACKET))
    {
        // Check each digit to see if it is in this contact group
        if (buf[i] & VS_PG_THUMB_BIT)
        {
            contacts[numContacts] = VS_CG_LTHUMB;
            numContacts++;
        }
        if (buf[i] & VS_PG_FORE_BIT)
        {
            contacts[numContacts] = VS_CG_LFORE;
            numContacts++;
        }
        if (buf[i] & VS_PG_MIDDLE_BIT)
        {
            contacts[numContacts] = VS_CG_LMIDDLE;
            numContacts++;
        }
        if (buf[i] & VS_PG_RING_BIT)
        {
            contacts[numContacts] = VS_CG_LRING;
            numContacts++;
        }
        if (buf[i] & VS_PG_PINKY_BIT)
        {
            contacts[numContacts] = VS_CG_LPINKY;
            numContacts++;
        }
        if (buf[i+1] & VS_PG_THUMB_BIT)
        {
            contacts[numContacts] = VS_CG_RTHUMB;
            numContacts++;
        }
        if (buf[i+1] & VS_PG_FORE_BIT)
        {
            contacts[numContacts] = VS_CG_RFORE;
            numContacts++;
        }
        if (buf[i+1] & VS_PG_MIDDLE_BIT)
        {
            contacts[numContacts] = VS_CG_RMIDDLE;
            numContacts++;
        }
        if (buf[i+1] & VS_PG_RING_BIT)
        {
            contacts[numContacts] = VS_CG_RRING;
            numContacts++;
        }
        if (buf[i+1] & VS_PG_PINKY_BIT)
        {
            contacts[numContacts] = VS_CG_RPINKY;
            numContacts++;
        }

        // Add the contacts to the state of the gloves
        for (j = 0; j < numContacts; j++)
        {
            gloves->connect(contacts[j], contacts[(j+1) % numContacts]);
        }

        // Reset for the next pair of bytes
        memset(contacts, 0, sizeof(contacts));
        numContacts = 0;

        // Increment the index
        i += 2;
    }
}
