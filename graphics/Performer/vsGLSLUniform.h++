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

#include "vsObject.h++"
#include "atVector.h++"
#include "atMatrix.h++"
#include <Performer/pr/pfShaderProgram.h>

enum vsGLSLUniformType
{
    VS_UNIFORM_FLOAT = PFUNI_FLOAT1,
    VS_UNIFORM_FLOAT_VEC2 = PFUNI_FLOAT2,
    VS_UNIFORM_FLOAT_VEC3 = PFUNI_FLOAT3,
    VS_UNIFORM_FLOAT_VEC4 = PFUNI_FLOAT4,
    VS_UNIFORM_INT = PFUNI_INT1,
    VS_UNIFORM_INT_VEC2 = PFUNI_INT2,
    VS_UNIFORM_INT_VEC3 = PFUNI_INT3,
    VS_UNIFORM_INT_VEC4 = PFUNI_INT4,
    VS_UNIFORM_BOOL = PFUNI_BOOL1,
    VS_UNIFORM_BOOL_VEC2 = PFUNI_BOOL2,
    VS_UNIFORM_BOOL_VEC3 = PFUNI_BOOL3,
    VS_UNIFORM_BOOL_VEC4 = PFUNI_BOOL4,
    VS_UNIFORM_FLOAT_MAT2 = PFUNI_MAT2,
    VS_UNIFORM_FLOAT_MAT3 = PFUNI_MAT3,
    VS_UNIFORM_FLOAT_MAT4 = PFUNI_MAT4,
    VS_UNIFORM_SAMPLER_1D = PFUNI_SAMP1D,
    VS_UNIFORM_SAMPLER_2D = PFUNI_SAMP2D,
    VS_UNIFORM_SAMPLER_3D = PFUNI_SAMP3D,
    VS_UNIFORM_SAMPLER_1D_SHADOW = PFUNI_SAMP1DSHADOW,
    VS_UNIFORM_SAMPLER_2D_SHADOW = PFUNI_SAMP2DSHADOW,
    VS_UNIFORM_UNDEFINED = -1
};

union vsGLSLUniformData
{
    bool                 boolVecData[4];
    int                  intVecData[4];
    float                floatData[16];
    int                  samplerData;
};

#define VS_GLSL_UNIFORM_NAME_MAX 256
#define VS_GLSL_UNIFORM_MAX_PARENTS 32

class VS_GRAPHICS_DLL vsGLSLUniform : public vsObject
{
private:

    char                 uniformName[VS_GLSL_UNIFORM_NAME_MAX];
    vsGLSLUniformType    uniformType;
    vsGLSLUniformData    *uniformData;

    pfShaderProgram      *parentPrograms[VS_GLSL_UNIFORM_MAX_PARENTS];
    GLint                parentUniformIndex[VS_GLSL_UNIFORM_MAX_PARENTS];
    int                  numParentPrograms;

    void                 updateParentPrograms();

VS_INTERNAL:

    void                 addParentProgram(pfShaderProgram *parent);
    void                 removeParentProgram(pfShaderProgram *parent);

public:

                         vsGLSLUniform(const char *name, 
                                       vsGLSLUniformType type);
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
};

#endif
