//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2003, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsParallelPort.c++
//
//    Description:  Support for the Parallel Port under Linux
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#include "vsParallelPort.h++"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Linux specific parallel-port includes
#include <linux/ppdev.h>
#include <linux/parport.h>

// --------------------------------------------------------------------------
// Constructor
// Open up access to the parallel port
// --------------------------------------------------------------------------
vsParallelPort::vsParallelPort( const char * deviceName )
    : portOpen( false )
{
    // Open the port
    portDescriptor = open(deviceName, O_RDWR);

    // Check to see if the port opened properly
    if (portDescriptor < 0)
    {
        printf("vsParallelPort:  Error opening port %s\n", deviceName);
        perror("vsParallelPort");
        return;
    }

    // Claim the parallel port for our own!
    if (ioctl(portDescriptor,PPCLAIM) != 0)
    {
        perror("vsParallelPort:  Unable to claim parallel port");
        close(portDescriptor);
        return;
    }

    setMode(VS_PARALLEL_PORT_MODE_COMPATIBILITY);

    portOpen = true;
}

// --------------------------------------------------------------------------
// Constructor
// Open up access to the parallel port with the given communications mode
// --------------------------------------------------------------------------
vsParallelPort::vsParallelPort( const char * deviceName,
        vsParallelPortMode newPortMode )
    : portOpen( false )
{
    // Open the port
    portDescriptor = open(deviceName, O_RDWR);

    // Check to see if the port opened properly
    if (portDescriptor < 0)
    {
        printf("vsParallelPort:  Error opening port %s\n", deviceName);
        return;
    }

    // Claim the parallel port for our own!
    if (ioctl(portDescriptor,PPCLAIM) != 0)
    {
        perror("vsParallelPort:  Unable to claim parallel port");
        close(portDescriptor);
        return;
    }

    setMode(portMode);

    portOpen = true;
}

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
vsParallelPort::~vsParallelPort()
{
    if( portOpen )
    {
        // Release the port to the kernel
        ioctl(portDescriptor,PPRELEASE);

        // Close the port
        close(portDescriptor);
    }
}

// --------------------------------------------------------------------------
// Inherited from vsObject
// --------------------------------------------------------------------------
const char * vsParallelPort::getClassName()
{
    return "vsParallelPort";
}

// --------------------------------------------------------------------------
// Check to see if the port is really open. (it might not be open if an error
// occurred in the constructor)
// --------------------------------------------------------------------------
bool vsParallelPort::isPortOpen()
{
    return portOpen;
}

// --------------------------------------------------------------------------
// Write a stream of data to the parallel port using the current communicati-
// ons method
// --------------------------------------------------------------------------
int vsParallelPort::writePacket( const unsigned char * string, int length )
{
    if( portOpen )
        return write( portDescriptor, (const void *)string, length );
    else
        return -1;
}

// --------------------------------------------------------------------------
// Reads a stream of data to the parallel port using the current communicati-
// ons method
// --------------------------------------------------------------------------
int vsParallelPort::readPacket( unsigned char * string, int length )
{
    if( portOpen )
        return read( portDescriptor, (void *)string, length );
    else
        return -1;
}

// --------------------------------------------------------------------------
// Set the communications method with the port
// --------------------------------------------------------------------------
int vsParallelPort::setMode( vsParallelPortMode newPortMode )
{
    int newMode, retVal;

    if( !portOpen )
        return 0;

    switch( newPortMode )
    {
        case VS_PARALLEL_PORT_MODE_NIBBLE:
            newMode = IEEE1284_MODE_NIBBLE;
            break;

        case VS_PARALLEL_PORT_MODE_BYTE:
            newMode = IEEE1284_MODE_BYTE;
            break;

        case VS_PARALLEL_PORT_MODE_EPP:
            newMode = IEEE1284_MODE_EPP;
            break;

        case VS_PARALLEL_PORT_MODE_ECP:
            newMode = IEEE1284_MODE_ECP;
            break;

        case VS_PARALLEL_PORT_MODE_COMPATIBILITY:
        default:
            newMode = IEEE1284_MODE_COMPAT;
            break;
    }

    if (retVal = ioctl(portDescriptor,PPSETMODE,&newMode))
        perror("vsParallelPort:  Unable to set communications mode");

    return retVal;
}

