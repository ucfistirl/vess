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
//    VESS Module:  vsNetworkInterface.h++
//
//    Description:  Abstract class supporting basic network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#ifndef VS_NETWORK_INTERFACE_HPP
#define VS_NETWORK_INTERFACE_HPP

// Abstract base class for a socket-based network interface

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include "vsObject.h++"

#define MAX_PACKET_SIZE   65000

class vsNetworkInterface : public vsObject
{
protected:
    int                   socketValue;
    struct sockaddr_in    readName;
    struct sockaddr_in    writeName;
    int                   readNameLength;
    int                   writeNameLength;

public:
                     vsNetworkInterface();
      virtual        ~vsNetworkInterface();

      virtual int    readPacket(u_char *buffer, int size) = 0;
      virtual int    readPacket(u_char *buffer, int size,
                                struct timeval *packetTime) = 0;
      virtual int    readPacket(u_char *buffer, int size, char *origin) = 0;
      virtual int    readPacket(u_char *buffer, int size, 
                                struct timeval *packetTime, char *origin) = 0;

      virtual int    writePacket(u_char *buffer, int length) = 0;
};

#endif

