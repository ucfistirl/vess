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

// Various UNIX implementations seem to disagree on what type to use for 
// the length of a socket in functions such as recvfrom and accept.  BSD
// originally defined it as int, while POSIX likes to use socklen_t (an
// unsigned int).  IRIX follows BSD, and Linux follows POSIX.  So, we
// have to create an ugly pseudo-type to get this working across both.
#ifdef __linux__
typedef socklen_t SOCKET_LENGTH;
#endif

#ifdef IRIX
typedef int SOCKET_LENGTH;
#endif

#ifdef IRIX64
typedef int SOCKET_LENGTH;
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
