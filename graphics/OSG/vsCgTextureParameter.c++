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
vsCgTextureParameter::vsCgTextureParameter(
    vsCgShaderAttribute *newShaderAttribute,
    vsCgShaderProgramType newWhichProgram,
    char *newVariableName) :
    vsCgParameter(newShaderAttribute, newWhichProgram, newVariableName)
{
    // Create the parameter and add it to the program.
    textureParameter = new osgNVCg::TextureParameter(getCgProgram(),
        variableName);
    textureParameter->ref();

    // Add the parameter to the program, in case there will not be a
    // a parameter block to handle this.
    getCgProgram()->addParameter(textureParameter);

    // Initialize the texture attribute to none.
    textureAttribute = NULL;
}

// ------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------
vsCgTextureParameter::~vsCgTextureParameter()
{
    // If there is a textureAttribute in use, unrefDelete it.
    if (textureAttribute)
        vsObject::unrefDelete(textureAttribute);

    // Unreference the OSG object.
    textureParameter->unref();
}

// ------------------------------------------------------------------------
// Gets a string representation of this object's class name
// ------------------------------------------------------------------------
const char *vsCgTextureParameter::getClassName()
{
    return "vsCgTextureParameter";
}

// ------------------------------------------------------------------------
// Return the osgNVCg parameter object this object uses.
// ------------------------------------------------------------------------
osgNVCg::Parameter *vsCgTextureParameter::getCgParameter()
{
    return textureParameter;
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
    if (textureAttribute)
        vsObject::unrefDelete(textureAttribute);

    // Store a reference to the texture attribute, so it is not prematurely
    // deleted.
    textureAttribute = value;
    textureAttribute->ref();

    // Give the osgNV parameter object the OSG texture.
    textureParameter->set(value->getBaseLibraryObject());
}

// ------------------------------------------------------------------------
// Get the osg::Texture object which vsTextureCubeAttribute uses and hand it
// to the osgNVCg texture parameter.
// ------------------------------------------------------------------------
void vsCgTextureParameter::set(vsTextureCubeAttribute *value)
{
    // If there was a previous textureAttribute in use, unrefDelete it.
    if (textureAttribute)
        vsObject::unrefDelete(textureAttribute);
                                                                                
    // Store a reference to the texture attribute, so it is not prematurely
    // deleted.
    textureAttribute = value;
    textureAttribute->ref();
                                                                                
    // Give the osgNV parameter object the OSG texture.
    textureParameter->set(value->getBaseLibraryObject());
}

