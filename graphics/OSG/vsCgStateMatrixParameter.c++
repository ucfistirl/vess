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
//    VESS Module:  vsCgStateMatrixParameter.c++
//
//    Description:  Class for managing a Cg state matrix parameter.
//                  This object takes enumerated identifiers that specify
//                  which state matrix to set and what operations to
//                  perform on it.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCgStateMatrixParameter.h++"

// ------------------------------------------------------------------------
// Constructor - Copy the variable name and create the osgNVCg parameter.
// ------------------------------------------------------------------------
vsCgStateMatrixParameter::vsCgStateMatrixParameter(osgNVCg::Program *newProgram,
                                         char *newVariableName) :
                                         vsCgParameter(newProgram)
{
    // Keep a copy of the variable name.
    strncpy(variableName, newVariableName, VARIABLE_NAME_MAX);

    // Create the parameter and add it to the program.
    stateMatrixParameter =
      new osgNVCg::StateMatrixParameter(program, variableName);
    program->addParameter(stateMatrixParameter);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCgStateMatrixParameter::~vsCgStateMatrixParameter()
{
}

// ------------------------------------------------------------------------
// Return the type of parameter object this object is.
// ------------------------------------------------------------------------
vsCgParameterType vsCgStateMatrixParameter::getCgParameterType()
{
    return VS_CG_STATE_MATRIX_PARAMETER;
}

// ------------------------------------------------------------------------
// Specify which state matrix to set, no operations are performed to it.
// ------------------------------------------------------------------------
void vsCgStateMatrixParameter::set(vsCgStateMatrix matrix)
{
    stateMatrixParameter->set(
      (osgNVCg::StateMatrixParameter::Matrix_type) matrix);
}

// ------------------------------------------------------------------------
// Specify which state matrix to set and which operation to perform no it.
// ------------------------------------------------------------------------
void vsCgStateMatrixParameter::set(vsCgStateMatrix matrix,
                              vsCgStateMatrixMapping mapping)
{
    stateMatrixParameter->set(
      (osgNVCg::StateMatrixParameter::Matrix_type) matrix,
      (osgNVCg::StateMatrixParameter::Mapping_type) mapping);
}
