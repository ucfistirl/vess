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
#include <sys/types.h>
#include <sys/timeb.h>

// ------------------------------------------------------------------------
// Creates and opens a UDP (datagram) socket
// ------------------------------------------------------------------------
vsUDPNetworkInterface::vsUDPNetworkInterface(int blocking) 
                     : vsNetworkInterface()
{
    unsigned long blockMode;
    int error;
    
    // Open the datagram socket, and print an error if this fails
    if ( (socketValue = socket(AF_INET, SOCK_DGRAM, 0)) < 0 )
        fprintf(stderr, "socket:  Unable to open socket! (%d)\n", WSAGetLastError());

    // Set to non blocking, if so configured
    if (!blocking)
    {
        blockMode = 1;
        
        if (ioctlsocket(socketValue, FIONBIO, &blockMode) < 0)
        {
            error = WSAGetLastError();
            fprintf(stderr, "ioctlsocket:  Unable to set socket to non-blocking\n");
        }
    }
}


// ------------------------------------------------------------------------
// Closes the UDP socket
// ------------------------------------------------------------------------
vsUDPNetworkInterface::~vsUDPNetworkInterface()
{
    // Close the socket
    closesocket(socketValue);
}

// ------------------------------------------------------------------------
// Reads up to maxSize bytes from the socket into the buffer and returns
// the actual number of bytes read
// ------------------------------------------------------------------------
int vsUDPNetworkInterface::readPacket(u_char *buffer, int maxSize)
{
    int length;
    int error;

    // Get the length of the socket structure for the origin of the packet
    readNameLength = sizeof(readName);

    // Receive a packet from the socket
    length = recvfrom(socketValue, (char *)buffer, maxSize, 0,
        (struct sockaddr *) &readName, (int *) &readNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong
    if (length == SOCKET_ERROR)
    { 
        error = WSAGetLastError();
        if ((error != WSAEINTR) && (error != WSAEWOULDBLOCK))
            fprintf(stderr, "recvfrom: Error receiving data! (%d)\n", error);
    }

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
    int error;
    struct _timeb timeBuffer;

    // Get the length of the socket structure for the origin of the packet
    readNameLength = sizeof(readName);

    // Receive a packet from the socket
    length = recvfrom(socketValue, (char *)buffer, maxSize, 0,
        (struct sockaddr *) &readName, (int *) &readNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong.  Otherwise, copy the current system time
    // into the packetTime field.
    if (length == SOCKET_ERROR)
    { 
        error = WSAGetLastError();
        if ((error != WSAEINTR) && (error != WSAEWOULDBLOCK))
            fprintf(stderr, "recvfrom: Error receiving data! (%d)\n", error);
    }
    else
    {
        // Use Windows' _ftime command and pack the result into the given 
        // struct timeval.  Unfortunately, this method is only accurate to
        // about 55ms.
        _ftime(&timeBuffer);
        packetTime->tv_sec = timeBuffer.time;
        packetTime->tv_usec = 1000 * timeBuffer.millitm;
    }

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
    int    error;
    char   *addr;

    // Get the length of the socket structure for the origin of the packet
    readNameLength = sizeof(readName);

    // Receive a packet from the socket
    length = recvfrom(socketValue, (char *)buffer, maxSize, 0,
        (struct sockaddr *) &readName, (int *) &readNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong
    if (length == SOCKET_ERROR)
    { 
        error = WSAGetLastError();
        if ((error != WSAEINTR) && (error != WSAEWOULDBLOCK))
            fprintf(stderr, "recvfrom: Error receiving data! (%d)\n", error);
    }

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
    int  error;
    char *addr;
    struct _timeb timeBuffer;

    // Get the length of the socket structure for the origin of the packet
    readNameLength = sizeof(readName);

    // Receive a packet from the socket
    length = recvfrom(socketValue, (char *)buffer, maxSize, 0,
        (struct sockaddr *) &readName, (int *) &readNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong.  Otherwise, copy the current system time
    // into the packetTime field.
    if (length == SOCKET_ERROR)
    { 
        error = WSAGetLastError();
        if ((error != WSAEINTR) && (error != WSAEWOULDBLOCK))
            fprintf(stderr, "recvfrom: Error receiving data! (%d)\n", error);
    }
    else
    {
        // Use Windows' _ftime command and pack the result into the given 
        // struct timeval.  Unfortunately, this method is only accurate to
        // about 55ms.
        _ftime(&timeBuffer);
        packetTime->tv_sec = timeBuffer.time;
        packetTime->tv_usec = 1000 * timeBuffer.millitm;
    }

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
    error = sendto(socketValue, (char *)buffer, length, 0,
        (struct sockaddr *) &writeName, writeNameLength);

    // Check the length and error conditions, and print an error
    // if anything is wrong
    if (error == SOCKET_ERROR)
    { 
        error = WSAGetLastError();
        if ((error != WSAEINTR) && (error != WSAEWOULDBLOCK))
            fprintf(stderr, "sendto: Error transmitting data! (%d)\n", error);
            
        return -1;
    }

    // Return the length of the packet
    return (error);
}

