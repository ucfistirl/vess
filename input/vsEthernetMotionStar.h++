#ifndef VS_ETHERNET_MOTION_STAR_HPP
#define VS_ETHERNET_MOTION_STAR_HPP

// Class to handle input from an Ascension MotionStar motion capture
// system.  This class supports the Wired and Wireless versions of the
// MotionStar running with the Ethernet option.
//
// This class does not yet support multiple chassis configurations. 
// Currently, it assumes a single chassis and uses UDP communication.
//
// MotionStar systems using serial ports are not supported by this class, 
// use vsSerialMotionStar instead.

#include "vsMotionTracker.h++"
#include "vsTrackingSystem.h++"
#include "vsVector.h++"
#include "vsMatrix.h++"
#include "vsQuat.h++"
#include "vsSharedInputData.h++"

// Temporary
#include "UDPUnicastNetworkInterface.h++"

// Maximum number of receivers
#define VS_MSTAR_MAX_TRACKERS     126

// Maximum number of MotionStar servers
#define VS_MSTAR_MAX_SERVERS      1

// Special tracker index to include all trackers
#define VS_MSTAR_ALL_TRACKERS     255

// Packet sizes
#define VS_MSTAR_DATA_POSITION_SIZE   6
#define VS_MSTAR_DATA_ANGLES_SIZE     6
#define VS_MSTAR_DATA_MATRIX_SIZE     18
#define VS_MSTAR_DATA_QUATERNION_SIZE 8
#define VS_MSTAR_DATA_POS_ANGLES_SIZE 12
#define VS_MSTAR_DATA_POS_MATRIX_SIZE 24
#define VS_MSTAR_DATA_POS_QUAT_SIZE   14
#define VS_MSTAR_DATA_PACKET_SIZE     3072
#define VS_MSTAR_CMD_PACKET_SIZE      200

// Address mode values
enum
{
    VS_MSTAR_ADDR_STANDARD = 0,
    VS_MSTAR_ADDR_EXPANDED = 1,
    VS_MSTAR_ADDR_SUPER_EXP = 3
};

// Scale factors
#define VS_MSTAR_SCALE_ERT_POS     (float)(144.0/32768.0)
#define VS_MSTAR_SCALE_SRT1_POS    (float)(36.0/32768.0)
#define VS_MSTAR_SCALE_SRT2_POS    (float)(72.0/32768.0)
#define VS_MSTAR_SCALE_DEFAULT_POS VS_MSTAR_SCALE_ERT_POS
#define VS_MSTAR_SCALE_ANGLE       (float)(180.0/32768.0)
#define VS_MSTAR_SCALE_MATRIX      (float)(1.0/32768.0)
#define VS_MSTAR_SCALE_QUAT        (float)(1.0/32768.0)


// Birdnet protocol v3.00b -------------------------------------------------

// Protocol revision

#define VS_BN_PROTOCOL_VERSION 3

// Birdnet packet types

#define VS_BN_MSG_WAKE_UP        10
#define VS_BN_RSP_WAKE_UP        20
#define VS_BN_MSG_SHUT_DOWN      11
#define VS_BN_RSP_SHUT_DOWN      21
#define VS_BN_MSG_GET_STATUS     101
#define VS_BN_MSG_SEND_SETUP     102
#define VS_BN_MSG_SINGLE_SHOT    103
#define VS_BN_MSG_RUN_CONTINUOUS 104
#define VS_BN_MSG_STOP_DATA      105
#define VS_BN_MSG_SEND_DATA      106
#define VS_BN_RSP_GET_STATUS     201
#define VS_BN_RSP_SEND_SETUP     202
#define VS_BN_RSP_SINGLE_SHOT    203
#define VS_BN_RSP_RUN_CONTINUOUS 204
#define VS_BN_RSP_STOP_DATA      205
#define VS_BN_RSP_SEND_DATA      206
#define VS_BN_DATA_PACKET_MULTI  210
#define VS_BN_DATA_PACKET_ACK    211
#define VS_BN_DATA_PACKET_SINGLE 212
#define VS_BN_RSP_ILLEGAL        40
#define VS_BN_RSP_UNKNOWN        50
#define VS_BN_MSG_SYNC_SEQUENCE  30
#define VS_BN_RSP_SYNC_SEQUENCE  31


// System status bit flags

#define VS_BN_SYSTEM_RUNNING        0x80
#define VS_BN_SYSTEM_ERROR          0x40
#define VS_BN_SYSTEM_FBB_ERROR      0x20
#define VS_BN_SYSTEM_LOCAL_ERROR    0x10
#define VS_BN_SYSTEM_LOCAL_POWER    0x08
#define VS_BN_SYSTEM_MASTER         0x04
#define VS_BN_SYSTEM_CRTSYNC_TYPE   0x02
#define VS_BN_SYSTEM_CRTSYNC        0x01

// Bird status bit flags

