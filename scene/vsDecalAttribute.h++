// File vsDecalAttribute.h++

#ifndef VS_DECAL_ATTRIBUTE_HPP
#define VS_DECAL_ATTRIBUTE_HPP

#include <Performer/pf/pfLayer.h>
#include "vsAttribute.h++"
#include "vsNode.h++"

class vsDecalAttribute : public vsAttribute
{
private:

    pfLayer     *performerLayer;

VS_INTERNAL:

                vsDecalAttribute(pfLayer *layerGroup);

    int         canAttach();
    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);

public:

                   vsDecalAttribute();
    virtual        ~vsDecalAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();
};

#endif
