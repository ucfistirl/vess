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
//    Description:  Encapsulates an OpenSceneGraph Shader object (which
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
    // Create the OSG version of the shader
    osgShader = new osg::Shader((osg::Shader::Type)type);
    osgShader->ref();

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
    // Tell OSG we don't need this shader anymore
    osgShader->unref();
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
    // Pass the source code along to the OSG shader
    osgShader->setShaderSource(std::string(sourceCode));
}

// ------------------------------------------------------------------------
// Retrieves the source code for this shader
// ------------------------------------------------------------------------
const char *vsGLSLShader::getSource()
{
    return osgShader->getShaderSource().c_str();
}

// ------------------------------------------------------------------------
// Loads the shader's source code from the given file
// ------------------------------------------------------------------------
void vsGLSLShader::setSourceFile(const char *filename)
{
    // Pass the filename to the OSG shader, so it can load the source code
    osgShader->loadShaderSourceFromFile(std::string(filename));
}

// ------------------------------------------------------------------------
// Return the type of shader (vertex or fragment)
// ------------------------------------------------------------------------
vsGLSLShaderType vsGLSLShader::getShaderType()
{
    // Get the OSG shader type and return it
    return (vsGLSLShaderType)osgShader->getType();
}

// ------------------------------------------------------------------------
// Returns the osg::Shader that this object encapsulates
// ------------------------------------------------------------------------
osg::Shader *vsGLSLShader::getBaseLibraryObject()
{
    return osgShader;
}
