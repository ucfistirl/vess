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
}

// ------------------------------------------------------------------------
// Enables blocking on the socket for reads and writes
// ------------------------------------------------------------------------
void vsNetworkInterface::enableBlocking()
{
    int    statusFlags;

    // Get the current status flags on the socket
    if ( (statusFlags = fcntl(socketValue, F_GETFL)) < 0 )
        printf("Unable to get status of socket.\n");
    else
    {
        // Try to set the socket flags (NOT including the nonblocking flag)
        // and notify the user if there was an error
        if (fcntl(socketValue, F_SETFL, statusFlags & (~FNONBLOCK)) < 0)
            printf("Unable to enable blocking on socket.\n");
    }
}

// ------------------------------------------------------------------------
// Disables blocking on the socket for reads and writes
// ------------------------------------------------------------------------
void vsNetworkInterface::disableBlocking()
{
    int    statusFlags;

    // Get the current status flags on the socket
    if ( (statusFlags = fcntl(socketValue, F_GETFL)) < 0 )
        printf("Unable to get status of socket.\n");
    else
    {
        // Try to set the socket flags (including the nonblocking flag)
        // and notify the user if there was an error
        if (fcntl(socketValue, F_SETFL, statusFlags | FNONBLOCK) < 0)
            printf("Unable to disable blocking on socket.\n");
    }
}

