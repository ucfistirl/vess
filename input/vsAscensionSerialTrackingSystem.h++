#ifndef VS_ASCENSION_SERIAL_TRACKING_SYSTEM_HPP
#define VS_ASCENSION_SERIAL_TRACKING_SYSTEM_HPP

// Base class to handle input from Ascension serial-based Motion Tracking 
// systems that use Ascension's RS-232 command set.  This includes the Flock 
// of Birds and MotionStar systems.  Do not instantiate this class directly.
// Instead, use the vsFlockOfBirds or vsSerialMotionStar classes.
//
// This class supports both Standalone and Flock modes for the Flock of Birds
// and single- or multiple-chassis configurations for the MotionStar, using 
// an RS-232 interface either to one bird or to all of the birds.  
// NOTE: RS-485 interfaces are not supported.
//
// This class was written to support Flock of Birds devices with PROM
// revisions 3.57 and later.  Any addressing mode (normal, expanded, and 
// super-expanded) is supported, allowing up to 126 FBB devices.
//
// This class also does not support the buttons on FOB 6D Mouse devices.
// (The position tracking features are supported).

#include "vsSerialPort.h++"
#include "vsMotionTracker.h++"
#include "vsTrackingSystem.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"
#include "vsSharedInputData.h++"

// Maximum number of receivers when in Standard, Expanded, and
// Super-Expanded addressing modes
#define VS_AS_MAX_STD_TRACKERS 14
#define VS_AS_MAX_EXP_TRACKERS 30
#define VS_AS_MAX_SUP_TRACKERS 126
#define VS_AS_MAX_TRACKERS     VS_AS_MAX_SUP_TRACKERS

// Packet sizes
#define VS_AS_DATA_POSITION_SIZE   6
#define VS_AS_DATA_ANGLES_SIZE     6
#define VS_AS_DATA_MATRIX_SIZE     18
#define VS_AS_DATA_QUATERNION_SIZE 8
#define VS_AS_DATA_POS_ANGLES_SIZE 12
#define VS_AS_DATA_POS_MATRIX_SIZE 24
#define VS_AS_DATA_POS_QUAT_SIZE   14
#define VS_AS_DATA_PACKET_SIZE     3024
#define VS_AS_CMD_PACKET_SIZE      200

// RS-232 command set
#define VS_AS_CMD_ANGLES           0x57
#define VS_AS_CMD_ANGLE_ALIGN1     0x4A
#define VS_AS_CMD_ANGLE_ALIGN2     0x71
#define VS_AS_CMD_BUTTON_MODE      0x4D
#define VS_AS_CMD_BUTTON_READ      0x4E
#define VS_AS_CMD_CHANGE_VALUE     0x50
#define VS_AS_CMD_EXAMINE_VALUE    0x4F
#define VS_AS_CMD_FACTORY_TEST     0x7A
#define VS_AS_CMD_HEMISPHERE       0x4C
#define VS_AS_CMD_MATRIX           0x58
#define VS_AS_CMD_NEXT_XMTR        0x30
#define VS_AS_CMD_POINT            0x42
#define VS_AS_CMD_POSITION         0x56
#define VS_AS_CMD_POS_ANGLES       0x59
#define VS_AS_CMD_POS_MATRIX       0x5A
#define VS_AS_CMD_POS_QUAT         0x5D
#define VS_AS_CMD_QUATERNION       0x5C
#define VS_AS_CMD_REF_FRAME1       0x48
#define VS_AS_CMD_REF_FRAME2       0x72   
#define VS_AS_CMD_REPORT_RATE1     0x51
#define VS_AS_CMD_REPORT_RATE2     0x52
#define VS_AS_CMD_REPORT_RATE8     0x53
#define VS_AS_CMD_REPORT_RATE32    0x54
#define VS_AS_CMD_RS232_TO_FBB_STD 0xF0
#define VS_AS_CMD_RS232_TO_FBB_EXP 0xE0
#define VS_AS_CMD_RS232_TO_FBB_SUP 0xA0
#define VS_AS_CMD_RUN              0x46
#define VS_AS_CMD_SLEEP            0x47
#define VS_AS_CMD_STREAM           0x40
#define VS_AS_CMD_SYNC             0x41
#define VS_AS_CMD_XON              0x11
#define VS_AS_CMD_XOFF             0x13

