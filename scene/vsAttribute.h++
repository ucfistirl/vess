// File vsAttribute.h++

#ifndef VS_ATTRIBUTE_HPP
#define VS_ATTRIBUTE_HPP

#include <stdlib.h>
#include <Performer/pr/pfGeoState.h>
#include "vsGlobals.h++"

class vsNode;

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
    VS_ATTRIBUTE_TYPE_BACKFACE,
    VS_ATTRIBUTE_TYPE_DECAL,
    VS_ATTRIBUTE_TYPE_SHADING
};

enum vsAttributeCategory
{
    VS_ATTRIBUTE_CATEGORY_STATE,
    VS_ATTRIBUTE_CATEGORY_GROUPING,
    VS_ATTRIBUTE_CATEGORY_XFORM,
    VS_ATTRIBUTE_CATEGORY_CONTAINER,
    VS_ATTRIBUTE_CATEGORY_OTHER
};

class vsAttribute
{
protected:

    char        attributeName[VS_ATTRIBUTE_NAME_MAX_LENGTH];
    
    int         attachedFlag;

VS_INTERNAL:

    virtual int     canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(pfGeoState *state);

public:

                   vsAttribute();
    virtual        ~vsAttribute();

    virtual int    getAttributeType() = 0;
    virtual int    getAttributeCategory() = 0;

    int            isAttached();

    void           setName(char *newName);
    const char     *getName();
};

#endif
