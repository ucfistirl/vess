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
//    VESS Module:  vsCgMatrixParameter.c++
//
//    Description:  Class for managing a Cg matrix parameter.  Setting a
//                  value to this object will set the value to the Cg
//                  variable name it is linked to.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCgMatrixParameter.h++"

// ------------------------------------------------------------------------
// Constructor - Copy the variable name and create the osgNVCg parameter.
// ------------------------------------------------------------------------
vsCgMatrixParameter::vsCgMatrixParameter(osgNVCg::Program *newProgram,
                                         char *newVariableName) :
                                         vsCgParameter(newProgram)
{
    // Keep a copy of the variable name.
    strncpy(variableName, newVariableName, VARIABLE_NAME_MAX);

    // Create the parameter and add it to the program.
    matrixParameter = new osgNVCg::MatrixParameter(program, variableName);
    program->addParameter(matrixParameter);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCgMatrixParameter::~vsCgMatrixParameter()
{
}

// ------------------------------------------------------------------------
// Return the type of parameter object this object is.
// ------------------------------------------------------------------------
vsCgParameterType vsCgMatrixParameter::getCgParameterType()
{
    return VS_CG_MATRIX_PARAMETER;
}

// ------------------------------------------------------------------------
// Set the OSG matrix to the transpose of the VESS matrix, and then
// hand it to the osgNV parameter object.
// ------------------------------------------------------------------------
void vsCgMatrixParameter::set(vsMatrix value)
{
    // Set the OSG matrix to be a transpose of the VESS matrix.
    osgMatrix.set(value[0][0], value[1][0], value[2][0], value[3][0],
                  value[0][1], value[1][1], value[2][1], value[3][1],
                  value[0][2], value[1][2], value[2][2], value[3][2],
                  value[0][3], value[1][3], value[2][3], value[3][3]);

    // Give the osgNV parameter object the OSG matrix.
    matrixParameter->set(osgMatrix);
}