// Scale factors
#define VS_AS_SCALE_ERT_POS  (float)(144.0/32768.0)
#define VS_AS_SCALE_SRT1_POS (float)(36.0/32768.0)
#define VS_AS_SCALE_SRT2_POS (float)(72.0/32768.0)
#define VS_AS_SCALE_ANGLE    (float)(180.0/32768.0)
#define VS_AS_SCALE_MATRIX   (float)(1.0/32768.0)
#define VS_AS_SCALE_QUAT     (float)(1.0/32768.0)

// Special address/index to indicate all trackers
#define VS_AS_ALL_TRACKERS 0xFF

// RS-485 (FBB) command set
enum
{
    VS_AS_FBB_RS232CMD    = 0,
    VS_AS_FBB_SEND_DATA   = 1,
    VS_AS_FBB_SEND_STATUS = 2,
    VS_AS_FBB_SEND_ERROR  = 3
};

// Change/Examine Value command parameters
enum
{
    VS_AS_VAL_BIRD_STATUS        = 0x00,
    VS_AS_VAL_SW_REV             = 0x01,
    VS_AS_VAL_CRYSTAL_SPEED      = 0x02,
    VS_AS_VAL_POS_SCALING        = 0x03,
    VS_AS_VAL_FILTER_ENABLE      = 0x04,
    VS_AS_VAL_FILTER_ALPHA_MIN   = 0x05,
    VS_AS_VAL_MEASURE_RATE_COUNT = 0x06,
    VS_AS_VAL_MEASURE_RATE       = 0x07,
    VS_AS_VAL_SEND_DATA_READY    = 0x08,
    VS_AS_VAL_DATA_READY_CHAR    = 0x09,
    VS_AS_VAL_ERROR_CODE         = 0x0A,
    VS_AS_VAL_ERROR_DETECT_MASK  = 0x0B,
    VS_AS_VAL_FILTER_VM          = 0x0C,
    VS_AS_VAL_FILTER_ALPHA_MAX   = 0x0D,
    VS_AS_VAL_SUDDEN_CHANGE_LOCK = 0x0E,
    VS_AS_VAL_SYSTEM_MODEL_ID    = 0x0F,
    VS_AS_VAL_EXP_ERROR_CODE     = 0x10,
    VS_AS_VAL_XYZ_REF_FRAME      = 0x11,
    VS_AS_VAL_XMTR_OP_MODE       = 0x12,
    VS_AS_VAL_ADDRESS_MODE       = 0x13,
    VS_AS_VAL_LINE_FREQUENCY     = 0x14,
    VS_AS_VAL_FBB_ADDRESS        = 0x15,
    VS_AS_VAL_HEMISPHERE         = 0x16,
    VS_AS_VAL_ANGLE_ALIGN_2      = 0x17,
    VS_AS_VAL_REF_FRAME_2        = 0x18,
    VS_AS_VAL_SERIAL_NUMBER      = 0x19,
    VS_AS_VAL_FBB_HOST_DELAY     = 0x20,
    VS_AS_VAL_GROUP_MODE         = 0x23,
    VS_AS_VAL_FLOCK_STATUS       = 0x24,
    VS_AS_VAL_FBB_AUTOCONFIG     = 0x32
};

// Configuration mode values
enum
{
    VS_AS_MODE_FLOCK      = 0,
    VS_AS_MODE_STANDALONE = 1
};

// Address mode values
enum
{
    VS_AS_ADDR_STANDARD = 0,
    VS_AS_ADDR_EXPANDED = 1,
    VS_AS_ADDR_SUPER_EXP = 3
};

