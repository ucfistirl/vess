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
//    VESS Module:  vsUDPNetworkInterface.h++
//
//    Description:  Class supporting UDP network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#ifndef VS_UDP_NETWORK_INTERFACE_HPP
#define VS_UDP_NETWORK_INTERFACE_HPP

#include <unistd.h>
#include <fcntl.h>
#include "vsNetworkInterface.h++"

// Stupid conditional blocks because Linux can't follow conventions
#ifdef IRIX
typedef int socklen_t;
#endif

#ifdef IRIX64
typedef int socklen_t;
#endif

class vsUDPNetworkInterface : public vsNetworkInterface
{
public:
               vsUDPNetworkInterface(int blocking);
    virtual    ~vsUDPNetworkInterface();

    int        readPacket(u_char *buffer);
    int        readPacket(u_char *buffer, int maxSize);
    int        readPacket(u_char *buffer, struct timeval *packetTime);
    int        readPacket(u_char *buffer, int maxSize, 
                          struct timeval *packetTime);      
    int        readPacket(u_char *buffer, char *origin);
    int        readPacket(u_char *buffer, int maxSize, char *origin);
    int        readPacket(u_char *buffer, struct timeval *packetTime,
                          char *origin);
    int        readPacket(u_char *buffer, int maxSize, 
                          struct timeval *packetTime, char *origin);

    int        writePacket(u_char *buffer, int length);
};

#endif
