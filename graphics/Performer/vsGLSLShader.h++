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
//    Description:  Encapsulates an Performer pfShaderObject (which
//                  encapsulates an OpenGL Shading Language shader object)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_GLSL_SHADER_HPP
#define VS_GLSL_SHADER_HPP

#include "Performer/pr/pfShaderObject.h"
#include "vsObject.h++"

enum vsGLSLShaderType
{
    VS_GLSL_VERTEX_SHADER = PFSHD_VERTEX_SHADER,
    VS_GLSL_FRAGMENT_SHADER = PFSHD_FRAGMENT_SHADER,
    VS_GLSL_UNDEFINED_SHADER = -1
};

class VS_GRAPHICS_DLL vsGLSLShader : public vsObject
{
private:

    pfShaderObject    *performerShader;

public:

                        vsGLSLShader(vsGLSLShaderType type);
                        ~vsGLSLShader();

    const char          *getClassName();

    void                setSource(const char *sourceCode);
    const char          *getSource();
    void                setSourceFile(const char *filename);

    vsGLSLShaderType    getShaderType();

    pfShaderObject      *getBaseLibraryObject();
};

#endif
