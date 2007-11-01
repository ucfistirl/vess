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
//    VESS Module:  vsCgMatrixParameter.h++
//
//    Description:  Class for managing a Cg matrix parameter.  Setting a
//                  value to this object will set the value to the Cg
//                  variable name it is linked to.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CG_MATRIX_PARAMETER_HPP
#define VS_CG_MATRIX_PARAMETER_HPP

#include "atMatrix.h++"
#include "vsCgParameter.h++"
#include <osg/Matrix>
#include <osgNVCg/MatrixParameter>

class vsCgMatrixParameter : public vsCgParameter
{
protected:
    osgNVCg::MatrixParameter    *matrixParameter;
    osg::Matrix                 osgMatrix;

VS_INTERNAL:
    virtual osgNVCg::Parameter    *getCgParameter();

public:
    vsCgMatrixParameter(vsCgShaderAttribute *newShaderAttribute,
                        vsCgShaderProgramType newWhichProgram,
                        char *newVariableName);
    ~vsCgMatrixParameter();   

    virtual const char           *getClassName();
    virtual vsCgParameterType    getCgParameterType();

    void                         set(const atMatrix &value);
};

#endif
