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
//    VESS Module:  vsGLSLShader.c++
//
//    Description:  Encapsulates a Performer pfShaderObject (which
//                  encapsulates an OpenGL Shading Language shader object)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsGLSLShader.h++"
#include <string>

// ------------------------------------------------------------------------
// Creates a vsGLSLShader with the given shader type
// ------------------------------------------------------------------------
vsGLSLShader::vsGLSLShader(vsGLSLShaderType type)
{
    // Create the Performer version of the shader
    performerShader = new pfShaderObject();
    performerShader->ref();
    performerShader->setShaderType((int)type);

    // Complain if the user tries to create an UNDEFINED shader
    if (type == VS_GLSL_UNDEFINED_SHADER)
        printf("vsGLSLShader::vsGLSLShader:  Creating a shader with an"
            " undefined type!\n");
}

// ------------------------------------------------------------------------
// Destroys this vsGLSLShader
// ------------------------------------------------------------------------
vsGLSLShader::~vsGLSLShader()
{
    // Tell Performer we don't need this shader anymore
    pfMemory::unrefDelete(performerShader);
}

// ------------------------------------------------------------------------
// Return the name of this class
// ------------------------------------------------------------------------
const char *vsGLSLShader::getClassName()
{
    return "vsGLSLShader";
}

// ------------------------------------------------------------------------
// Sets the source code to be compiled and used by the shader
// ------------------------------------------------------------------------
void vsGLSLShader::setSource(const char *sourceCode)
{
    // Pass the source code along to the Performer shader
    performerShader->setSource((GLcharARB *)sourceCode);
}

// ------------------------------------------------------------------------
// Retrieves the source code for this shader
// ------------------------------------------------------------------------
const char *vsGLSLShader::getSource()
{
    return performerShader->getSource();
}

// ------------------------------------------------------------------------
// Loads the shader's source code from the given file
// ------------------------------------------------------------------------
void vsGLSLShader::setSourceFile(const char *filename)
{
    // Pass the filename to the Performer shader, so it can load the source 
    // code (yes, I think setName() is a lousy name for this function too).
    performerShader->setName(filename);

    // Load the shader source
    performerShader->load();
}

// ------------------------------------------------------------------------
// Return the type of shader (vertex or fragment)
// ------------------------------------------------------------------------
vsGLSLShaderType vsGLSLShader::getShaderType()
{
    int shaderType;

    // Get the Performer shader type and return the VESS version of it
    shaderType = performerShader->getShaderType();
    switch (shaderType)
    {
        case PFSHD_VERTEX_SHADER:
            return VS_GLSL_VERTEX_SHADER;
        case PFSHD_FRAGMENT_SHADER:
            return VS_GLSL_FRAGMENT_SHADER;
        default:
            return VS_GLSL_UNDEFINED_SHADER;
    };
}

// ------------------------------------------------------------------------
// Returns the osg::Shader that this object encapsulates
// ------------------------------------------------------------------------
pfShaderObject *vsGLSLShader::getBaseLibraryObject()
{
    return performerShader;
}
