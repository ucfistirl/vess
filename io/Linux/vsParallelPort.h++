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
//    VESS Module:  vsParallelPort.h++
//
//    Description:  Support for the Parallel Port under Linux
//                  Note: this requires a 2.4.x version of Linux
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#ifndef VS_PARALLEL_PORT_HPP
#define VS_PARALLEL_PORT_HPP

#include "vsObject.h++"

// Parallel Port Communications Modes
// These are defined as part of the IEEE1284 Standard
enum vsParallelPortMode
{
    VS_PARALLEL_PORT_MODE_COMPATIBILITY,
    VS_PARALLEL_PORT_MODE_NIBBLE,
    VS_PARALLEL_PORT_MODE_BYTE,
    VS_PARALLEL_PORT_MODE_EPP,
    VS_PARALLEL_PORT_MODE_ECP,
};

// Parallel Port Control Lines
enum 
{
    VS_PARALLEL_PORT_CONTROL_STROBE = 0x1,
    VS_PARALLEL_PORT_CONTROL_AUTOFD = 0x2,
    VS_PARALLEL_PORT_CONTROL_INIT   = 0x4,
    VS_PARALLEL_PORT_CONTROL_SELECT = 0x8,
};

// Parallel Port Status Lines
enum
{
    VS_PARALLEL_PORT_STATUS_ERROR   = 0x08,
    VS_PARALLEL_PORT_STATUS_SELECT  = 0x10,
    VS_PARALLEL_PORT_STATUS_PAPEROUT= 0x20,
    VS_PARALLEL_PORT_STATUS_ACK     = 0x40,
    VS_PARALLEL_PORT_STATUS_BUSY    = 0x80,
};

class vsParallelPort : public vsObject
{
    private:
        // The file descriptor of the port
        int                 portDescriptor;

        // Indicate if the port is open or not (it might not be open if an
        // error occurred)
        bool                portOpen;

        // What communications mode is the port in?
        vsParallelPortMode  portMode;

    public:
                        vsParallelPort(const char * deviceName);
                        vsParallelPort(const char * deviceName,
                                vsParallelPortMode newPortMode);

        virtual         ~vsParallelPort(void);

        const char *    getClassName();

        bool            isPortOpen();

        // Write/Read data from the port using the current communications
        // mode
        int             writePacket(const unsigned char * string, int length);
        int             readPacket(unsigned char * string, int length);

        int             setMode(vsParallelPortMode newPortMode);

        // Set the direction of data on the data lines
        // true  - data goes from pc to peripheral
        // false - data comes from peripheral to pc
        void            setDataDirection( bool isForward );

        // Set/Get the state of the data pins on the port
        void            setDataLines( unsigned char dataByte );
        unsigned char   getDataLines( );

        int             getStatusLines( );

        void            setControlLines( int controlLines );

        // Set the timeout value for readPacket/writePacket
        void            setTimeout( double timeoutInSeconds );
};

#endif
