#ifndef VS_CG_TEXTURE_PARAMETER_HPP
#define VS_CG_TEXTURE_PARAMETER_HPP

#include "vsCgParameter.h++"
#include "vsTextureAttribute.h++"
#include "vsTextureCubeAttribute.h++"
#include <osgNVCg/TextureParameter>

class vsCgTextureParameter : public vsCgParameter
{
protected:
    osgNVCg::TextureParameter *textureParameter;
    vsStateAttribute          *textureAttribute;

VS_INTERNAL:
    virtual osgNVCg::Parameter    *getCgParameter();

public:
    vsCgTextureParameter(vsCgShaderAttribute *newShaderAttribute,
                         vsCgShaderProgramType newWhichProgram,
                         char *newVariableName);
    ~vsCgTextureParameter();   

    virtual const char        *getClassName();
    virtual vsCgParameterType getCgParameterType();

    void set(vsTextureAttribute *value);
    void set(vsTextureCubeAttribute *value);
};

#endif
