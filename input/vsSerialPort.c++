#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "vsSerialPort.h++"

// ------------------------------------------------------------------------
// Opens the serial port with the specified device name
// ------------------------------------------------------------------------
vsSerialPort::vsSerialPort(char *deviceName)
{
    portDescriptor = open(deviceName, O_RDWR);
    if (portDescriptor < 0)
    {
        printf("vsSerialPort:  Error opening port %s\n", deviceName);
        return;
    }
   
    tcgetattr(portDescriptor, &oldAttributes);
    currentAttributes = oldAttributes;
   
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
    portDescriptor = open(deviceName, O_RDWR);
    if (portDescriptor < 0)
    {
        printf("vsSerialPort:  Error opening port %s\n", deviceName);
        return;
    }
   
    tcgetattr(portDescriptor, &oldAttributes);
    currentAttributes = oldAttributes;
   
    setDefaults(&currentAttributes);
    setAttributes(&currentAttributes);

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
    setAttributes(&oldAttributes);
    close(portDescriptor);
}

// ------------------------------------------------------------------------
// Set the port to the current set of communcations attributes
// ------------------------------------------------------------------------
int vsSerialPort::setAttributes(struct termios *desiredAttributes)
{
    return tcsetattr(portDescriptor, TCSAFLUSH, desiredAttributes);
}

// ------------------------------------------------------------------------
// Set up the default communication parameters
// ------------------------------------------------------------------------
void vsSerialPort::setDefaults(struct termios *tioStructure)
{
    cfsetispeed(tioStructure, B9600);
    cfsetospeed(tioStructure, B9600);

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
    int bytesRead;
    int timeoutCounter;

    bytesRead = 0;
    timeoutCounter = VS_SERIAL_NUM_READ_RETRYS;

    while ((bytesRead < length) && (timeoutCounter > 0)) 
    {
        bytesRead += read(portDescriptor, &(packet[bytesRead]), 
            length - bytesRead);
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
    int character;
    int readFlag;

    readFlag = read(portDescriptor, &character, 1);

    if (readFlag == 1)
        return character;
    else
        return -1;
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
        case 'E':                      // Even parity
            flags = PARENB;
            break;
        case 'O':                      // Odd parity
            flags = PARENB | PARODD;
            break;
        case 'N':                      // No parity
            flags = 0;
            break;
        default:                       // Default:  no parity
            flags = 0;
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
// Flush the remaining data in the serial port
// ------------------------------------------------------------------------
void vsSerialPort::flushPort()
{
    tcflush(portDescriptor, TCIFLUSH);
}