// --------------------------------------------------------------------------
// The data pins on modern systems are bi-directional. Use this to change the
// direction of the data.
// (forward = host -> peripheral ---- reverse = host <- peripheral)
// --------------------------------------------------------------------------
void vsParallelPort::setDataDirection( bool isForward )
{
    int forwardReverse;

    if( !portOpen )
        return;

    if( isForward )
        forwardReverse = 0;
    else
        forwardReverse = 1;

    if (ioctl(portDescriptor,PPDATADIR,forwardReverse))
        perror("vsParallelPort:  Unable to set data direction");
}

// --------------------------------------------------------------------------
// Set the data output on the parallel port (when the direction is forward)
// --------------------------------------------------------------------------
void vsParallelPort::setDataLines( unsigned char dataByte )
{
    if( !portOpen )
        return;

    if (ioctl(portDescriptor,PPWDATA,&dataByte))
        perror("vsParallelPort:  Unable to set data lines");
}

// --------------------------------------------------------------------------
// Get the status of the data pins (when the direction is reverse)
// --------------------------------------------------------------------------
unsigned char vsParallelPort::getDataLines()
{
    unsigned char dataByte = 0x0;

    if( portOpen )
    {
        if (ioctl(portDescriptor,PPRDATA,&dataByte))
            perror("vsParallelPort:  Unable to read data lines");
    }

    return dataByte;
}

// --------------------------------------------------------------------------
// Get the status of the status pins
// --------------------------------------------------------------------------
int vsParallelPort::getStatusLines()
{
    unsigned char dataByte = 0x0;
    int returnByte = 0x0;

    if( portOpen )
    {
        if (ioctl(portDescriptor,PPRSTATUS,&dataByte))
            perror("vsParallelPort:  Unable to read status lines");

        if( dataByte & PARPORT_STATUS_ERROR )
            returnByte |= VS_PARALLEL_PORT_STATUS_ERROR;
        if( dataByte & PARPORT_STATUS_SELECT )
            returnByte |= VS_PARALLEL_PORT_STATUS_SELECT;
        if( dataByte & PARPORT_STATUS_PAPEROUT )
            returnByte |= VS_PARALLEL_PORT_STATUS_PAPEROUT;
        if( dataByte & PARPORT_STATUS_ACK )
            returnByte |= VS_PARALLEL_PORT_STATUS_ACK;
        if( dataByte & PARPORT_STATUS_BUSY )
            returnByte |= VS_PARALLEL_PORT_STATUS_BUSY;
    }

    return dataByte;
}

// --------------------------------------------------------------------------
// Set the status of the control pins
// --------------------------------------------------------------------------
void vsParallelPort::setControlLines(int controlLines)
{
    unsigned char dataByte = 0x0;

    if( portOpen )
    {
        if (controlLines & VS_PARALLEL_PORT_CONTROL_STROBE)
            dataByte |= PARPORT_CONTROL_STROBE;
        if (controlLines & VS_PARALLEL_PORT_CONTROL_AUTOFD)
            dataByte |= PARPORT_CONTROL_AUTOFD;
        if (controlLines & VS_PARALLEL_PORT_CONTROL_INIT)
            dataByte |= PARPORT_CONTROL_INIT;
        if (controlLines & VS_PARALLEL_PORT_CONTROL_SELECT)
            dataByte |= PARPORT_CONTROL_SELECT;

        if (ioctl(portDescriptor,PPWCONTROL,&dataByte))
            perror("vsParallelPort:  Unable to read status lines");
    }
}

// --------------------------------------------------------------------------
// Set the timeout value for transmissions using writePacket/readPacket
// --------------------------------------------------------------------------
void vsParallelPort::setTimeout( double timeoutInSeconds )
{
    struct timeval timeout;

    if( !portOpen )
        return;

    if( timeoutInSeconds < 1.0e-6 )
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
    }
    else
    {
        timeout.tv_sec = (long)floor(timeoutInSeconds);
        timeout.tv_usec = (long)
            ( (timeoutInSeconds - floor(timeoutInSeconds)) * 1.0e6 );
    }

    if (ioctl(portDescriptor,PPSETTIME,&timeout))
        perror("vsParallelPort:  Unable to set timeout");
}
