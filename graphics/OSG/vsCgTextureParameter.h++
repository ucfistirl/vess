#ifndef VS_CG_TEXTURE_PARAMETER_HPP
#define VS_CG_TEXTURE_PARAMETER_HPP

#include "vsCgParameter.h++"
#include "vsTextureAttribute.h++"
#include <osgNVCg/TextureParameter>

class vsCgTextureParameter : public vsCgParameter
{
protected:
    osgNVCg::TextureParameter *textureParameter;
    vsTextureAttribute        *textureAttribute;

public:
    vsCgTextureParameter(osgNVCg::Program *newProgram, char *newVariableName);
    ~vsCgTextureParameter();   

    virtual vsCgParameterType getCgParameterType();

    void set(vsTextureAttribute *value);
};

#endif