// Data format values
enum
{
    VS_AS_DATA_POSITION   = 0,
    VS_AS_DATA_ANGLES     = 1,
    VS_AS_DATA_MATRIX     = 2,
    VS_AS_DATA_QUATERNION = 3,
    VS_AS_DATA_POS_ANGLES = 4,
    VS_AS_DATA_POS_MATRIX = 5,
    VS_AS_DATA_POS_QUAT   = 6
};

// Hemispheres
enum
{
    VS_AS_HSPH_FORWARD = 0x0000,
    VS_AS_HSPH_AFT     = 0x0001,
    VS_AS_HSPH_UPPER   = 0x0C01,
    VS_AS_HSPH_LOWER   = 0x0C00,
    VS_AS_HSPH_LEFT    = 0x0601,
    VS_AS_HSPH_RIGHT   = 0x0600
};

// Synchronization modes
enum
{
    VS_AS_SYNC_NONE = 0,
    VS_AS_SYNC_1X   = 1,
    VS_AS_SYNC_2X   = 2
};

// Shared memory key
#define VS_AS_SHM_KEY_BASE 0xAF0B0000


class vsAscensionSerialTrackingSystem : public vsTrackingSystem
{
protected:
   
    // Flag to indicate whether multiple serial ports are being used
    int                  multiSerial;

    // Pointer name for the serial port object(s)
    vsSerialPort         *port[VS_AS_MAX_TRACKERS];

    // Motion trackers
    int                  numTrackers;
    vsMotionTracker      *tracker[VS_AS_MAX_TRACKERS];

    // Shared memory object and state variables
    vsSharedInputData    *sharedData;
    int                  forked;
    int                  serverPID;
    static int           serverDone;

    // Flock parameters
    int                  configuration;
    int                  addressMode;
    int                  ercAddress;
    int                  dataFormat;
    int                  streaming;
    double               posScale;
    int                  ornScale;

    // Size of the data packet per bird and total data size during update
    int                  birdDataSize;
    int                  dataSize;

    // String to hold an error message
    char                 errorString[80];

    // Conversion from tracker to VESS coordinates
    vsQuat               coordXform;

    // Functions for multi-process operation
    void                 serverLoop();
    static void          quitServer(int arg);

    // Utility functions
    void                 enumerateTrackers();
    int                  initializeFlock();
    void                 getErrorString(unsigned char errorNum,
                             unsigned char errorAddr);
    void                 fbbCommand(int address, unsigned char command,
                                    unsigned char data[], int dataSize);

    // Functions to groom data for each data format
    void                 updatePosition(int trackerIndex, short flockData[]);
    void                 updateAngles(int trackerIndex, short flockData[]);
    void                 updateMatrix(int trackerIndex, short flockData[]);
    void                 updateQuaternion(int trackerIndex, short flockData[]);
    void                 updatePosAngles(int trackerIndex, short flockData[]);
    void                 updatePosMatrix(int trackerIndex, short flockData[]);
    void                 updatePosQuat(int trackerIndex, short flockData[]);

    // Function to update the data from the tracking hardware
    void                 updateSystem();
    
public:

                               vsAscensionSerialTrackingSystem(int portNumber, 
                                     int nTrackers, int dataFormat, long baud, 
                                     int mode);
                               vsAscensionSerialTrackingSystem(
                                   int portNumbers[], int nTrackers,
                                   int dataFormat, long baud);
    virtual                    ~vsAscensionSerialTrackingSystem();

    // Function to split the TrackingSystem into two separate processes
    void                       forkTracking();

    // Measurement functions
    void                       ping();
    void                       startStream();

    // Other useful FOB functions
    void                       setDataFormat(int format);
    void                       setActiveHemisphere(int trackerNum, 
                                                   short hSphere);
    void                       setReferenceFrame(float h, float p, float r);
    void                       setAngleAlignment(int trackerNum, float h,
                                                 float p, float r);
    void                       sleepFlock();
    void                       runFlock();
    void                       setSyncMode(int syncType);
    void                       setTransmitter(int address, int number);

    // Tracker functions
    virtual int                getNumTrackers();
    virtual vsMotionTracker    *getTracker(int index);

    virtual void               update();
};

#endif
