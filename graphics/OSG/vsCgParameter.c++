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
vsCgParameter::vsCgParameter(vsCgShaderAttribute *newShaderAttribute,
                             vsCgShaderProgramType newWhichProgram,
                             char *newVariableName)
{
    // Store the shader attribute which this parameter belongs to.
    shaderAttribute = newShaderAttribute;

    // Store the type of program this parameter belongs to.
    whichProgram = newWhichProgram;

    // Keep a copy of the variable name.
    strncpy(variableName, newVariableName, VARIABLE_NAME_MAX);

    if (getCgProgram() == NULL)
    {
        printf("vsCgParameter::vsCgParameter: Error: "
            "Specified shader program is currently NULL!\n");
    }
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
osgNVCg::Program *vsCgParameter::getCgProgram()
{
    return shaderAttribute->getCgProgram(whichProgram);
}

// ------------------------------------------------------------------------
// Return the vsCgShaderAttribute which this parameter is a part of.
// ------------------------------------------------------------------------
vsCgShaderAttribute *vsCgParameter::getCgShaderAttribute()
{
    return shaderAttribute;
}

// ------------------------------------------------------------------------
// Return the type of program this parameter is for within the attribute.
// ------------------------------------------------------------------------
vsCgShaderProgramType vsCgParameter::getCgShaderProgramType()
{
    return whichProgram;
}

// ------------------------------------------------------------------------
// Return the variable name which this parameter is linked to.
// ------------------------------------------------------------------------
char *vsCgParameter::getCgVariableName()
{
    return variableName;
}
