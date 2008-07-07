
#ifndef VS_SKIN_PROGRAM_NODE_HPP
#define VS_SKIN_PROGRAM_NODE_HPP

#include "vsObject.h++"

#include "vsSkin.h++"
#include "vsGLSLProgramAttribute.h++"

class vsSkinProgramNode : public vsObject
{
protected:

    vsSkin                    *skin;
    vsGLSLProgramAttribute    *program;

public:

                              vsSkinProgramNode(vsSkin *theSkin,
                                             vsGLSLProgramAttribute *theProg);
                              ~vsSkinProgramNode();

    const char                *getClassName();

    vsSkin                    *getSkin();
    vsGLSLProgramAttribute    *getProgram();
    void                      setProgram(vsGLSLProgramAttribute *prog);
};

#endif

