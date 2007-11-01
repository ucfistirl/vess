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
//    VESS Module:  vsFastrak.h++
//
//    Description:  Class supporting the Polhemus FASTRAK motion tracking
//                  system.  This class supports a single FASTRAK running
//                  over an RS-232 interface with up to VS_FT_MAX_TRACKERS
//                  receivers.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_FASTRAK_HPP
#define VS_FASTRAK_HPP

// This class will always use the binary mode of operation.  Because of this,
// certain ASCII-specific output options (such as the extended precision
// formats) have no meaning, and are not supported.  The space and CR/LF
// outputs are still usable.  Also, the 16-bit output options are supported
// to allow for increased I/O speed.  The output options marked "(factory
// use only)" in the manual are not supported, nor are the rotational cosines.
//
// Multiple FASTRAK's can be used by creating multiple instances of this 
// class and issuing the appropriate commands as detailed in section 6.5
// of the FASTRAK manual.  Note that each FASTRAK system must be given a
// different carrier frequency for this.  Changing carrier frequencies is
// done in hardware.
//
// The Stylus and 3Ball optional accessories are not supported by this class.

#include "vsSerialPort.h++"
#include "vsMotionTracker.h++"
#include "vsTrackingSystem.h++"
#include "atVector.h++"
#include "atMatrix.h++"
#include "atQuat.h++"

// Maximum number of trackers in a FASTRAK system
#define VS_FT_MAX_TRACKERS 4

// Packet sizes
#define VS_FT_SIZE_CMD_PACKET  99
#define VS_FT_SIZE_DATA_PACKET 1024

// Maximum number of items in an output packet 
#define VS_FT_MAX_OUTPUT_ITEMS 32

// RS-232 command set
#define VS_FT_CMD_SET_ALIGNMENT    'A'
#define VS_FT_CMD_RESET_ALIGNMENT  'R'
#define VS_FT_CMD_BORESIGHT        'B'
#define VS_FT_CMD_BORESIGHT_ANGLES 'G'
#define VS_FT_CMD_UNBORESIGHT      'b'
#define VS_FT_CMD_ENABLE_MTL_COMP  'D'
#define VS_FT_CMD_DISABLE_MTL_COMP 'd'
#define VS_FT_CMD_XMTR_MOUNT_FRAME 'r'
#define VS_FT_CMD_ATTITUDE_FILTER  'v'
#define VS_FT_CMD_POSITION_FILTER  'x'
#define VS_FT_CMD_SYNC_MODE        'y'
#define VS_FT_CMD_SAVE_CONFIG      0x0B
#define VS_FT_CMD_REINIT_SYSTEM    0x19
#define VS_FT_CMD_FACTORY_DEFAULTS 'W'
#define VS_FT_CMD_CONFIG_ID        'X'
#define VS_FT_CMD_ANGULAR_ENV      'Q'
#define VS_FT_CMD_POSITIONAL_ENV   'V'
#define VS_FT_CMD_HEMISPHERE       'H'
#define VS_FT_CMD_INCREMENT        'I'
#define VS_FT_CMD_OUTPUT_LIST      'O'
#define VS_FT_CMD_ASCII_OUTPUT     'F'
#define VS_FT_CMD_BINARY_OUTPUT    'f'
#define VS_FT_CMD_SERIAL_PARAMS    'o'
#define VS_FT_CMD_START_CONTINUOUS 'C'
#define VS_FT_CMD_STOP_CONTINUOUS  'c'
#define VS_FT_CMD_PING             'P'
#define VS_FT_CMD_UNITS_INCHES     'U'
#define VS_FT_CMD_UNITS_CM         'u'
#define VS_FT_CMD_XON              0x13
#define VS_FT_CMD_XOFF             0x11
#define VS_FT_CMD_STATION_STATE    'l'
#define VS_FT_CMD_STATUS           'S'
#define VS_FT_CMD_TEST_INFO        'T'
#define VS_FT_CMD_TIP_OFFSETS      'N'
#define VS_FT_CMD_BUTTON_FUNCTION  'e'

// Synchronization modes
#define VS_FT_SYNC_INTERNAL 0
#define VS_FT_SYNC_EXTERNAL 1
#define VS_FT_SYNC_VIDEO    2

