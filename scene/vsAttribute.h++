// File vsAttribute.h++

#ifndef VS_ATTRIBUTE_HPP
#define VS_ATTRIBUTE_HPP

class vsAttribute;
class vsNode;

#include <stdlib.h>
#include <string.h>
#include <Performer/pr.h>
#include <Performer/pr/pfMemory.h>
#include "vsAttributeList.h++"

#define VS_ATTRIBUTE_NAME_MAX_LENGTH    80

enum vsAttributeType
{
    VS_ATTRIBUTE_TYPE_TRANSFORM,
    VS_ATTRIBUTE_TYPE_SWITCH,
    VS_ATTRIBUTE_TYPE_SEQUENCE,
    VS_ATTRIBUTE_TYPE_LOD,
    VS_ATTRIBUTE_TYPE_LIGHT,
    VS_ATTRIBUTE_TYPE_FOG,
    VS_ATTRIBUTE_TYPE_MATERIAL,
    VS_ATTRIBUTE_TYPE_TEXTURE,
    VS_ATTRIBUTE_TYPE_TRANSPARENCY,
    VS_ATTRIBUTE_TYPE_BILLBOARD,
    VS_ATTRIBUTE_TYPE_VIEWPOINT,
    VS_ATTRIBUTE_TYPE_BACKFACE
};

class vsAttribute
{
protected:

    char        attributeName[VS_ATTRIBUTE_NAME_MAX_LENGTH];
    
    int         attachedFlag;

VS_INTERNAL:

    int             isAttached();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();

public:

    static void    *operator new(size_t objSize);
    static void    operator delete(void *deadObj);

                   vsAttribute();
    virtual        ~vsAttribute();

    virtual int    getAttributeType() = 0;

    void           setName(char *newName);
    const char     *getName();
};

#endif
