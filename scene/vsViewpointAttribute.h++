// File vsViewpointAttribute.h++

#ifndef VS_VIEWPOINT_ATTRIBUTE_HPP
#define VS_VIEWPOINT_ATTRIBUTE_HPP

class vsViewpointAttribute;

#include <Performer/pf/pfGroup.h>
#include "vsNode.h++"
#include "vsView.h++"
#include "vsAttribute.h++"

class vsViewpointAttribute : public vsAttribute
{
private:

    vsView      *viewObject;
    vsMatrix    offsetMatrix;
    
    pfGroup     *componentMiddle;

VS_INTERNAL:

    int         canAttach();
    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);
    
    void        update();

public:

                vsViewpointAttribute(vsView *theView);
    virtual     ~vsViewpointAttribute();

    int         getAttributeType();
    int         getAttributeCategory();

    void        setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix    getOffsetMatrix();
};

#endif
