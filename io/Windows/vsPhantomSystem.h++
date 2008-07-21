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

/******************************************************************************/
/* Some macros and functions to do endian conversions of floats               */
/*   with htonl and ntohl.                                                    */
/******************************************************************************/
static float ntohf(float x)
{
    unsigned long  *tempLong;

    // Make tempLong point to the same memory space as x.
    tempLong = (unsigned long *) &x;

    // Now we can handle the float data as if it were a long here.
    *tempLong = ntohl(*tempLong);

    // Return the altered float.
    return(x);
}
#define htonf(x) ntohf(x);
static double ntohd(double x)
{
    unsigned long tempLong;

    union
    {
        double        dval;
        unsigned long lvals[2];
    } value;

    value.dval = x;
    // Individually Shift the two 32bit (long) values within the double.
    value.lvals[0] = ntohl(value.lvals[0]);
    value.lvals[1] = ntohl(value.lvals[1]);

    // Swap the longs inside the double to complete the endian transfer.
    tempLong = value.lvals[0];
    value.lvals[0] = value.lvals[1];
    value.lvals[1] = tempLong;
    return(value.dval);
}
#define htond(x) ntohd(x)

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
