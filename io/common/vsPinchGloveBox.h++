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
//    VESS Module:  vsPinchGloveBox.h++
//
//    Description:  Input system class supporting the Fakespace PINCH
//                  glove system
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_PINCH_GLOVE_BOX_HPP
#define VS_PINCH_GLOVE_BOX_HPP

// This vsInputSystem descendant creates a vsChordGlove input device to
// maintain its state data.  The glove box communicates via serial port.
// The PINCH system supports a timestamp feature to indicate when and for
// how long finger contacts occur.  This feature is not supported by this
// class since vsInputButton handles this in software.

#include "vsInputSystem.h++"
#include "vsSerialPort.h++"
#include "vsChordGloves.h++"

// Maximum size for data packets (command response packets may be larger)
#define VS_PG_MAX_DATA_SIZE 14

// Bytes to mark the packet type and the start/end of packets
#define VS_PG_DATA_PACKET     0x80
#define VS_PG_DATA_TS_PACKET  0x81
#define VS_PG_RESPONSE_PACKET 0x82
#define VS_PG_END_PACKET      0x8F

// Data format commands
#define VS_PG_CMD_TIMESTAMP   'T'
#define VS_PG_CMD_VERSION     'V'

// Configuration queries
#define VS_PG_CMD_CONFIG      'C'
#define VS_PG_CONFIG_LEFT     'L'
#define VS_PG_CONFIG_RIGHT    'R'
#define VS_PG_CONFIG_CPU      'P'
#define VS_PG_CONFIG_TICK     'T'

// Digit bits
#define VS_PG_THUMB_BIT       0x10
#define VS_PG_FORE_BIT        0x08
#define VS_PG_MIDDLE_BIT      0x04
#define VS_PG_RING_BIT        0x02
#define VS_PG_PINKY_BIT       0x01

class VS_IO_DLL vsPinchGloveBox : public vsInputSystem
{
protected:

    // The serial port
    vsSerialPort     *port;

    // The input device object for the gloves
    vsChordGloves    *gloves;

    // Utility function for reading packets byte-by-byte
    int              readPacket(unsigned char *buffer, int bufferSize,
                                unsigned char packetHeader);

public:

                          vsPinchGloveBox(int portNumber, long baud);
                          ~vsPinchGloveBox();

    virtual const char    *getClassName();

    vsChordGloves         *getGloves();

    virtual void          update();
};

#endif
