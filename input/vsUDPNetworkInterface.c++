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
//    VESS Module:  vsUDPNetworkInterface.c++
//
//    Description:  Class supporting UDP network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#include "vsUDPNetworkInterface.h++"

// ------------------------------------------------------------------------
// Creates and opens a UDP (datagram) socket
// ------------------------------------------------------------------------
vsUDPNetworkInterface::vsUDPNetworkInterface(int blocking) 
                     : vsNetworkInterface()
{
    if ( (socketValue = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        perror("socket");

    if (!blocking)
    {
        // Set to non blocking
        if (fcntl(socketValue, F_SETFL, FNDELAY) < 0)
            perror("fcntl");
    }
}


// ------------------------------------------------------------------------
// Closes the UDP socket
// ------------------------------------------------------------------------
vsUDPNetworkInterface::~vsUDPNetworkInterface()
{
    close(socketValue);
}

// ------------------------------------------------------------------------
// Reads up to maxSize bytes from the socket into the buffer and returns
// the actual number of bytes read
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::readPacket(u_char *buffer, int maxSize)
{
    int length;

    readNameLength = sizeof(readName);
    length = recvfrom(socketValue, buffer, maxSize, 0,
                      (struct sockaddr *) &readName, 
                      (socklen_t *) &readNameLength);

    if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
        perror("recvfrom");

    return (length);
}

// ------------------------------------------------------------------------
// Reads up to maxSize bytes from the socket into the buffer and returns
// the actual number of bytes read.  Also stores a timestamp in packetTime
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::readPacket(u_char *buffer, int maxSize,
                                      struct timeval *packetTime)
{
    int length;

    readNameLength = sizeof(readName);
    length = recvfrom(socketValue, buffer, maxSize, 0,
                      (struct sockaddr *) &readName,
                      (socklen_t *) &readNameLength);

    if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
        perror("recvfrom");
    else
        gettimeofday(packetTime, NULL);

    return (length);
}

// ------------------------------------------------------------------------
// Reads up to maxSize bytes from the socket into the buffer and returns 
// the actual number of bytes read.  The originating host of the packet is 
// stored in origin.
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::readPacket(u_char *buffer, int maxSize, char *origin)
{
   int    length;
   char   *addr;

   readNameLength = sizeof(readName);
   length = recvfrom(socketValue, buffer, maxSize, 0,
                     (struct sockaddr *) &readName,
                     (socklen_t *) &readNameLength);

   if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
      perror("recvfrom");

   addr = (char *) &readName.sin_addr.s_addr;
   sprintf(origin, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);

   return (length);
}

// ------------------------------------------------------------------------
// Reads up to maxSize bytes from the socket into the buffer and returns 
// the actual number of bytes read.  The originating host of the packet is 
// stored in origin and a timestamp is stored in packetTime.
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::readPacket(u_char *buffer, int maxSize, 
                                      struct timeval *packetTime, char *origin)
{
    int  length;
    char *addr;

    readNameLength = sizeof(readName);
    length = recvfrom(socketValue, buffer, maxSize, 0,
                      (struct sockaddr *) &readName,
                      (socklen_t *) &readNameLength);

    if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
        perror("recvfrom");
    else
        gettimeofday(packetTime, NULL);

    addr = (char *) &readName.sin_addr.s_addr;
    sprintf(origin, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);

    return (length);
}

// ------------------------------------------------------------------------
// Writes a packet of the specified length containing the data in buffer
// to the socket.
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::writePacket(u_char *buffer, int length)
{
    int error;

    error = sendto(socketValue, buffer, length, 0,
                   (struct sockaddr *) &writeName, writeNameLength);

    if ( (error == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
        perror("sendto");

    return (error);
}

