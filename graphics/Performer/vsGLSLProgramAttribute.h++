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
//    VESS Module:  vsGLSLProgramAttribute.h++
//
//    Description:  Attribute to handle OpenGL Shading Language (GLSL) 
//                  shader programs.
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#ifndef VS_GLSL_PROGRAM_ATTRIBUTE_HPP
#define VS_GLSL_PROGRAM_ATTRIBUTE_HPP

#include <Performer/pr/pfShaderProgram.h>
#include <Performer/pf/pfTraverser.h>
#include "vsGLSLShader.h++"
#include "vsGLSLUniform.h++"
#include "vsStateAttribute.h++"
#include "vsGrowableArray.h++"
#include "vsVector.h++"

#define VS_GPROG_MAX_SHADERS 8
#define VS_GPROG_MAX_UNIFORMS 32

#define VS_GPROG_MAX_ATTR_BINDINGS 16
#define VS_GPROG_ATTR_NAME_LENGTH 128

class VS_GRAPHICS_DLL vsGLSLProgramAttribute : public vsStateAttribute
{
private:

    pfShaderProgram      *performerProgram;

    vsGLSLShader         *shaders[VS_GPROG_MAX_SHADERS];
    int                  numShaders;

    vsGLSLUniform        *uniforms[VS_GPROG_MAX_UNIFORMS];
    int                  numUniforms;

    bool                 attrBindingsChanged;

    char   attrBindings[VS_GPROG_MAX_ATTR_BINDINGS][VS_GPROG_ATTR_NAME_LENGTH];

    static int           travCallback(pfTraverser *trav, void *userData);

VS_INTERNAL:

    virtual void        attach(vsNode *theNode);
    virtual void        detach(vsNode *theNode);
    virtual void        attachDuplicate(vsNode *theNode);
    virtual void        saveCurrent();
    virtual void        apply();
    virtual void        restoreSaved();
    virtual void        setState(pfGeoState *state);
    virtual bool        isEquivalent(vsAttribute *attribute);

public:

                          vsGLSLProgramAttribute();
    virtual               ~vsGLSLProgramAttribute();

    virtual const char    *getClassName();
    virtual int           getAttributeType();

    void                  addShader(vsGLSLShader *shader);
    void                  removeShader(vsGLSLShader *shader);
    int                   getNumShaders();
    vsGLSLShader          *getShader(int index);

    void                  addUniform(vsGLSLUniform *uniform);
    void                  removeUniform(vsGLSLUniform *uniform);
    int                   getNumUniforms();
    vsGLSLUniform         *getUniform(int index);

    void                  bindVertexAttr(const char *name, unsigned int index);
    void                  removeVertexAttrBinding(const char *name);
};

#endif
