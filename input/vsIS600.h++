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
//    VESS Module:  vsIS600.h++
//
//    Description:  Class supporting the InterSense IS-600 Mark 2 motion
//                  tracking system. This class supports a single IS-600
//                  running over an RS-232 interface with up to
//                  VS_IS_MAX_TRACKERS receivers.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_IS600_HPP
#define VS_IS600_HPP

// This class will always use the binary mode of operation.  Because of this,
// certain ASCII-specific output options (such as the extended precision
// formats) have no meaning, and are not supported.  The space and CR/LF
// outputs are still usable.  Also, the 16-bit output options are supported
// to allow for increased I/O speed.  The output options marked "(factory
// use only)" in the manual are not supported, nor are the rotational cosines.

#include "vsSerialPort.h++"
#include "vsMotionTracker.h++"
#include "vsTrackingSystem.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"
#include "vsSharedInputData.h++"

// Maximum number of trackers in an IS-600 system
#define VS_IS_MAX_TRACKERS 32

// Packet sizes
#define VS_IS_SIZE_CMD_PACKET  99
#define VS_IS_SIZE_DATA_PACKET 4096

// Maximum number of items in an output packet 
#define VS_IS_MAX_OUTPUT_ITEMS 32

// RS-232 command set (FASTRAK compatible)
#define VS_IS_CMD_SET_ALIGNMENT    'A'
#define VS_IS_CMD_RESET_ALIGNMENT  'R'
#define VS_IS_CMD_BORESIGHT        'B'
#define VS_IS_CMD_BORESIGHT_ANGLES 'G'
#define VS_IS_CMD_UNBORESIGHT      'b'
#define VS_IS_CMD_ENABLE_MTL_COMP  'D'
#define VS_IS_CMD_DISABLE_MTL_COMP 'd'
#define VS_IS_CMD_XMTR_MOUNT_FRAME 'r'
#define VS_IS_CMD_ATTITUDE_FILTER  'v'
#define VS_IS_CMD_POSITION_FILTER  'x'
#define VS_IS_CMD_SYNC_MODE        'y'
#define VS_IS_CMD_SAVE_CONFIG      0x0B
#define VS_IS_CMD_REINIT_SYSTEM    0x19
#define VS_IS_CMD_FACTORY_DEFAULTS 'W'
#define VS_IS_CMD_CONFIG_ID        'X'
#define VS_IS_CMD_ANGULAR_ENV      'Q'
#define VS_IS_CMD_POSITIONAL_ENV   'V'
#define VS_IS_CMD_HEMISPHERE       'H'
#define VS_IS_CMD_INCREMENT        'I'
#define VS_IS_CMD_OUTPUT_LIST      'O'
#define VS_IS_CMD_ASCII_OUTPUT     'F'
#define VS_IS_CMD_BINARY_OUTPUT    'f'
#define VS_IS_CMD_SERIAL_PARAMS    'o'
#define VS_IS_CMD_START_CONTINUOUS 'C'
#define VS_IS_CMD_STOP_CONTINUOUS  'c'
#define VS_IS_CMD_PING             'P'
#define VS_IS_CMD_UNITS_INCHES     'U'
#define VS_IS_CMD_UNITS_CM         'u'
#define VS_IS_CMD_XON              0x13
#define VS_IS_CMD_XOFF             0x11
#define VS_IS_CMD_STATION_STATE    'l'
#define VS_IS_CMD_STATUS           'S'
#define VS_IS_CMD_TEST_INFO        'T'
#define VS_IS_CMD_TIP_OFFSETS      'N'
#define VS_IS_CMD_BUTTON_FUNCTION  'e'

// InterSense-specific commands

// All InterSense commands begin with 'M'
#define VS_IS_CMD_MFR_SPECIFIC     'M'

// System commands
#define VS_IS_CMD_TIME_UNITS_MILLI 'T'
#define VS_IS_CMD_TIME_UNITS_MICRO 't'
#define VS_IS_CMD_TIME_ZERO        'Z'
#define VS_IS_CMD_IS_SYS_STATUS    'S'
#define VS_IS_CMD_TRACKING_STATUS  'P'
#define VS_IS_CMD_SONIC_TIMEOUT    'U'
#define VS_IS_CMD_SONIC_SENS       'g'

// Genlock commands (all begin with 'G', 'G' can appear by itself)
#define VS_IS_CMD_GENLOCK          'G'
#define VS_IS_CMD_GENLOCK_PHASE    'P'

// Station commands
#define VS_IS_CMD_STATION_STATUS   's'
#define VS_IS_CMD_COMPASS_CORRECT  'H'
#define VS_IS_CMD_PREDICT_INTERVAL 'p'
#define VS_IS_CMD_PERCEP_ENH_LVL   'F'
#define VS_IS_CMD_ROT_SENS_LVL     'Q'

// Configuration commands (all begin with 'C')
#define VS_IS_CMD_CONFIGURE        'C'

