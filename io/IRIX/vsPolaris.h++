#ifndef VS_POLARIS_HPP
#define VS_POLARIS_HPP

#include "vsTrackingSystem.h++"
#include "vsSerialPort.h++"
#include "vsSharedInputData.h++"

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

class vsPolaris : public vsTrackingSystem
{
protected:

    vsSerialPort         *port;

    int                  numTrackers;
    unsigned char        portHandle[VS_PL_MAX_TRACKERS];
    vsMotionTracker      *tracker[VS_PL_MAX_TRACKERS];
    double               trackingError[VS_PL_MAX_TRACKERS];

    unsigned char        dataBuffer[512];

    bool                 bigEndian;

    vsQuat               coordXform, coordXformInv;
    vsQuat               referenceFrame;

    vsSharedInputData    *sharedData;
    bool                 forked;
    int                  serverPID;
    static bool          serverDone;

    void                 initializeSystem(long baud);
    bool                 testIR();
    int                  enumerateTrackers();

    bool                 isBigEndian();
    short                swapShort(short input);
    float                swapFloat(float input);

    unsigned int         calculateCRC(unsigned char *string, int length);
    void                 sendCommand(char *command);
    int                  getReply();
    int                  getBinaryReply();
    void                 printError(char *method, char *header, int code);

    void                 serverLoop();
    static void          quitServer(int arg);

    void                 startTracking();
    void                 stopTracking();
    void                 ping();
    void                 processTrackerData();

    void                 updateSystem();

public:

                               vsPolaris(int portNumber, long baud, 
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
