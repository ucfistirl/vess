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
//    VESS Module:  vsCgParameter.c++
//
//    Description:  Abstract class for all Cg Parameter object.  Provides
//                  some basic common features.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCgParameter.h++"

// ------------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------------
vsCgParameter::vsCgParameter(osgNVCg::Program *newProgram)
{
    // Store the program which this parameter belongs to.
    program = newProgram;

    // Clear the stored variable string.
    variableName[0] = 0;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCgParameter::~vsCgParameter()
{
}

// ------------------------------------------------------------------------
// Return the program which this parameter is a part of.
// ------------------------------------------------------------------------
osgNVCg::Program *vsCgParameter::getProgram()
{
    return program;
}

// ------------------------------------------------------------------------
// Return the variable name which this parameter is linked to.
// ------------------------------------------------------------------------
char *vsCgParameter::getVariableName()
{
    return variableName;
}
