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
//    VESS Module:  vsPhantomProtocol.c++
//
//    Description:  A header file defining the commands and structures of
//                  used to communicate with the Phantom server using a
//                  TCP connection.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_PHANTOM_PROTOCOL_HPP
#define VS_PHANTOM_PROTOCOL_HPP

// The version of the protocol used by whoever includes this file.
#define PS_PROTOCOL_VERSION 1

// The maximum size of a command.
#define PS_MAX_COMMAND_LENGTH 1024

// The size of the header.
#define PS_HEADER_LENGTH 8

// These are the commands.
// Ensure the server and client are using the same definitions!
enum
{
    PS_COMMAND_QUIT,
    PS_COMMAND_INITIALIZE,
    PS_COMMAND_RESET,
    PS_COMMAND_GETSTATE,
    PS_COMMAND_GETMAXSTIFFNESS,
    PS_COMMAND_GETUPDATERATE,
    PS_COMMAND_DISABLEFORCE,
    PS_COMMAND_ENABLEFORCE,
    PS_COMMAND_APPLYFORCE,
    PS_COMMAND_ERROR,
    PS_COMMAND_ACKNOWLEDGE,
    PS_COMMAND_ISRESETNEEDED
};

// The structure for the header.  Word alligned with 8 byte words.  SGI did not
// like just 4 bytes.  The 4 unused bytes can be used for future additions.
struct VESS_SYM PhantomCommandHeader
{
    u_char      version;
    u_char      command;
    u_short     length;
    u_char      unused[4];
};

// Structure used to communicate the physical state of the phantom.
// A boolean value to indicate if the switch is pressed or not,
// and a matrix to specify the orientation and position of the Phantom.
// Windows word aligns with 8 byte words and changing that is not trivial,
// so we word align wiht 8 byte words as well.
struct VESS_SYM PhantomState
{
    char    switchState;
    char    unused[7];
    double  velocityData[3];
    double  matrixData[16];
};

#endif

