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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "vsGlobals.h++"

#define MAX_PACKET_SIZE   65000

class VS_IO_DLL vsNetworkInterface
{
protected:
    SOCKET                socketValue;
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

