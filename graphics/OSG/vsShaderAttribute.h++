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
//    VESS Module:  vsShaderAttribute.h++
//
//    Description:  Attribute to handle standard OpenGL ARB_vertex_program
//                  and ARB_fragment_program shaders.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_SHADER_ATTRIBUTE_HPP
#define VS_SHADER_ATTRIBUTE_HPP

#include "vsStateAttribute.h++"
#include <osg/VertexProgram>
#include <osg/FragmentProgram>

class VESS_SYM vsShaderAttribute : public vsStateAttribute
{
private:

    osg::VertexProgram     *vertexProgram;
    char                   *vertexProgramFile;
    char                   *vertexProgramSource;

    osg::FragmentProgram   *fragmentProgram;
    char                   *fragmentProgramFile;
    char                   *fragmentProgramSource;

    virtual void           setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    virtual void        attach(vsNode *node);
    virtual void        detach(vsNode *node);
    virtual void        attachDuplicate(vsNode *theNode);
    virtual bool        isEquivalent(vsAttribute *attribute);

public:

                           vsShaderAttribute();
    virtual                ~vsShaderAttribute();

    virtual const char     *getClassName();
    virtual int            getAttributeType();
    virtual vsAttribute    *clone();

    void                   setVertexSourceFile(char *filename);
    char                   *getVertexSourceFile();
    void                   setVertexSource(char *source);
    char                   *getVertexSource();

    void                   setFragmentSourceFile(char *filename);
    char                   *getFragmentSourceFile();
    void                   setFragmentSource(char *source);
    char                   *getFragmentSource();

    void                   setVertexLocalParameter(int index, float x);
    void                   setVertexLocalParameter(int index, float x, float y);
    void                   setVertexLocalParameter(int index, float x, float y,
                                                   float z);
    void                   setVertexLocalParameter(int index, float x, float y,
                                                   float z, float w);
    void                   setVertexLocalParameter(int index,
                                                   const atVector &value);
    atVector               getVertexLocalParameter(int index);

    void                   setFragmentLocalParameter(int index, float x);
    void                   setFragmentLocalParameter(int index, float x,
                                                     float y);
    void                   setFragmentLocalParameter(int index, float x,
                                                     float y, float z);
    void                   setFragmentLocalParameter(int index, float x,
                                                     float y, float z, float w);
    void                   setFragmentLocalParameter(int index,
                                                     const atVector &value);
    atVector               getFragmentLocalParameter(int index);
};

#endif
