#ifndef VS_COLLADA_TEXTURE_SET_HPP
#define VS_COLLADA_TEXTURE_SET_HPP

#include "vsObject.h++"
#include "atList.h++"
#include "vsTextureAttribute.h++"

class VESS_SYM vsCOLLADATextureSet : public vsObject
{
protected:

    atList    *textureList;

public:

                           vsCOLLADATextureSet();
    virtual                ~vsCOLLADATextureSet();

    virtual const char     *getClassName();

    vsCOLLADATextureSet    *clone();

    void                   addTexture(vsTextureAttribute *newTexture);
    atList                 *getTextureList();
};

#endif
