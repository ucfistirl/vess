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
//                  implementation uses the Win32 API.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include <stdio.h>
#include "vsSerialPort.h++"

// ------------------------------------------------------------------------
// Opens the serial port with the specified device name
// ------------------------------------------------------------------------
vsSerialPort::vsSerialPort(char *deviceName)
{
    BOOL result;
    
    // Open the port
    portDescriptor = CreateFile(deviceName, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);

    // Check to see if the port opened properly
    if (portDescriptor == INVALID_HANDLE_VALUE)
    {
        printf("vsSerialPort:  Error opening port %s\n", deviceName);
        return;
    }
   
    // Obtain the current serial port configuration
    result = GetCommState(portDescriptor, &oldAttributes);
    if (!result)
    {
        printf("vsSerialPort: Unable to get port configuration\n");
        return;
    }
    
    // Copy the existing port attributes
    currentAttributes = oldAttributes;
    
    // Obtain the current communications timeout values
    result = GetCommTimeouts(portDescriptor, &oldTimeouts);
    if (!result)
    {
        printf("vsSerialPort: Unable to get port timeout configuration\n");
        return;
    }
    
    // Copy the existing timeout attributes
    currentTimeouts = oldTimeouts;
   
    // Set up the default configuration
    setDefaults(&currentAttributes, &currentTimeouts);
    setAttributes(&currentAttributes, &currentTimeouts);
}

// ------------------------------------------------------------------------
// Opens the serial port with the specified device name, and sets the 
// communications parameters to the specified settings
// ------------------------------------------------------------------------
vsSerialPort::vsSerialPort(char *deviceName, long baud, 
                           int wordLength, char parity, int stopBits)
{
    BOOL result;
    
    // Open the port
    portDescriptor = CreateFile(deviceName, GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);

    // Check to see if the port opened properly
    if (portDescriptor == INVALID_HANDLE_VALUE)
    {
        printf("vsSerialPort:  Error opening port %s\n", deviceName);
        return;
    }
   
    // Obtain the current serial port configuration
    result = GetCommState(portDescriptor, &oldAttributes);
    if (!result)
    {
        printf("vsSerialPort: Unable to get port configuration\n");
        return;
    }
    
    // Copy the existing port attributes
    currentAttributes = oldAttributes;
    
    // Obtain the current communications timeout values
    result = GetCommTimeouts(portDescriptor, &oldTimeouts);
    if (!result)
    {
        printf("vsSerialPort: Unable to get port timeout configuration\n");
        return;
    }
    
    // Copy the existing timeout attributes
    currentTimeouts = oldTimeouts;
   
    // Set up the default configuration
    setDefaults(&currentAttributes, &currentTimeouts);
    setAttributes(&currentAttributes, &currentTimeouts);

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
    setAttributes(&oldAttributes, &oldTimeouts);

    // Close the port
    CloseHandle(portDescriptor);
}

// ------------------------------------------------------------------------
// Set the port to the current set of communcations attributes
// ------------------------------------------------------------------------
int vsSerialPort::setAttributes(DCB *newDCB, COMMTIMEOUTS *newTimeouts)
{
    int result1, result2;
    
    // Configure the serial port according to the given attributes
    result1 = (int)SetCommState(portDescriptor, newDCB);
    result2 = (int)SetCommTimeouts(portDescriptor, newTimeouts);
    
    // Return true iff both operations succeeded
    return (result1 && result2);
}

// ------------------------------------------------------------------------
// Set up the default communication parameters
// ------------------------------------------------------------------------
void vsSerialPort::setDefaults(DCB *newDCB, COMMTIMEOUTS *newTimeouts)
{
    // Set the port speed
    newDCB->BaudRate = CBR_9600;

    // Set typical communications defaults
    newDCB->ByteSize = 8;
    newDCB->Parity = NOPARITY;
    newDCB->StopBits = ONESTOPBIT;
    
    // Disable software flow control
    newDCB->fOutX = FALSE;
    newDCB->fInX = FALSE;
    
    // Disable hardware flow control
    newDCB->fOutxCtsFlow = FALSE;
    newDCB->fOutxDsrFlow = FALSE;
    
    // Set that we want manual control of RTS and DTR (i.e.: hardware flow
    // control is off)
    newDCB->fRtsControl = RTS_CONTROL_ENABLE;
    newDCB->fDtrControl = DTR_CONTROL_ENABLE;
    
    // Set timeouts so that ReadFile returns immediately even if nothing is
    // in the buffer, and WriteFile does not use timeouts.
    newTimeouts->ReadIntervalTimeout = MAXDWORD;
    newTimeouts->ReadTotalTimeoutConstant = 0;
    newTimeouts->ReadTotalTimeoutMultiplier = 0;
    newTimeouts->WriteTotalTimeoutConstant = 0;
    newTimeouts->WriteTotalTimeoutMultiplier = 0;
}

