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
//    VESS Module:  vsCgVectorParameter.h++
//
//    Description:  Class for managing a Cg vector parameter from 1 to 4
//                  dimensions.  Setting a value to this object will
//                  set the value to the Cg variable name it is linked to.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_CG_VECTOR_PARAMETER_HPP
#define VS_CG_VECTOR_PARAMETER_HPP

#include "vsVector.h++"
#include "vsCgParameter.h++"
#include <osgNVCg/VectorParameter>

class vsCgVectorParameter : public vsCgParameter
{
protected:
    osgNVCg::VectorParameter *vectorParameter;

public:
    vsCgVectorParameter(osgNVCg::Program *newProgram, char *newVariableName);
    ~vsCgVectorParameter();   

    virtual vsCgParameterType getCgParameterType();

    void set(double x);
    void set(double x, double y);
    void set(double x, double y, double z);
    void set(double x, double y, double z, double w);
    void set(vsVector value);
};

#endif
