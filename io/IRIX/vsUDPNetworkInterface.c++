
//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2003, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsUDPNetworkInterface.c++
//
//    Description:  Class supporting UDP network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include "vsUDPNetworkInterface.h++"

// ------------------------------------------------------------------------
// Constructor, opens a socket to the given address on the given port
// ------------------------------------------------------------------------
vsUDPNetworkInterface::vsUDPNetworkInterface(char *address, u_short port)
{
    char             hostname[MAXHOSTNAMELEN];
    struct hostent   *host;

    // Open the socket
    if ( (socketValue = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        printf("Unable to open socket for communication.\n");

    // Get information about this host and initialize the read name field
    gethostname(hostname, sizeof(hostname));
    host = gethostbyname(hostname);
    readName.sin_family = AF_INET;
    memcpy(&readName.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    readName.sin_port = htons(port);

    // Get information about remote host and initialize the write name field
    host = gethostbyname(address);
    writeName.sin_family = AF_INET;
    memcpy(&writeName.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
    writeName.sin_port = htons(port);

    // Bind to the port
    if (bind(socketValue, (struct sockaddr *) &readName, sizeof(readName)) < 0)
    {
        printf("Unable to bind to the port.\n");
    }
}

// ------------------------------------------------------------------------
// Constructor, opens a socket on the broadcast address on the given port
// ------------------------------------------------------------------------
vsUDPNetworkInterface::vsUDPNetworkInterface(u_short port)
{
    int    on;

    // Open the socket
    if ( (socketValue = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        printf("Unable to open socket for communication.\n");

    // Initialize the name fields
    readName.sin_family = AF_INET;
    readName.sin_addr.s_addr = htonl(INADDR_ANY);
    readName.sin_port = htons(port);
    writeName.sin_family = AF_INET;
    writeName.sin_addr.s_addr = htonl(INADDR_BROADCAST);
    writeName.sin_port = htons(port);

    // Set the options we need for broadcasting
    on = 1;
    if (setsockopt(socketValue, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
        printf("Unable to set broadcasting on socket.\n");

    // Bind to the port
    if (bind(socketValue, (struct sockaddr *) &readName, sizeof(readName)) < 0)
    {
        printf("Unable to bind to the port.\n");
    }
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsUDPNetworkInterface::~vsUDPNetworkInterface()
{
    // Close the socket
    close(socketValue);
}

// ------------------------------------------------------------------------
// Reads up to maxSize bytes from the socket into the buffer and returns
// the actual number of bytes read
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::read(u_char *buffer, u_long len)
{
    struct sockaddr_in    fromAddress;
    socklen_t             fromAddressLength;
    int                   packetLength;

    // Get a packet
    fromAddressLength = sizeof(fromAddress);
    packetLength = recvfrom(socketValue, buffer, len, 0, 
                            (struct sockaddr *) &fromAddress, 
                            &fromAddressLength);

    // Tell user how many bytes we read (-1 if error)
    return packetLength;
}

// ------------------------------------------------------------------------
// Writes a packet of the specified length containing the data in buffer
// to the socket.
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::write(u_char *buffer, u_long len)
{
    int    lengthWritten;

    // Write the packet
    lengthWritten = sendto(socketValue, buffer, len, 0, 
                           (struct sockaddr *) &writeName, writeNameLength);

    // Tell user how many bytes we wrote (-1 if error)
    return lengthWritten;
}

