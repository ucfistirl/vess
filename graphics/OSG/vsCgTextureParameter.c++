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
//    VESS Module:  vsCgTextureParameter.c++
//
//    Description:  Class for managing a Cg sampler parameter.  Setting a
//                  value to this object will set the value to the Cg
//                  variable name it is linked to.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#include "vsCgTextureParameter.h++"

// ------------------------------------------------------------------------
// Constructor - Copy the variable name and create the osgNVCg parameter.
// ------------------------------------------------------------------------
vsCgTextureParameter::vsCgTextureParameter(osgNVCg::Program *newProgram,
                                         char *newVariableName) :
                                         vsCgParameter(newProgram)
{
    // Keep a copy of the variable name.
    strncpy(variableName, newVariableName, VARIABLE_NAME_MAX);

    // Create the parameter and add it to the program.
    textureParameter = new osgNVCg::TextureParameter(program, variableName);
    program->addParameter(textureParameter);

    // Initialize the texture attribute to none.
    textureAttribute = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCgTextureParameter::~vsCgTextureParameter()
{
    // If there is a textureAttribute in use, unrefDelete it.
    if (textureAttribute != NULL)
        vsObject::unrefDelete(textureAttribute);
}

// ------------------------------------------------------------------------
// Return the type of parameter object this object is.
// ------------------------------------------------------------------------
vsCgParameterType vsCgTextureParameter::getCgParameterType()
{
    return VS_CG_TEXTURE_PARAMETER;
}

// ------------------------------------------------------------------------
// Get the osg::Texture object which vsTextureAttribute uses and hand it
// to the osgNVCg texture parameter.
// ------------------------------------------------------------------------
void vsCgTextureParameter::set(vsTextureAttribute *value)
{
    // If there was a previous textureAttribute in use, unrefDelete it.
    if (textureAttribute != NULL)
        vsObject::unrefDelete(textureAttribute);

    // Store a reference to the texture attribute, so it is not prematurely
    // deleted.
    textureAttribute = value;
    textureAttribute->ref();

    // Give the osgNV parameter object the OSG texture.
    textureParameter->set(textureAttribute->getBaseLibraryObject());
}
