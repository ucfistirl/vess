#ifndef VS_COLLADA_GLSL_EFFECT_HPP
#define VS_COLLADA_GLSL_EFFECT_HPP

#include "vsCOLLADAEffect.h++"

class VESS_SYM vsCOLLADAGLSLEffect : public vsCOLLADAEffect
{
protected:

    vsGLSLProgramAttribute    *program;
    atList                    *shaders;
    atList                    *uniforms;

public:

                                      vsCOLLADAFixedEffect(atString id);
                                      ~vsCOLLADAFixedEffect();

    virtual const char                *getClassName();

    virtual vsCOLLADAEffectType       getEffectType();

    virtual void                      setProgram(vsGLSLProgramAttribute *prog);
    virtual vsGLSLProgramAttribute    *getProgram();
    virtual void                      addShader(vsShaderAttribute *shader);
    virtual vsGLSLShader              *getShader(int index);
    virtual void                      addUniform(vsUniformAttribute *uniform);
    virtual vsGLSLUniform             *getUniform(int index);
};

#endif
