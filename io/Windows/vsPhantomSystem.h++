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
//    VESS Module:  vsPhantomSystem.c++
//
//    Description:  A class for accessing and controlling the Phantom
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_PHANTOM_SYSTEM_HPP
#define VS_PHANTOM_SYSTEM_HPP

#include "vsIOSystem.h++"
#include "vsPhantom.h++"
#include "atTCPNetworkInterface.h++"

class VESS_SYM vsPhantomSystem : public vsIOSystem
{
protected:

    atTCPNetworkInterface    *netInterface;
    vsPhantom                *phantom;
    bool                     initialized;
    bool                     forces;
    atMatrix                 gstToVsRotation;
    atMatrix                 vsToGstRotation;
    double                   positionScale;
    u_char                   *receiveBuffer;
    int                      receiveBufferLength;
    u_char                   *sendBuffer;
    int                      sendBufferLength;

    void                     readCommand(u_char *version, u_char *command,
                                         u_short *dataLength, u_char **data);

    void                     writeCommand(u_char version, u_char command,
                                          u_short dataLength, u_char *data);

    bool                     readAcknowledge();

public:

                          vsPhantomSystem(char *serverName, u_short port,
                                          char *phantomName);

    virtual               ~vsPhantomSystem(void);

    virtual const char    *getClassName(void);

    vsPhantom             *getPhantom(void);

    void                  setScale(double newScale);
    double                getScale(void);

    bool                  setForce(atVector force);
    bool                  enableForces(void);
    bool                  disableForces(void);
    bool                  resetPhantom(void);
    bool                  isResetNeeded(void);
    
    float                 getUpdateRate(void);
    float                 getMaxStiffness(void);

    bool                  isForceEnabled(void);

    virtual void          update(void);
};

#endif
