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
//    VESS Module:  vsVestSystem.h++
//
//    Description:  Interface to IST's vibrating vest
//
//    Author(s):    Ryan Wilson
//
//------------------------------------------------------------------------

#ifndef VS_VEST_SYSTEM_HPP
#define VS_VEST_SYSTEM_HPP

#include "vsIOSystem.h++"
#include "vsSerialPort.h++"
#include "vsVest.h++"

#define VS_VEST_BUFFER_SIZE 128

// The various states that the vest can be in
enum 
{
    VS_VEST_STATE_UNKNOWN       =  0,// don't know anything about a vest yet
    VS_VEST_STATE_NOT_AVAILABLE = -1,// vest has not been detected
    VS_VEST_STATE_STOPPED       =  1,// vest has been reset
    VS_VEST_STATE_RUNNING       =  2,// vest is in ready mode, but not listening
    VS_VEST_STATE_LISTENING     =  3,// vest is ready and listening for commands
};

class VESS_SYM vsVestSystem : public vsIOSystem
{
protected:

    vsSerialPort            *port;
    int                     vestState;

    // A byte used for tracking which zones are currently on/off
    // least significant bit represents zone 1
    // most significant bit represents zone 8
    // bit on = zone on
    // bit off = zone off
    unsigned char           currentState;
    bool                    zonesChanged;

    vsVest                  *vest;

    // The sent-to-vest buffer of commands
    unsigned char           buffer[ VS_VEST_BUFFER_SIZE ];
    int                     bufferLength;

    // the vest repeats everything we send back to us - make sure that we
    // can ignore these bytes
    int                     bytesToIgnore;

    bool                    addToBuffer( char * commands, int length );
    void                    consumeBuffer( int length );

    // Send/Receive Data from the vest
    void                    readVestData();
    void                    sendVestData();

public:

                            vsVestSystem( int portNumber );
    virtual                 ~vsVestSystem();

    // Inherited from vsObject
    virtual const char      *getClassName();

    // Initializes the vest for use (called automatically in the constructor)
    void                    initializeVest();

    int                     getNumberOfZones();

    // Turn a given zone on or off
    void                    setZoneState( int whichZone, bool newState );
    bool                    getZoneState( int whichZone );

    vsVest                  *getVest();

    // The update function
    void                    update();

    bool                    isSendBufferEmpty();
    int                     getVestState();
};

#endif
