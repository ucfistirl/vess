#ifndef VS_COLLADA_TEXTURE_SET_HPP
#define VS_COLLADA_TEXTURE_SET_HPP

#include "vsObject.h++"
#include "vsList.h++"
#include "vsTextureAttribute.h++"

class VESS_SYM vsCOLLADATextureSet : public vsObject
{
protected:

    vsList    *textureList;

public:

                           vsCOLLADATextureSet();
    virtual                ~vsCOLLADATextureSet();

    virtual const char     *getClassName();

    vsCOLLADATextureSet    *clone();

    void                   addTexture(vsTextureAttribute *newTexture);
    vsList                 *getTextureList();
};

#endif
