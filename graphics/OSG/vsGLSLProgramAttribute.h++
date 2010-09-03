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

#include "vsStateAttribute.h++"
#include "vsGLSLShader.h++"
#include "vsGLSLUniform.h++"
#include "vsArray.h++"
#include <osg/Program>

#define VS_GPROG_MAX_SHADERS 16
#define VS_GPROG_MAX_UNIFORMS 256
#define VS_GPROG_MAX_BINDINGS 16


class VESS_SYM vsGLSLProgramAttribute : public vsStateAttribute
{
private:

    osg::Program      *osgProgram;

    vsGLSLShader      *shaders[VS_GPROG_MAX_SHADERS];
    int               numShaders;

    vsGLSLUniform     *uniforms[VS_GPROG_MAX_UNIFORMS];
    int               numUniforms;

    char              bindingName[VS_GPROG_MAX_BINDINGS][256];
    int               bindingLocation[VS_GPROG_MAX_BINDINGS];
    int               numVertexAttrBindings;

    vsArray           attachedNodes;

    virtual void      setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    virtual void        attach(vsNode *node);
    virtual void        detach(vsNode *node);
    virtual void        attachDuplicate(vsNode *theNode);
    virtual bool        isEquivalent(vsAttribute *attribute);

public:

                           vsGLSLProgramAttribute();
    virtual                ~vsGLSLProgramAttribute();

    virtual const char     *getClassName();
    virtual int            getAttributeType();
    virtual vsAttribute    *clone();

    void                   addShader(vsGLSLShader *shader);
    void                   removeShader(vsGLSLShader *shader);
    int                    getNumShaders();
    vsGLSLShader           *getShader(int index);

    void                   addUniform(vsGLSLUniform *uniform);
    void                   removeUniform(vsGLSLUniform *uniform);
    int                    getNumUniforms();
    vsGLSLUniform          *getUniform(int index);
    vsGLSLUniform          *getUniform(const char *name);

    void                   bindVertexAttr(const char *name, unsigned int loc);
    void                   removeVertexAttrBinding(const char *name);
    int                    getNumVertexAttrBindings();
    void                   getVertexAttrBinding(int index, char **name,
                                                unsigned int *loc);
};

#endif
