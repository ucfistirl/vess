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
#include "vsObject.h++"

#define VS_SERIAL_NUM_READ_RETRYS  320000

class vsSerialPort : public vsObject
{
private:

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
    virtual      ~vsSerialPort(void);
      
    virtual const char    *getClassName();

    int          writePacket(unsigned char *string, int length);
    int          readPacket(unsigned char *string, int length);

    int          readCharacter();

    bool         isDataWaiting();
    bool         isDataWaiting(double secondsToWait);

    void         setBaudRate(long baudRate);
    void         setParity(char parity);
    void         setWordLength(int wordLength);
    void         setStopBits(int stopBits);
    void         setRTS(bool enable);
    void         setDTR(bool enable);

    void         sendBreakSignal();

    void         flushPort(void);
};

#endif