// Output format
enum 
{
    VS_FT_FORMAT_SPACE        = 0,
    VS_FT_FORMAT_CRLF         = 1,
    VS_FT_FORMAT_POSITION     = 2,
    VS_FT_FORMAT_REL_POS      = 3,
    VS_FT_FORMAT_ANGLES       = 4,
    VS_FT_FORMAT_MATRIX       = 5,
    VS_FT_FORMAT_QUAT         = 11,

    // 16-bit precision selections
    VS_FT_FORMAT_16BIT_POS    = 18,
    VS_FT_FORMAT_16BIT_ANGLES = 19,
    VS_FT_FORMAT_16BIT_QUAT   = 20
};

// Scale factors for the 16-bit formats
#define VS_FT_SCALE_POS_INCHES (float)(118.110/8192.0)
#define VS_FT_SCALE_POS_CM     (float)(300.0/8192.0)
#define VS_FT_SCALE_ANGLES     (float)(180.0/8192.0)
#define VS_FT_SCALE_QUAT       (float)(1.0/8192.0)

// Inches or centimeters
enum 
{
    VS_FT_UNITS_INCHES,
    VS_FT_UNITS_CENTIMETERS
};

// Shared memory and semaphore key
#define VS_FT_SHM_KEY_BASE 0xfa570000

class VS_IO_DLL vsFastrak : public vsTrackingSystem
{
protected:

    // Pointer and device name for the serial port
    vsSerialPort           *port;

    // The trackers
    int                  numTrackers;
    vsMotionTracker        *tracker[VS_FT_MAX_TRACKERS];

    // Mapping from station number (one-based) to tracker number (zero-based)
    int                    station[VS_FT_MAX_TRACKERS + 1];

    // For multithreading, private copy of tracker data, thread 
    // state variables, and synchronization structures
    vsMotionTracker        *privateTracker[VS_FT_MAX_TRACKERS];
    int                    forked;
    HANDLE                 serverThread;
    DWORD                  serverThreadID;
    int                    serverDone;
    CRITICAL_SECTION       criticalSection;

    // Array representing the current output format
    int                    formatArray[VS_FT_MAX_OUTPUT_ITEMS];
    int                    formatNum;

    // Indicates whether or not this machine is big-endian
    bool                   bigEndian;

    // Size of the output record (for each tracker)
    int                    outputSize;

    // Indicates whether or not we're streaming data 
    bool                   streaming;

    // Units for positional output
    int                    outputUnits;

    // Coordinate conversion quaternion
    atQuat                 coordXform;

    // Utility functions
    void                   enumerateTrackers();
    void                   initOutputFormat();
    bool                   isBigEndian();
    void                   endianSwap(float *inFloat, float *outFloat);
    void                   setBinaryOutput();

    // Internal update functions
    void                   updatePosition(int trackerNum, atVector posVec);
    void                   updateRelativePosition(int trackerNum, 
                                                  atVector posVec);
    void                   updateAngles(int trackerNum, 
                                        atVector orientationVec);
    void                   updateMatrix(int trackerNum, 
                                        atMatrix orientationMat);
    void                   updateQuat(int trackerNum, atQuat orientationQuat);

    // Multi-threading functions
    static DWORD WINAPI    serverLoop(void *parameter);

    // Measurement functions
    void                   ping();
    void                   updateSystem();

public:

                               vsFastrak(int portNumber, long baud, 
                                         int nTrackers);
    virtual                    ~vsFastrak();

    // Inherited from vsObject
    virtual const char         *getClassName();

    // FASTRAK measurement functions
    void                       startStream();
    void                       stopStream();

    // Function to split the FASTRAK operation into two separate processes
    void                       forkTracking();

    // Other useful FASTRAK functions
    void                       setAlignment(int station, atVector origin, 
                                            atVector positiveX, 
                                            atVector positiveY);
    void                       resetAlignment(int station);
    void                       setMountingFrame(int station, 
                                   atVector orientation);
    void                       setSyncMode(int syncMode);
    void                       setActiveHemisphere(int station, 
                                                   atVector zenithVec);
    void                       setOutputFormat(int newFormat[], 
                                               int newFormatNum);
    void                       setBaudRate(long baud);
    void                       setUnits(int units);

    // Tracker functions
    virtual int                getNumTrackers();
    virtual vsMotionTracker    *getTracker(int index);

    virtual void               update();
};

#endif