// ------------------------------------------------------------------------
// Write a packet to the port
// ------------------------------------------------------------------------
int vsSerialPort::writePacket(unsigned char *packet, int length)
{
    DWORD bytesWritten;
    
    WriteFile(portDescriptor, packet, (DWORD)length, &bytesWritten, NULL);
    
    return (int)bytesWritten;
}

// ------------------------------------------------------------------------
// Read a packet from the port
// ------------------------------------------------------------------------
int vsSerialPort::readPacket(unsigned char *packet, int length)
{
    DWORD result;
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
        ReadFile(portDescriptor, &(packet[bytesRead]), 
            (DWORD)(length - bytesRead), &result, NULL);

        // Add the number of bytes read on this loop
        if (result > 0)
            bytesRead += (int)result;

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
    int result;
    DWORD readFlag;

    // Read a single byte from the port
    ReadFile(portDescriptor, &character, 1, &readFlag, NULL);

    // Return the character read if we successfully read a byte
    // otherwise, return -1
    if (readFlag == 1)
    {
        result = character;
        return result;
    }
    else
        return -1;
}

// ------------------------------------------------------------------------
// Set the communication speed
// ------------------------------------------------------------------------
void vsSerialPort::setBaudRate(long baudRate)
{
    DWORD rate;

    // First, determine the correct flag for the desired
    // new baud rate
    switch (baudRate)
    {
        case 0:
            rate = 0;
            break;
        case 300:
            rate = CBR_300;
            break;
        case 1200:
            rate = CBR_1200;
            break;
        case 2400:
            rate = CBR_2400;
            break;
        case 4800:
            rate = CBR_4800;
            break;
        case 9600:
            rate = CBR_9600;
            break;
        case 19200:
            rate = CBR_19200;
            break;
        case 38400:
            rate = CBR_38400;
            break;
        case 57600:
            rate = CBR_57600;
            break;
        case 115200:
            rate = CBR_115200;
            break;
        default:
            rate = CBR_9600;
            break;
    }
   
    // Set the new baud rate
    currentAttributes.BaudRate = rate;

    // Change the current Attributes
    if (!setAttributes(&currentAttributes, &currentTimeouts))
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
    BYTE newParity;

    // Determine the correct flag(s) for the 
    // desired parity setting
    switch (parity)
    {
        case 'E':
            // Even parity
            newParity = EVENPARITY;
            break;

        case 'O':
            // Odd parity
            newParity = ODDPARITY;
            break;

        case 'N':
            // No parity
            newParity = NOPARITY;
            break;

        default:
            // Default:  no parity
            newParity = NOPARITY;
            break;
    };

    // Set the new parity
    currentAttributes.Parity = newParity;

    // Change the currentAttributes
    setAttributes(&currentAttributes, &currentTimeouts);
}

// ------------------------------------------------------------------------
// Set the word length
// ------------------------------------------------------------------------
void vsSerialPort::setWordLength(int wordLength)
{
    BYTE byteSize;

    // Determine the correct flag for the new
    // desired word length (number of data bits)
    switch (wordLength) 
    {
        case 5:
        case 6:
        case 7:
        case 8:
            byteSize = wordLength;
            break;
            
        default:
            byteSize = 8;
            break;
    };

    // Set the new byte size
    currentAttributes.ByteSize = byteSize;

    // Change the currentAttributes
    setAttributes(&currentAttributes, &currentTimeouts);
}

// ------------------------------------------------------------------------
// Set the number of stop bits
// ------------------------------------------------------------------------
void vsSerialPort::setStopBits(int stopBits)
{
    BYTE numStopBits;

    // If we want two stop bits, set the CSTOPB
    // flag, otherwise don't set any
    if (stopBits == 2)
        numStopBits = TWOSTOPBITS;
    else
        numStopBits = ONESTOPBIT;


    // Set the new number of stop bits
    currentAttributes.StopBits = numStopBits;

    // Change the currentAttributes
    setAttributes(&currentAttributes, &currentTimeouts);
} 

// ------------------------------------------------------------------------
// Raises (if the parameter is VS_TRUE) or lowers (if VS_FALSE) the RTS
// line on the serial port
// ------------------------------------------------------------------------
void vsSerialPort::setRTS(int enable)
{
    // Check if we're raising or dropping RTS
    if (enable)
        EscapeCommFunction(portDescriptor, SETRTS);
    else
        EscapeCommFunction(portDescriptor, CLRRTS);
}

// ------------------------------------------------------------------------
// Raises (if the parameter is VS_TRUE) or lowers (if VS_FALSE) the DTR
// line on the serial port
// ------------------------------------------------------------------------
void vsSerialPort::setDTR(int enable)
{
    // Check if we're raising or dropping DTR
    if (enable)
        EscapeCommFunction(portDescriptor, SETDTR);
    else
        EscapeCommFunction(portDescriptor, CLRDTR);
}

// ------------------------------------------------------------------------
// Flush the remaining data in the serial port
// ------------------------------------------------------------------------
void vsSerialPort::flushPort()
{
    PurgeComm(portDescriptor, PURGE_TXCLEAR | PURGE_RXCLEAR);
}
