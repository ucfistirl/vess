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
    // Clear the name fields
    readNameLength = sizeof(readName);
    writeNameLength = sizeof(writeName);
    bzero((char *) &readName, readNameLength);
    bzero((char *) &writeName, writeNameLength);
}


// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsNetworkInterface::~vsNetworkInterface()
{
}

