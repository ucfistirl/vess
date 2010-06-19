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
//    VESS Module:  vsPolaris.h++
//
//    Description:  Tracking system class supporting the Northern Digital
//                  Polaris optical tracking system
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_POLARIS_HPP
#define VS_POLARIS_HPP

#include "vsTrackingSystem.h++"
#include "vsSerialPort.h++"

#define VS_PL_MAX_TRACKERS 25

#define VS_PL_LED_OFF   'B'
#define VS_PL_LED_FLASH 'F'
#define VS_PL_LED_ON    'S'

#define VS_PL_BX_REPLY_1 0xA5
#define VS_PL_BX_REPLY_2 0xC4

#define VS_PL_SHM_KEY_BASE 0x71A50000

enum
{
    VS_PL_ERR_NONE,
    VS_PL_ERR_NO_REPLY,
    VS_PL_ERR_BAD_CRC,
    VS_PL_ERR_ERROR_MSG
};

class VESS_SYM vsPolaris : public vsTrackingSystem
{
protected:

    vsSerialPort           *port;

    int                    numTrackers;
    unsigned char          portHandle[VS_PL_MAX_TRACKERS];
    vsMotionTracker        *tracker[VS_PL_MAX_TRACKERS];
    double                 trackingError[VS_PL_MAX_TRACKERS];

    unsigned char          dataBuffer[512];

    bool                   bigEndian;

    atQuat                 coordXform, coordXformInv;
    atQuat                 referenceFrame;

    // For multithreading, private copy of tracker data, thread
    // state variables, and synchronization structures
    vsMotionTracker        *privateTracker[VS_PL_MAX_TRACKERS];
    double                 privateTrackingError[VS_PL_MAX_TRACKERS];
    bool                   forked;
    HANDLE                 serverThread;
    DWORD                  serverThreadID;
    bool                   serverDone;
    CRITICAL_SECTION       criticalSection;

    void                   initializeSystem(long baud);
    bool                   testIR();
    int                    enumerateTrackers();

    bool                   isBigEndian();
    short                  swapShort(short input);
    float                  swapFloat(float input);

    unsigned int           calculateCRC(unsigned char *string, int length);
    void                   sendCommand(char *command);
    int                    getReply();
    int                    getBinaryReply();
    void                   printError(char *method, char *header, int code);

    static DWORD WINAPI    serverLoop(void *parameter);

    void                   startTracking();
    void                   stopTracking();
    void                   ping();
    void                   processTrackerData();

    void                   updateSystem();

public:

                               vsPolaris(int portNumber, long baud, 
                                         int numTrackers);
                               vsPolaris(char *portDev, long baud, 
                                         int numTrackers);
    virtual                    ~vsPolaris();

    virtual const char         *getClassName();

    void                       forkTracking();

    void                       setBaudRate(long newBaud);

    void                       loadToolImage(int trackerNum, char *fileName);
    void                       setTrackingVolume(int volumeNumber);

    void                       setLED(int tracker, int led, int ledState);
    double                     getTrackingError(int index);

    void                       setReferenceFrame(double h, double p, double r);

    void                       resetSystem();

    virtual int                getNumTrackers();
    virtual vsMotionTracker    *getTracker(int index);

    virtual void               update();
};

#endif