#define VS_IS_CMD_ADD_ICUBE        'I'
#define VS_IS_CMD_DEL_ICUBE        'i'
#define VS_IS_CMD_ADD_MOBILE_PSE   'M'
#define VS_IS_CMD_DEL_MOBILE_PSE   'm'
#define VS_IS_CMD_CLEAR_STATION    'c'
#define VS_IS_CMD_ADD_FIXED_PSE    'F'
#define VS_IS_CMD_DEL_FIXED_PSE    'f'
#define VS_IS_CMD_CLEAR_CONST      'C'
#define VS_IS_CMD_APPLY_CONFIG     'e'
#define VS_IS_CMD_CANCEL_CONFIG    'x'

// Synchronization modes
#define VS_IS_SYNC_INTERNAL 0
#define VS_IS_SYNC_EXTERNAL 1
#define VS_IS_SYNC_VIDEO    2

// Output format
enum
{
    VS_IS_FORMAT_SPACE        = 0,
    VS_IS_FORMAT_CRLF         = 1,
    VS_IS_FORMAT_POSITION     = 2,
    VS_IS_FORMAT_REL_POS      = 3,
    VS_IS_FORMAT_ANGLES       = 4,
    VS_IS_FORMAT_MATRIX       = 5,
    VS_IS_FORMAT_QUAT         = 11,

    // 16-bit precision selections
    VS_IS_FORMAT_16BIT_POS    = 18,
    VS_IS_FORMAT_16BIT_ANGLES = 19,
    VS_IS_FORMAT_16BIT_QUAT   = 20
};

// Scale factors for the 16-bit formats
#define VS_IS_SCALE_POS_INCHES (float)(118.110/8192.0)
#define VS_IS_SCALE_POS_CM     (float)(300.0/8192.0)
#define VS_IS_SCALE_ANGLES     (float)(180.0/8192.0)
#define VS_IS_SCALE_QUAT       (float)(1.0/8192.0)

// Inches or centimeters
enum
{
    VS_IS_UNITS_INCHES,
    VS_IS_UNITS_CENTIMETERS
};

// Shared memory and semaphore key
#define VS_IS_SHM_KEY_BASE 0x16000000

class vsIS600 : public vsTrackingSystem
{
protected:

    // Pointer and device name for the serial port
    vsSerialPort         *port;

    // The trackers
    int                  numTrackers;
    vsMotionTracker      *tracker[VS_IS_MAX_TRACKERS];

    // Mapping from station number (one-based) to tracker number (zero-based)
    int                  station[VS_IS_MAX_TRACKERS + 1];

    // Shared memory object and state variables
    vsSharedInputData    *sharedData;
    int                  forked;
    int                  serverPID;
    static int           serverDone;

    // Array representing the current output format
    int                  formatArray[VS_IS_MAX_OUTPUT_ITEMS];
    int                  formatNum;

    // Size of the output record (for each tracker)
    int                  outputSize;

    // Indicates whether or not we're streaming data 
    int                  streaming;

    // Units for positional output
    int                  outputUnits;

    // Coordinate conversion quaternion
    vsQuat               coordXform;

    // Utility functions
    void                 enumerateTrackers();
    void                 initOutputFormat();
    void                 endianSwap(float *inFloat, float *outFloat);
    void                 setBinaryOutput();

    // Internal update functions
    void                 updatePosition(int trackerNum, vsVector posVec);
    void                 updateRelativePosition(int trackerNum, 
                                                vsVector posVec);
    void                 updateAngles(int trackerNum, vsVector orientationVec);
    void                 updateMatrix(int trackerNum, vsMatrix orientationMat);
    void                 updateQuat(int trackerNum, vsQuat orientationQuat);

    // Multi-process functions
    void                 serverLoop();
    static void          quitServer(int arg);

    // Measurement functions
    void                 ping();
    void                 updateSystem();

public:

                               vsIS600(int portNumber, long baud, 
                                         int nTrackers);
    virtual                    ~vsIS600();

    // IS-600 stream control functions
    void                       startStream();
    void                       stopStream();

    // IS-600 configuration functions
    void                       clearStation(int stationNum);
    void                       clearConstellation();
    void                       addInertiaCube(int stationNum, int cubeNum);
    void                       removeInertiaCube(int stationNum, int cubeNum);
    void                       addSoniDisc(int stationNum, int discNum,
                                           vsVector pos, vsVector normal,
                                           int discID);
    void                       removeSoniDisc(int stationNum, int discNum,
                                              int discID);
    void                       addReceiverPod(int podNum, vsVector pos,
                                              vsVector normal, int podID);
    void                       removeReceiverPod(int podNum, int podID);
    void                       applyConfig();
    void                       cancelConfig();

    // Function to split the IS-600 operation into two separate processes
    void                       forkTracking();

    // Other useful IS-600 functions
    void                       setAlignment(int station, vsVector origin, 
                                            vsVector positiveX, 
                                            vsVector positiveY);
    void                       resetAlignment(int station);
    void                       setMountingFrame(int station, 
                                   vsVector orientation);
    void                       setGenlock(int syncMode, int rate);
    void                       setGenlockPhase(int phase);
    void                       setOutputFormat(int newFormat[], 
                                               int newFormatNum);
    void                       setUnits(int units);

    // Tracker functions
    virtual int                getNumTrackers();
    virtual vsMotionTracker    *getTracker(int index);

    virtual void               update();
};

#endif
