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
//    VESS Module:  vsUDPUnicastNetworkInterface.c++
//
//    Description:  Class supporting Unicast UDP network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#include "vsUDPUnicastNetworkInterface.h++"

// ------------------------------------------------------------------------
// Constructor, opens a socket to the given address on the given port
// ------------------------------------------------------------------------
vsUDPUnicastNetworkInterface::vsUDPUnicastNetworkInterface(char *address, 
                                                           short port, 
                                                           int blocking) 
                            : vsUDPNetworkInterface(blocking)
{
   struct hostent *host;
   BOOL           on;
   int            max;

    // Initialize the read name field
    readName.sin_family = AF_INET;
    readName.sin_addr.s_addr = htonl(INADDR_ANY);
    readName.sin_port = htons(port);

    // Get information about remote host
    host = gethostbyname(address);

    // Initialize the write name field
    writeName.sin_family = AF_INET;
    memcpy(&writeName.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    writeName.sin_port = htons(port);

    // Set the options we need on the socket
    on = 1;
    max = MAX_PACKET_SIZE;
    if (setsockopt(socketValue, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on)) < 0)
        fprintf(stderr, "setsockopt broadcast:  Error setting socket option! (%d)\n",
            WSAGetLastError());
    if (setsockopt(socketValue, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
        fprintf(stderr, "setsockopt reuseport:  Error setting socket option! (%d)\n",
            WSAGetLastError());
    if (setsockopt(socketValue, SOL_SOCKET, SO_SNDBUF, (char *)&max, sizeof(max)) < 0)
        fprintf(stderr, "setsockopt sndbuf:  Error setting socket option! (%d)\n",
            WSAGetLastError());
    if (setsockopt(socketValue, SOL_SOCKET, SO_RCVBUF, (char *)&max, sizeof(max)) < 0)
        fprintf(stderr, "setsockopt rcvbuf:  Error setting socket option! (%d)\n",
            WSAGetLastError());

    // Bind to the port
    if (bind(socketValue, (struct sockaddr *) &readName, sizeof(readName)) < 0)
    {
        fprintf(stderr, "bind:  Error binding to the port! (%d)\n", WSAGetLastError());
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsUDPUnicastNetworkInterface::~vsUDPUnicastNetworkInterface()
{
}

