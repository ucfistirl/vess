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

#include "vsGlobals.h++"
#include <Performer/pr/pfVertexProgram.h>
#include <Performer/pr/pfFragmentProgram.h>
#include <Performer/pr/pfGProgramParms.h>
#include "vsStateAttribute.h++"
#include "vsGrowableArray.h++"
#include "atVector.h++"

class VS_GRAPHICS_DLL vsShaderAttribute : public vsStateAttribute
{
private:

    pfVertexProgram      *vertexProgram;
    pfGProgramParms      *vertexParameters;
    char                 *vertexProgramFile;
    char                 *vertexProgramSource;
    int                  vertexParameterCount;

    pfFragmentProgram    *fragmentProgram;
    pfGProgramParms      *fragmentParameters;
    char                 *fragmentProgramFile;
    char                 *fragmentProgramSource;
    int                  fragmentParameterCount;

    vsGrowableArray     *vertexParameterArray;
    vsGrowableArray     *fragmentParameterArray;

VS_INTERNAL:

    virtual void        attachDuplicate(vsNode *theNode);
    virtual void        saveCurrent();
    virtual void        apply();
    virtual void        restoreSaved();
    virtual void        setState(pfGeoState *state);
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
                                                  const atVector &value);
    atVector              getVertexLocalParameter(int index);

    void                  setFragmentLocalParameter(int index, float x);
    void                  setFragmentLocalParameter(int index, float x,
                                                    float y);
    void                  setFragmentLocalParameter(int index, float x, float y,
                                                    float z);
    void                  setFragmentLocalParameter(int index, float x, float y,
                                                    float z, float w);
    void                  setFragmentLocalParameter(int index,
                                                    const atVector &value);
    atVector              getFragmentLocalParameter(int index);
};

#endif
