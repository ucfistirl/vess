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
//    VESS Module:  vsSerialPort.h++
//
//    Description:  Class for handling serial port communications
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_SERIAL_PORT_HPP
#define VS_SERIAL_PORT_HPP

#include <termios.h>
#include <unistd.h>

#define VS_SERIAL_NUM_READ_RETRYS  32000

class vsSerialPort
{
private:

    int               portNumber;
   
    int               portDescriptor;
    struct termios    oldAttributes;
    struct termios    currentAttributes;
      
    int               setAttributes(termios *);
    void              setDefaults(termios *);
    void              termioPrint(termios *);
      
public:

                 vsSerialPort(char *deviceName);
                 vsSerialPort(char *deviceName, long baud, int wordLength,
                              char parity, int stopBits);
                 ~vsSerialPort(void);
      
    int          writePacket(unsigned char *string, int length);
    int          readPacket(unsigned char *string, int length);

    int          readCharacter();

    void         setBaudRate(long baudRate);
    void         setParity(char parity);
    void         setWordLength(int wordLength);
    void         setStopBits(int stopBits);
    void         setRTS(int enable);
    void         setDTR(int enable);

    void         flushPort(void);
};

#endif
