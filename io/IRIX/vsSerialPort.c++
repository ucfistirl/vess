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
//    VESS Module:  vsSerialPort.c++
//
//    Description:  Class for handling serial port communications.  This
//                  implementation uses the UNIX-standard termio library.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>

#include "vsSerialPort.h++"

// ------------------------------------------------------------------------
// Opens the serial port with the specified device name
// ------------------------------------------------------------------------
vsSerialPort::vsSerialPort(char *deviceName)
{
    // Open the port
    portDescriptor = open(deviceName, O_RDWR);

    // Check to see if the port opened properly
    if (portDescriptor < 0)
    {
        printf("vsSerialPort:  Error opening port %s\n", deviceName);
        return;
    }
   
    // Save the current serial port configuration
    tcgetattr(portDescriptor, &oldAttributes);
    currentAttributes = oldAttributes;
   
    // Set up the default configuration
    setDefaults(&currentAttributes);
    setAttributes(&currentAttributes);
}

// ------------------------------------------------------------------------
// Opens the serial port with the specified device name, and sets the 
// communications parameters to the specified settings
// ------------------------------------------------------------------------
vsSerialPort::vsSerialPort(char *deviceName, long baud, 
                           int wordLength, char parity, int stopBits)
{
    // Open the serial port
    portDescriptor = open(deviceName, O_RDWR);

    // Check to see if the port opened properly
    if (portDescriptor < 0)
    {
        printf("vsSerialPort:  Error opening port %s\n", deviceName);
        return;
    }

    // Save the current serial port configuration
    tcgetattr(portDescriptor, &oldAttributes);
    currentAttributes = oldAttributes;
   
    // Set up the default configuration
    setDefaults(&currentAttributes);
    setAttributes(&currentAttributes);

    // Set the attributes (baud, parity, etc.) passed in
    setBaudRate(baud);
    setParity(parity);
    setWordLength(wordLength);
    setStopBits(stopBits);
}

