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
//    VESS Module:  vsGLSLUniform.c++
//
//    Description:  Encapsulates an OpenSceneGraph Uniform object (which
//                  encapsulates an OpenGL Shading Language uniform 
//                  attribute)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_GLSL_UNIFORM_HPP
#define VS_GLSL_UNIFORM_HPP

#include "osg/Uniform"
#include "vsObject.h++"
#include "atVector.h++"
#include "atMatrix.h++"

enum vsGLSLUniformType
{
    VS_UNIFORM_FLOAT = GL_FLOAT,
    VS_UNIFORM_FLOAT_VEC2 = GL_FLOAT_VEC2,
    VS_UNIFORM_FLOAT_VEC3 = GL_FLOAT_VEC3,
    VS_UNIFORM_FLOAT_VEC4 = GL_FLOAT_VEC4,
    VS_UNIFORM_INT = GL_INT,
    VS_UNIFORM_INT_VEC2 = GL_INT_VEC2,
    VS_UNIFORM_INT_VEC3 = GL_INT_VEC3,
    VS_UNIFORM_INT_VEC4 = GL_INT_VEC4,
    VS_UNIFORM_BOOL = GL_BOOL,
    VS_UNIFORM_BOOL_VEC2 = GL_BOOL_VEC2,
    VS_UNIFORM_BOOL_VEC3 = GL_BOOL_VEC3,
    VS_UNIFORM_BOOL_VEC4 = GL_BOOL_VEC4,
    VS_UNIFORM_FLOAT_MAT2 = GL_FLOAT_MAT2,
    VS_UNIFORM_FLOAT_MAT3 = GL_FLOAT_MAT3,
    VS_UNIFORM_FLOAT_MAT4 = GL_FLOAT_MAT4,
    VS_UNIFORM_SAMPLER_1D = GL_SAMPLER_1D,
    VS_UNIFORM_SAMPLER_2D = GL_SAMPLER_2D,
    VS_UNIFORM_SAMPLER_3D = GL_SAMPLER_3D,
    VS_UNIFORM_SAMPLER_1D_SHADOW = GL_SAMPLER_1D_SHADOW,
    VS_UNIFORM_SAMPLER_2D_SHADOW = GL_SAMPLER_2D_SHADOW,
    VS_UNIFORM_UNDEFINED = -1
};

#define VS_UNIFORM_NAME_LENGTH 128

class VESS_SYM vsGLSLUniform : public vsObject
{
private:

    osg::Uniform    *osgUniform;

    u_long          elementCount;

    char            uniformName[VS_UNIFORM_NAME_LENGTH];

public:

                         vsGLSLUniform(const char *name, 
                                       vsGLSLUniformType type);
                         vsGLSLUniform(const char *name, 
                                       vsGLSLUniformType type,
                                       u_long numElements);
                         ~vsGLSLUniform();

    const char           *getClassName();

    const char           *getName();

    vsGLSLUniformType    getType();

    void                 set(bool b1);
    void                 set(bool b1, bool b2);
    void                 set(bool b1, bool b2, bool b3);
    void                 set(bool b1, bool b2, bool b3, bool b4);
    void                 set(int i1);
    void                 set(int i1, int i2);
    void                 set(int i1, int i2, int i3);
    void                 set(int i1, int i2, int i3, int i4);
    void                 set(float floatVal);
    void                 set(double doubleVal);
    void                 set(atVector vec);
    void                 set(atMatrix mat);
    void                 set(int size, atMatrix mat);

    void                 setEntry(u_long index, bool b1);
    void                 setEntry(u_long index, bool b1, bool b2);
    void                 setEntry(u_long index, bool b1, bool b2, bool b3);
    void                 setEntry(u_long index, bool b1, bool b2, bool b3,
                                  bool b4);
    void                 setEntry(u_long index, int i1);
    void                 setEntry(u_long index, int i1, int i2);
    void                 setEntry(u_long index, int i1, int i2, int i3);
    void                 setEntry(u_long index, int i1, int i2, int i3, int i4);
    void                 setEntry(u_long index, float floatVal);
    void                 setEntry(u_long index, double doubleVal);
    void                 setEntry(u_long index, atVector vec);
    void                 setEntry(u_long index, atMatrix mat);
    void                 setEntry(u_long index, int size, atMatrix mat);

    osg::Uniform         *getBaseLibraryObject();
};

#endif
