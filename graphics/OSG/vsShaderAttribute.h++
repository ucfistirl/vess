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

#include <osg/VertexProgram>
#include <osg/FragmentProgram>
#include "vsStateAttribute.h++"
#include "vsGrowableArray.h++"

class VS_GRAPHICS_DLL vsShaderAttribute : public vsStateAttribute
{
private:

    osg::VertexProgram     *vertexProgram;
    char                   *vertexProgramFile;
    char                   *vertexProgramSource;

    osg::FragmentProgram   *fragmentProgram;
    char                   *fragmentProgramFile;
    char                   *fragmentProgramSource;

    virtual void        setOSGAttrModes(vsNode *node);

    vsGrowableArray     *vertexParameterArray;
    vsGrowableArray     *fragmentParameterArray;

VS_INTERNAL:

    virtual void        attach(vsNode *node);
    virtual void        detach(vsNode *node);
    virtual void        attachDuplicate(vsNode *theNode);
    virtual bool        isEquivalent(vsAttribute *attribute);

public:

                          vsShaderAttribute();
    virtual               ~vsShaderAttribute();

    virtual const char    *getClassName();
    virtual int           getAttributeType();

    void                  setVertexSourceFile(char *filename);
    char                  *getVertexSourceFile();
    void                  setVertexSource(char *source);
    char                  *getVertexSource();

    void                  setFragmentSourceFile(char *filename);
    char                  *getFragmentSourceFile();
    void                  setFragmentSource(char *source);
    char                  *getFragmentSource();

    void                  setVertexLocalParameter(int index, float x);
    void                  setVertexLocalParameter(int index, float x, float y);
    void                  setVertexLocalParameter(int index, float x, float y,
                                                  float z);
    void                  setVertexLocalParameter(int index, float x, float y,
                                                  float z, float w);
    void                  setVertexLocalParameter(int index,
                                                  const vsVector &value);
    vsVector              getVertexLocalParameter(int index);

    void                  setFragmentLocalParameter(int index, float x);
    void                  setFragmentLocalParameter(int index, float x,
                                                    float y);
    void                  setFragmentLocalParameter(int index, float x, float y,
                                                    float z);
    void                  setFragmentLocalParameter(int index, float x, float y,
                                                    float z, float w);
    void                  setFragmentLocalParameter(int index,
                                                    const vsVector &value);
    vsVector              getFragmentLocalParameter(int index);
};

#endif