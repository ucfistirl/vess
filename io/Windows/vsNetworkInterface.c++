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
//    VESS Module:  vsNetworkInterface.c++
//
//    Description:  Abstract class supporting basic network communications
//
//    Author(s):    Glenn Martin
//
//------------------------------------------------------------------------

#include "vsNetworkInterface.h++"

// ------------------------------------------------------------------------
// Constructor, iniitializes some variables
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
    writeNameLength = sizeof(writeName);
    ZeroMemory((void *) &readName, readNameLength);
    ZeroMemory((void *) &writeName, writeNameLength);
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

