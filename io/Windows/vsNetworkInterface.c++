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
//    VESS Module:  vsNetworkInterface.c++
//
//    Description:  Class supporting network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "vsNetworkInterface.h++"

// ------------------------------------------------------------------------
// Initializes the read and write address structures
// ------------------------------------------------------------------------
vsNetworkInterface::vsNetworkInterface()
{
    WORD winsockVersionReq;
    WSADATA winsockData;

    // Start winsock (we do this once per network interface so that winsock
    // will be cleaned up only when we're done with all of our network
    // interfaces)

    // Request winsock 1.1
    winsockVersionReq = MAKEWORD(1, 1);

    // Start winsock, check for errors
    if (WSAStartup(winsockVersionReq, &winsockData) != 0)
    {
        printf("vsNetworkInterface::vsNetworkInterface: "
            "Error starting winsock!\n");
        return;
    }

    // Clear the name fields
    readNameLength = sizeof(readName);
    memset(&readName, 0, readNameLength);
    writeNameLength = sizeof(writeName);
    memset(&writeName, 0, writeNameLength);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsNetworkInterface::~vsNetworkInterface()
{
    // Clean up winsock.  Note that the actual cleanup is done only when
    // we've called WSACleanup a number of times equal to the number of
    // times we've called WSAStartup.  Windows keeps track of the number
    // of WSAStartup and WSACleanup calls internally.
    WSACleanup();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsNetworkInterface::getClassName()
{
    return "vsNetworkInterface";
}

// ------------------------------------------------------------------------
// Enables blocking on the socket for reads and writes
// ------------------------------------------------------------------------
void vsNetworkInterface::enableBlocking()
{
    u_long    nonblockingMode;

    // Try to enable blocking (actually disable non-blocking) and notify the 
    // user if there was an error
    nonblockingMode = 0;
    if (ioctlsocket(socketValue, FIONBIO, &nonblockingMode) == SOCKET_ERROR)
        printf("Unable to enable blocking on socket.\n");
}

// ------------------------------------------------------------------------
// Disables blocking on the socket for reads and writes
// ------------------------------------------------------------------------
void vsNetworkInterface::disableBlocking()
{
    u_long    nonblockingMode;

    // Try to enable non-blocking and notify the user if there was an error
    nonblockingMode = 1;
    if (ioctlsocket(socketValue, FIONBIO, &nonblockingMode) == SOCKET_ERROR)
        printf("Unable to disable blocking on socket.\n");
}

