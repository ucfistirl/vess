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
   int            on;
   int            max;

    // Initialize the read name field
    readName.sin_family = AF_INET;
    readName.sin_addr.s_addr = htonl(INADDR_ANY);
    readName.sin_port = htons(port);

    // Get information about remote host
    host = gethostbyname(address);

    // Initialize the write name field
    writeName.sin_family = AF_INET;
    bcopy(host->h_addr_list[0], &writeName.sin_addr.s_addr, host->h_length);
    writeName.sin_port = htons(port);

    // Set the options we need on the socket
    on = 1;
    max = MAX_PACKET_SIZE;
    if (setsockopt(socketValue, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
        perror("setsockopt broadcast");
    if (setsockopt(socketValue, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        perror("setsockopt reuseport");
    if (setsockopt(socketValue, SOL_SOCKET, SO_SNDBUF, &max, sizeof(max)) < 0)
        perror("setsockopt sndbuf");
    if (setsockopt(socketValue, SOL_SOCKET, SO_RCVBUF, &max, sizeof(max)) < 0)
        perror("setsockopt rcvbuf");

    // Bind to the port
    if ( bind(socketValue, (struct sockaddr *) &readName, 
              sizeof(readName)) < 0 )
        perror("bind");
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsUDPUnicastNetworkInterface::~vsUDPUnicastNetworkInterface()
{
}

