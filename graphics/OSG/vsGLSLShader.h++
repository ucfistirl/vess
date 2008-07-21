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
#ifndef VS_GLSL_SHADER_HPP
#define VS_GLSL_SHADER_HPP

#include "osg/Shader"
#include "vsObject.h++"

enum vsGLSLShaderType
{
    VS_GLSL_VERTEX_SHADER = osg::Shader::VERTEX,
    VS_GLSL_FRAGMENT_SHADER = osg::Shader::FRAGMENT,
    VS_GLSL_UNDEFINED_SHADER = osg::Shader::UNDEFINED
};

class VESS_SYM vsGLSLShader : public vsObject
{
private:

    osg::Shader    *osgShader;

public:

                        vsGLSLShader(vsGLSLShaderType type);
                        ~vsGLSLShader();

    const char          *getClassName();

    void                setSource(const char *sourceCode);
    const char          *getSource();
    void                setSourceFile(const char *filename);

    vsGLSLShaderType    getShaderType();

    osg::Shader         *getBaseLibraryObject();
};

#endif
