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
//    VESS Module:  vsCgVectorParameter.c++
//
//    Description:  Class for managing a Cg vector parameter from 1 to 4
//                  dimensions.  Setting a value to this object will
//                  set the value to the Cg variable name it is linked to.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCgVectorParameter.h++"

// ------------------------------------------------------------------------
// Constructor - Copy the variable name and create the osgNVCg parameter.
// ------------------------------------------------------------------------
vsCgVectorParameter::vsCgVectorParameter(osgNVCg::Program *newProgram,
                                         char *newVariableName) :
                                         vsCgParameter(newProgram)
{
    // Keep a copy of the variable name.
    strncpy(variableName, newVariableName, VARIABLE_NAME_MAX);

    // Create the parameter and add it to the program.
    vectorParameter = new osgNVCg::VectorParameter(program, variableName);
    program->addParameter(vectorParameter);
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCgVectorParameter::~vsCgVectorParameter()
{
}

// ------------------------------------------------------------------------
// Return the type of parameter object this object is.
// ------------------------------------------------------------------------
vsCgParameterType vsCgVectorParameter::getCgParameterType()
{
    return VS_CG_VECTOR_PARAMETER;
}

// ------------------------------------------------------------------------
// Set only the first value of the vector.
// ------------------------------------------------------------------------
void vsCgVectorParameter::set(double x)
{
    // Set the parameter to the new values.
    vectorParameter->set(x);
}

// ------------------------------------------------------------------------
// Set only the first two values of the vector.
// ------------------------------------------------------------------------
void vsCgVectorParameter::set(double x, double y)
{
    // Set the parameter to the new values.
    vectorParameter->set(x, y);
}

// ------------------------------------------------------------------------
// Set the first three values of the vector.
// ------------------------------------------------------------------------
void vsCgVectorParameter::set(double x, double y, double z)
{
    // Set the parameter to the new values.
    vectorParameter->set(x, y, z);
}

// ------------------------------------------------------------------------
// Set the four values of the vector.
// ------------------------------------------------------------------------
void vsCgVectorParameter::set(double x, double y, double z, double w)
{
    // Set the parameter to the new values.
    vectorParameter->set(x, y, z, w);
}

// ------------------------------------------------------------------------
// Set the values according to the given vsVector.
// ------------------------------------------------------------------------
void vsCgVectorParameter::set(vsVector value)
{
    double v[4];
    int loop, size;

    // Initialized the values to zero.
    v[0] = v[1] = v[2] = v[3] = 0.0;

    // Get the size of the vector.
    size = value.getSize();

    // Get the valid values from the vector, depending on its size.
    for (loop = 0; loop < size; loop++)
        v[loop] = value.getValue(loop);

    // Set the parameter to the new values, unused ones should be zero.
    vectorParameter->set(v[0], v[1], v[2], v[3]);
}