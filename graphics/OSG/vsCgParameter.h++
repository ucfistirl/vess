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

#define VARIABLE_NAME_MAX 64

enum vsCgParameterType
{
    VS_CG_VECTOR_PARAMETER,
    VS_CG_MATRIX_PARAMETER,
    VS_CG_STATE_MATRIX_PARAMETER,
    VS_CG_TEXTURE_PARAMETER
};

class vsCgParameter
{
protected:
    osgNVCg::Program *program;
    char variableName[VARIABLE_NAME_MAX];

public:
                                 vsCgParameter(osgNVCg::Program *newProgram);
                                 ~vsCgParameter();

    osgNVCg::Program             *getProgram();
    char                         *getVariableName();
    virtual vsCgParameterType    getCgParameterType() = 0;
};

#endif
