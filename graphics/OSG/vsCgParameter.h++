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
//    VESS Module:  vsCgParameter.h++
//
//    Description:  Abstract class for all Cg Parameter object.  Provides
//                  some basic common features.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CG_PARAMETER_HPP
#define VS_CG_PARAMETER_HPP

#include <osgNVCg/Program>
#include <osgNVCg/Parameter>
#include "vsObject.h++"
#include "vsCgShaderAttribute.h++"

#define VARIABLE_NAME_MAX 64

enum vsCgParameterType
{
    VS_CG_VECTOR_PARAMETER,
    VS_CG_MATRIX_PARAMETER,
    VS_CG_STATE_MATRIX_PARAMETER,
    VS_CG_TEXTURE_PARAMETER
};

class vsCgParameter : public vsObject
{
protected:
    vsCgShaderAttribute      *shaderAttribute;
    vsCgShaderProgramType    whichProgram;
    char variableName[VARIABLE_NAME_MAX];

VS_INTERNAL:
    osgNVCg::Program             *getCgProgram();
    virtual osgNVCg::Parameter   *getCgParameter() = 0;

public:
                                 vsCgParameter(
                                        vsCgShaderAttribute *newShaderAttribute,
                                        vsCgShaderProgramType newWhichProgram,
                                        char *newVariableName);
                                 ~vsCgParameter();

    vsCgShaderAttribute          *getCgShaderAttribute();
    vsCgShaderProgramType        getCgShaderProgramType();
    char                         *getCgVariableName();

    virtual vsCgParameterType    getCgParameterType() = 0;
};

#endif