#define VS_BN_FLOCK_ERROR              0x80
#define VS_BN_FLOCK_RUNNING            0x40
#define VS_BN_FLOCK_BUTTONSPRESENT     0x08
#define VS_BN_FLOCK_RECEIVERPRESENT    0x04
#define VS_BN_FLOCK_TRANSMITTERPRESENT 0x02
#define VS_BN_FLOCK_TRANSMITTERRUNNING 0x01

// Bird type ID

#define VS_BN_STANDALONE_BIRD 1
#define VS_BN_ERC             2
#define VS_BN_OLD_MOTIONSTAR  3
#define VS_BN_PC_BIRD         4
#define VS_BN_SPACE_PAD       5
#define VS_BN_MOTIONSTAR      6
#define VS_BN_WIRELESS        7
#define VS_BN_UNRECOGNIZED    255

// Bird setup byte bit flags

#define VS_BN_FLOCK_SUDDENOUTPUTCHANGE  0x20
#define VS_BN_FLOCK_XYZREFERENCE        0x10
#define VS_BN_FLOCK_APPENDBUTTONDATA    0x08
#define VS_BN_FLOCK_ACNARROWNOTCHFILTER 0x04
#define VS_BN_FLOCK_ACWIDENOTCHFILTER   0x02
#define VS_BN_FLOCK_DCFILTER            0x01

// Bird data format
// These are in the lower 4 bits of the dataFormat record of the status
// The size of the data packet is in the upper 4 bits of the same field

#define VS_BN_FLOCK_NOBIRDDATA         0
#define VS_BN_FLOCK_POSITION           1
#define VS_BN_FLOCK_ANGLES             2
#define VS_BN_FLOCK_MATRIX             3
#define VS_BN_FLOCK_POSITIONANGLES     4
#define VS_BN_FLOCK_POSITIONMATRIX     5
#define VS_BN_FLOCK_QUATERNION         7
#define VS_BN_FLOCK_POSITIONQUATERNION 8

// Bird hemisphere

#define VS_BN_FRONT_HEMISHPERE 0
#define VS_BN_REAR_HEMISPHERE  1
#define VS_BN_UPPER_HEMISPHERE 2
#define VS_BN_LOWER_HEMISPHERE 3
#define VS_BN_LEFT_HEMISPHERE  4
#define VS_BN_RIGHT_HEMISPHERE 5

// Transmitter type bit flags

#define VS_BN_XMTR_ERT    0x80
#define VS_BN_XMTR_SRT    0x40
#define VS_BN_XMTR_PCBIRD 0x20
#define VS_BN_XMTR_ACTIVE 0x10

// Packet header error codes

#define VS_BN_NO_PACKET_ERROR                0
#define VS_BN_SINGLE_PACKET_SEQUENCE_ERROR   1
#define VS_BN_TWO_PACKET_SEQUENCE_ERROR      2
#define VS_BN_LARGE_PACKET_SEQUENCE_ERROR    3
#define VS_BN_REPEATED_PACKET_SEQUENCE_ERROR 4
#define VS_BN_UNEXPECTED_PACKET_ERROR        6
#define VS_BN_BAD_PACKET_ERROR               7
#define VS_BN_ILLEGAL_STATUS_REQUEST         8
#define VS_BN_ILLEGAL_SETUP_ERROR            9
#define VS_BN_SYSTEM_NOT_READY               100

// Shared memory and semaphore key
#define VS_MSTAR_SHM_KEY_BASE 0x57AA0000

// Birdnet-related structures

typedef struct
{
   unsigned short int    sequence;
   unsigned short int    milliseconds;
   unsigned long         time;

   unsigned char         type;
   unsigned char         xtype;
   unsigned char         protocol;
   
   unsigned char         errorCode;
   unsigned short int    extErrorCode;
   
   unsigned short int    numBytes;
}  vsBirdnetHeader;

typedef struct
{
   vsBirdnetHeader    header;
   char               buffer[64984];
}  vsBirdnetPacket;

typedef struct
{
   unsigned char    all;
   unsigned char    error;

   unsigned char    flockNumber;
   unsigned char    serverNumber;
   unsigned char    transmitterNumber;

   unsigned char    measurementRate[6];

   unsigned char    chassisNumber;
   unsigned char    chassisDevices;
   unsigned char    firstAddress;

   unsigned char    softwareRevision[2];
   
   unsigned char    fbbStatus[126];
}  vsBirdnetSystemStatusPacket;

typedef struct
{
   unsigned char         status;
   unsigned char         id;
   unsigned short int    softwareRevision;
   unsigned char         errorCode;
   unsigned char         setup;
   unsigned char         dataFormat;
   unsigned char         reportRate;
   unsigned short int    scaling;
   unsigned char         hemisphere;
   unsigned char         FBBaddress;
   unsigned char         transmitterType;
   unsigned char         spare1;
   unsigned short int    spare2;
}  vsBirdnetBirdStatusPacket;

