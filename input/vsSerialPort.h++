#ifndef VS_SERIAL_PORT_HPP
#define VS_SERIAL_PORT_HPP

// Class for handling serial port communications

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

    void         flushPort(void);
};

#endif
