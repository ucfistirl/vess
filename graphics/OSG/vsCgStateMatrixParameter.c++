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
vsCgStateMatrixParameter::vsCgStateMatrixParameter(
    vsCgShaderAttribute *newShaderAttribute,
    vsCgShaderProgramType newWhichProgram,
    char *newVariableName) :
    vsCgParameter(newShaderAttribute, newWhichProgram, newVariableName)
{
    // Create the parameter and add it to the program.
    stateMatrixParameter =
        new osgNVCg::StateMatrixParameter(getCgProgram(), variableName);

    // Add the parameter to the program, in case there will not be a
    // a parameter block to handle this.
    getCgProgram()->addParameter(stateMatrixParameter);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCgStateMatrixParameter::~vsCgStateMatrixParameter()
{
}

// ------------------------------------------------------------------------
// Return the osgNVCg parameter object this object uses.
// ------------------------------------------------------------------------
osgNVCg::Parameter *vsCgStateMatrixParameter::getCgParameter()
{
    return stateMatrixParameter;
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCgStateMatrixParameter::getClassName()
{
    return "vsCgStateMatrixParameter";
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
