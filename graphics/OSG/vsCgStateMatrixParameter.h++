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
//    VESS Module:  vsCgStateMatrixParameter.h++
//
//    Description:  Class for managing a Cg state matrix parameter.
//                  This object takes enumerated identifiers that specify
//                  which state matrix to set and what operations to
//                  perform on it.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CG_STATE_MATRIX_PARAMETER_HPP
#define VS_CG_STATE_MATRIX_PARAMETER_HPP

#include "vsCgParameter.h++"
#include <osgNVCg/StateMatrixParameter>

enum vsCgStateMatrix
{
    VS_MODELVIEW_MATRIX = osgNVCg::StateMatrixParameter::MODELVIEW,
    VS_PROJECTION_MATRIX = osgNVCg::StateMatrixParameter::PROJECTION,
    VS_TEXTURE_MATRIX = osgNVCg::StateMatrixParameter::TEXTURE,
    VS_MODELVIEW_PROJECTION_MATRIX =
      osgNVCg::StateMatrixParameter::MODELVIEW_PROJECTION
};

enum vsCgStateMatrixMapping
{
    VS_IDENTITY_MAPPING = osgNVCg::StateMatrixParameter::IDENTITY,
    VS_TRANSPOSE_MAPPING = osgNVCg::StateMatrixParameter::TRANSPOSE,
    VS_INVERSE_MAPPING = osgNVCg::StateMatrixParameter::INVERSE,
    VS_INVERSE_TRANSPOSE_MAPPING =
      osgNVCg::StateMatrixParameter::INVERSE_TRANSPOSE 
};

class vsCgStateMatrixParameter : public vsCgParameter
{
protected:
    osgNVCg::StateMatrixParameter    *stateMatrixParameter;

VS_INTERNAL:
    virtual osgNVCg::Parameter       *getCgParameter();

public:
    vsCgStateMatrixParameter(vsCgShaderAttribute *newShaderAttribute,
                             vsCgShaderProgramType newWhichProgram,
                             char *newVariableName);
    ~vsCgStateMatrixParameter();   

    virtual const char           *getClassName();
    virtual vsCgParameterType    getCgParameterType();

    void                         set(vsCgStateMatrix matrix);
    void                         set(vsCgStateMatrix matrix,
                                     vsCgStateMatrixMapping mapping);
};

#endif