// ------------------------------------------------------------------------
// Destructor.  Resets the port to the original state and closes the port
// descriptor
// ------------------------------------------------------------------------
vsSerialPort::~vsSerialPort()
{
    // Restore the old port configuration
    setAttributes(&oldAttributes);

    // Close the port
    close(portDescriptor);
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsSerialPort::getClassName()
{
    return "vsSerialPort";
}

// ------------------------------------------------------------------------
// Set the port to the current set of communcations attributes
// ------------------------------------------------------------------------
int vsSerialPort::setAttributes(struct termios *desiredAttributes)
{
    // Configure the serial port according to the given attributes
    return tcsetattr(portDescriptor, TCSAFLUSH, desiredAttributes);
}

// ------------------------------------------------------------------------
// Set up the default communication parameters
// ------------------------------------------------------------------------
void vsSerialPort::setDefaults(struct termios *tioStructure)
{
    // Set the port speed
    cfsetispeed(tioStructure, B9600);
    cfsetospeed(tioStructure, B9600);

    // Set some other defaults (man termios for details)
    tioStructure->c_cflag      = CS8 | CREAD | CLOCAL;
    tioStructure->c_iflag      = IGNBRK | IGNPAR; 
    tioStructure->c_oflag      = 0;
    tioStructure->c_lflag      = 0;
    tioStructure->c_cc[VMIN]   = 0;
    tioStructure->c_cc[VTIME]  = 0;
}

// ------------------------------------------------------------------------
// Print the current communications parameters
// ------------------------------------------------------------------------
void vsSerialPort::termioPrint(struct termios *tioStructure)
{
    fprintf(stderr, "------TERMIO STATUS------\n");
    fprintf(stderr, "c_iflag:  %d\n", tioStructure->c_iflag);
    fprintf(stderr, "c_oflag:  %d\n", tioStructure->c_oflag);
    fprintf(stderr, "c_cflag:  %d\n", tioStructure->c_cflag);
    fprintf(stderr, "c_lflag:  %d\n", tioStructure->c_lflag);
    //fprintf(stderr, "c_ospeed: %d\n", tioStructure->c_ospeed);
    //fprintf(stderr, "c_ispeed: %d\n", tioStructure->c_ispeed);
    fprintf(stderr, "V_MIN:    %d\n", tioStructure->c_cc[VMIN]);
    fprintf(stderr, "V_TIME:   %d\n", tioStructure->c_cc[VTIME]);
    fprintf(stderr, "-------------------------\n");
}

// ------------------------------------------------------------------------
// Write a packet to the port
// ------------------------------------------------------------------------
int vsSerialPort::writePacket(unsigned char *packet, int length)
{
    return write(portDescriptor, packet, length);
}

// ------------------------------------------------------------------------
// Read a packet from the port
// ------------------------------------------------------------------------
int vsSerialPort::readPacket(unsigned char *packet, int length)
{
    int result;
    int bytesRead;
    int timeoutCounter;

    // Set up the byte and timeout counters
    bytesRead = 0;
    timeoutCounter = VS_SERIAL_NUM_READ_RETRYS;

    // Keep reading until we read the specified number of bytes, or
    // the timeout expires
    while ((bytesRead < length) && (timeoutCounter > 0)) 
    {
        // Read from the port
        result = read(portDescriptor, &(packet[bytesRead]), 
            length - bytesRead);

        // Add the number of bytes read on this loop
        if (result > 0)
            bytesRead += result;

        // Decrement timeout counter
        timeoutCounter--;
    }

    return bytesRead;
}

// ------------------------------------------------------------------------
// Returns a character read from the port.  If no character is available
// -1 is returned
// ------------------------------------------------------------------------
int vsSerialPort::readCharacter()
{
    char character;
    int  result;
    bool  readFlag;

    // Read a single byte from the port
    readFlag = read(portDescriptor, &character, 1);

    // Return the character read if we successfully read a byte
    // otherwise, return -1
    if (readFlag)
    {
        result = character;
        return result;
    }
    else
        return -1;
}

// ------------------------------------------------------------------------
// Checks to see if there is data waiting to be read on the port. Will wait
// up to secondsToWait seconds before returning.
// ------------------------------------------------------------------------
bool vsSerialPort::isDataWaiting()
{
    return isDataWaiting(0.0);
}

// ------------------------------------------------------------------------
// Checks to see if there is data waiting to be read on the port. Will wait
// up to secondsToWait seconds before returning.
// ------------------------------------------------------------------------
bool vsSerialPort::isDataWaiting(double secondsToWait)
{
    fd_set readfds;
    struct timeval tv;
    int returnValue;
    
    // Make sure that the port is open before performing any actions on it
    if( portDescriptor >= 0 )
    {
        // Initialize our file descriptor set
        FD_ZERO(&readfds);
        FD_SET(portDescriptor, &readfds);

        // Determine how much time we should wait to see if there is data
        if(fabs(secondsToWait) < 1e-6)
        {
            // The input (secondsToWait) was almost 0.0 so we'll wait 0 seconds
            tv.tv_sec = 0;
            tv.tv_usec = 0;
        }
        else
        {
            // The input (secondsToWait) wasn't 0.0 so calculate the correct
            // time to wait
            tv.tv_sec = (long)trunc(secondsToWait);
            tv.tv_usec = (long)((secondsToWait - trunc(secondsToWait))
                    *((double)1e6));
        }

        // Call select to see if there is data waiting for us
        returnValue = select(portDescriptor+1, &readfds, NULL, NULL, &tv);

        // If the returnValue > 0, then there is data waiting
        if(returnValue > 0)
            return true;
        else if(returnValue == -1)
        {
            // Select returned an error condition
            perror("vsSerialPort::isDataWaiting() - select");
            return false;
        }
    }
    return false;
}

// ------------------------------------------------------------------------
// Set the communication speed
// ------------------------------------------------------------------------
void vsSerialPort::setBaudRate(long baudRate)
{
    long     flags;

    // First, determine the correct flag for the desired
    // new baud rate
    switch (baudRate)
    {
        case 0:
            flags = B0;
            break;
        case 300:
            flags = B300;
            break;
        case 1200:
            flags = B1200;
            break;
        case 2400:
            flags = B2400;
            break;
        case 4800:
            flags = B4800;
            break;
        case 9600:
            flags = B9600;
            break;
        case 19200:
            flags = B19200;
            break;
        case 38400:
            flags = B38400;
            break;
#ifdef B57600
        case 57600:
            flags = B57600;
            break;
#endif
#ifdef B115200
        case 115200:
            flags = B115200;
            break;
#endif
        default:
            flags = B9600;
            break;
    }
   
    // Set the new baud rate flags
    cfsetispeed(&currentAttributes, flags);
    cfsetospeed(&currentAttributes, flags);

    // Change the current Attributes
    if (setAttributes(&currentAttributes) == -1)
    {
        printf("vsSerialPort::setBaudRate:\n");
        perror("   Unable to change baud rate: ");
    }
}

// ------------------------------------------------------------------------
// Set the type of parity checking
// ------------------------------------------------------------------------
void vsSerialPort::setParity(char parity)
{
    long flags;

    // Determine the correct flag(s) for the 
    // desired parity setting
    switch (parity)
    {
        case 'E':
            // Even parity
            flags = PARENB;
            break;

        case 'O':
            // Odd parity
            flags = PARENB | PARODD;
            break;

        case 'N':
            // No parity
            flags = 0;
            break;

        default:
            // Default:  no parity
            flags = 0;
            break;
    };

    // Clear any current parity flags
    if (currentAttributes.c_cflag & PARENB)
        currentAttributes.c_cflag &= ~PARENB;
    if (currentAttributes.c_cflag & PARODD)
        currentAttributes.c_cflag &= ~PARODD;

    // Set the new flags
    currentAttributes.c_cflag |= flags;

    // Change the currentAttributes
    setAttributes(&currentAttributes);
}

// ------------------------------------------------------------------------
// Set the word length
// ------------------------------------------------------------------------
void vsSerialPort::setWordLength(int wordLength)
{
    long flags;

    // Determine the correct flag for the new
    // desired word length (number of data bits)
    switch (wordLength) 
    {
        case 5:
            flags = CS5;
            break;
        case 6:
            flags = CS6;
            break;
        case 7:
            flags = CS7;
            break;
        case 8:
            flags = CS8;
            break;
        default:
            flags = CS8;
            break;
    };

    // Clear any current data bit flags
    if (currentAttributes.c_cflag & CS5)
        currentAttributes.c_cflag &= ~CS5;
    if (currentAttributes.c_cflag & CS6)
        currentAttributes.c_cflag &= ~CS6;
    if (currentAttributes.c_cflag & CS7)
        currentAttributes.c_cflag &= ~CS7;
    if (currentAttributes.c_cflag & CS8)
        currentAttributes.c_cflag &= ~CS8;

    // Set the new flag
    currentAttributes.c_cflag |= flags;

    // Change the currentAttributes
    setAttributes(&currentAttributes);
}

// ------------------------------------------------------------------------
// Set the number of stop bits
// ------------------------------------------------------------------------
void vsSerialPort::setStopBits(int stopBits)
{
    long flags;

    // If we want two stop bits, set the CSTOPB
    // flag, otherwise don't set any
    if (stopBits == 2)
        flags = CSTOPB;     // Two stop bits
    else
        flags = 0;          // One stop bit


    // Clear the current stop bit flag (if it's set)
    if (currentAttributes.c_cflag & CSTOPB)
        currentAttributes.c_cflag &= ~CSTOPB;

    // Set the new flag
    currentAttributes.c_cflag |= flags;

    // Change the currentAttributes
    setAttributes(&currentAttributes);
} 

// ------------------------------------------------------------------------
// Raises (if the parameter is true) or lowers (if false) the RTS line on
// the serial port
// ------------------------------------------------------------------------
void vsSerialPort::setRTS(bool enable)
{
    int status;

    // Get the current control line status
    ioctl(portDescriptor, TIOCMGET, &status);

    // Set the RTS bit appropriately
    if (enable)
        status |= TIOCM_RTS;
    else
        status &= ~TIOCM_RTS;

    // Set the control lines with the new bit setting
    ioctl(portDescriptor, TIOCMSET, &status);
}

// ------------------------------------------------------------------------
// Raises (if the parameter is true) or lowers (if false) the DTR line on
// the serial port
// ------------------------------------------------------------------------
void vsSerialPort::setDTR(bool enable)
{
    int status;

    // Get the current control line status
    ioctl(portDescriptor, TIOCMGET, &status);

    // Set the DTR bit appropriately
    if (enable)
        status |= TIOCM_DTR;
    else
        status &= ~TIOCM_DTR;

    // Set the control lines with the new bit setting
    ioctl(portDescriptor, TIOCMSET, &status);
}

// ------------------------------------------------------------------------
// Send a serial BREAK signal
// ------------------------------------------------------------------------
void vsSerialPort::sendBreakSignal()
{
    tcsendbreak(portDescriptor, 1);
}

// ------------------------------------------------------------------------
// Flush the remaining data in the serial port
// ------------------------------------------------------------------------
void vsSerialPort::flushPort()
{
    tcflush(portDescriptor, TCIFLUSH);
}
