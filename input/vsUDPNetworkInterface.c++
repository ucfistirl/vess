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
    // Open the datagram socket, and print an error if this fails
    if ( (socketValue = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        perror("socket");

    // Set to non blocking, if so configured
    if (!blocking)
    {
        if (fcntl(socketValue, F_SETFL, FNDELAY) < 0)
            perror("fcntl");
    }
}


// ------------------------------------------------------------------------
// Closes the UDP socket
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
int vsUDPNetworkInterface::readPacket(u_char *buffer, int maxSize)
{
    int length;

    // Get the length of the socket structure for the origin of the packet
    readNameLength = sizeof(readName);

    // Receive a packet from the socket
    length = recvfrom(socketValue, buffer, maxSize, 0,
                      (struct sockaddr *) &readName, 
                      (socklen_t *) &readNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong
    if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
        perror("recvfrom");

    // Return the length of the packet
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

    // Get the length of the socket structure for the origin of the packet
    readNameLength = sizeof(readName);

    // Receive a packet from the socket
    length = recvfrom(socketValue, buffer, maxSize, 0,
                      (struct sockaddr *) &readName,
                      (socklen_t *) &readNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong.  Otherwise, return the current system
    // time in the packetTime field
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

    // Get the length of the socket structure for the origin of the packet
    readNameLength = sizeof(readName);

    // Receive a packet from the socket
    length = recvfrom(socketValue, buffer, maxSize, 0,
                      (struct sockaddr *) &readName,
                      (socklen_t *) &readNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong.
    if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
        perror("recvfrom");

    // Copy the packet's origin IP address into the origin field
    addr = (char *) &readName.sin_addr.s_addr;
    sprintf(origin, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);

    // Return the length of the packet
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

    // Get the length of the socket structure for the origin of the packet
    readNameLength = sizeof(readName);

    // Receive a packet from the socket
    length = recvfrom(socketValue, buffer, maxSize, 0,
                      (struct sockaddr *) &readName,
                      (socklen_t *) &readNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong.  Otherwise, return the current system
    // time in the packetTime field
    if ( (length == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
        perror("recvfrom");
    else
        gettimeofday(packetTime, NULL);

    // Copy the packet's origin IP address into the origin field
    addr = (char *) &readName.sin_addr.s_addr;
    sprintf(origin, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);

    // Return the length of the packet
    return (length);
}

// ------------------------------------------------------------------------
// Writes a packet of the specified length containing the data in buffer
// to the socket.
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::writePacket(u_char *buffer, int length)
{
    int error;

    // Send the packet on the open socket
    error = sendto(socketValue, buffer, length, 0,
                   (struct sockaddr *) &writeName, writeNameLength);

    // Check for any error condition, and print an error message if
    // any exists
    if ( (error == -1) && (errno != EINTR) && (errno != EWOULDBLOCK) )
        perror("sendto");

    // Return the length of the packet
    return (error);
}