//  Ranges in Inches from Transmitter
typedef struct
{
   unsigned short int    Range0to55;
   unsigned short int    Range55to70;
   unsigned short int    Range70to90;
   unsigned short int    Range90to110;
   unsigned short int    Range110to138;
   unsigned short int    Range138to170;
   unsigned short int    Range170;
}  vsBirdnetFilterTablePacket;

typedef struct
{
   unsigned short int    azimuth;
   unsigned short int    elevation;
   unsigned short int    roll;
}  vsBirdnetRefAlignmentPacket;

typedef struct
{
   unsigned short int    xData;
   unsigned short int    yData;
   unsigned short int    zData;
}  vsBirdnetPositionPacket;

typedef struct
{
   unsigned short int    rotZ;
   unsigned short int    rotY;
   unsigned short int    rotX;
}  vsBirdnetAnglePacket;

typedef struct
{
   unsigned short int    r11, r12, r13;
   unsigned short int    r21, r22, r23;
   unsigned short int    r31, r32, r33;
}  vsBirdnetMatrixPacket;

typedef struct
{
   vsBirdnetPositionPacket    position;
   vsBirdnetAnglePacket       angle;
}  vsBirdnetPositionAnglesPacket;

typedef struct
{
   vsBirdnetPositionPacket    position;
   vsBirdnetMatrixPacket      matrix;
}  vsBirdnetPositionMatrixPacket;

typedef struct
{
   unsigned short int    xData;
   unsigned short int    yData;
   unsigned short int    zData;
   unsigned short int    hData;
}  vsBirdnetQuaternionPacket;

typedef struct
{
   vsBirdnetPositionPacket      position;
   vsBirdnetQuaternionPacket    quaternion;
}  vsBirdnetPositionQuaternionPacket;

// End of BirdNet protocol -------------------------------------------------

// Tracker configuration structure
typedef struct
{
    unsigned char  dataFormat;
    unsigned char  hemisphere;
    unsigned short refH, refP, refR;

} vsMStarTrackerConfig;

class vsEthernetMotionStar : public vsTrackingSystem
{
protected:
   
    // Network interface
    UDPUnicastNetworkInterface    *net;

    // Motion trackers and the associated information
    int                     numTrackers;
    vsMotionTracker         *tracker[VS_MSTAR_MAX_TRACKERS];
    int                     fbbAddress[VS_MSTAR_MAX_TRACKERS];
    vsMStarTrackerConfig    trackerConfig[VS_MSTAR_MAX_TRACKERS];

    // Shared memory object and state variables
    vsSharedInputData       *sharedData;
    int                     forked;
    int                     serverPID;
    static int              serverDone;

    // MotionStar parameters
    int                     numChassis;
    int                     addressMode;
    int                     master;
    int                     xmtrAddress;
    int                     streaming;
    int                     configured;
    double                  posScale;
    int                     ornScale;

    // Coordinate conversion matrices
    vsQuat                  coordXform;

    // MotionStar command state
    int                     currentSequence;

    // Functions for multi-process operation
    void                    serverLoop();
    static void             quitServer(int arg);

    // Utility functions
    int                     sendCommand(unsigned char command, 
                                        unsigned char extType, 
                                        vsBirdnetPacket *response);
    int                     sendPacket(vsBirdnetPacket *packet, int length,
                                       vsBirdnetPacket *response);
    int                     configureSystem();
    void                    enumerateTrackers(
                                vsBirdnetSystemStatusPacket *status);
    void                    updateConfiguration();

    // Functions to groom data for each data format
    void                    updatePosition(int trackerIndex, short birdData[]);
    void                    updateAngles(int trackerIndex, short birdData[]);
    void                    updateMatrix(int trackerIndex, short birdData[]);
    void                    updateQuaternion(int trackerIndex, 
                                             short birdData[]);
    void                    updatePosAngles(int trackerIndex, short birdData[]);
    void                    updatePosMatrix(int trackerIndex, short birdData[]);
    void                    updatePosQuat(int trackerIndex, short birdData[]);

    void                    updateSystem();

public:

                               vsEthernetMotionStar(char *serverName, int port,
                                                    int nTrackers, 
                                                    int masterFlag, 
                                                    int dataFormat);
    virtual                    ~vsEthernetMotionStar();

    // Function to split the MotionStar operations into two separate processes
    void                       forkTracking();

    // Measurement functions
    void                       ping();
    void                       startStream();
    void                       stopStream();

    // Other useful MotionStar functions
    void                       setDataFormat(int trackerNum, int format);
    void                       setActiveHemisphere(int trackerNum, 
                                                   short hSphere);
    void                       setReferenceFrame(int trackerNum, float h, 
                                                 float p, float r);
    void                       wakeMStar();
    void                       shutdownMStar();

    // Tracker functions
    virtual int                getNumTrackers();
    virtual vsMotionTracker    *getTracker(int index);

    virtual void               update();
};

#endif
