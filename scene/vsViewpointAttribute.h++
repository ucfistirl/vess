// File vsViewpointAttribute.h++

#ifndef VS_VIEWPOINT_ATTRIBUTE_HPP
#define VS_VIEWPOINT_ATTRIBUTE_HPP

class vsViewpointAttribute;

#include <Performer/pf/pfSCS.h>
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

    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);
    
    void        update();

public:

                vsViewpointAttribute(vsView *theView);
                ~vsViewpointAttribute();

    int         getAttributeType();

    void        setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix    getOffsetMatrix();
};

#endif
