// File vsTransformAttribute.h++

#ifndef VS_TRANSFORM_ATTRIBUTE_HPP
#define VS_TRANSFORM_ATTRIBUTE_HPP

#include <Performer/pf/pfDCS.h>
#include "vsMatrix.h++"
#include "vsComponent.h++"
#include "vsDatabaseLoader.h++"
#include "vsAttribute.h++"

class vsTransformAttribute : public vsAttribute
{
private:

    pfGroup     *componentTop;
    pfSCS       *preTransform;
    pfDCS       *dynTransform;
    pfSCS       *postTransform;
    
    vsMatrix    preMatrix;
    vsMatrix    dynMatrix;
    vsMatrix    postMatrix;
    
    void        pushBottom(pfGroup *splitGroup);

VS_INTERNAL:

                vsTransformAttribute(pfSCS *transformGroup,
                                     vsComponent *targetComponent,
                                     vsDatabaseLoader *nameDirectory);

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

public:

                   vsTransformAttribute();
    virtual        ~vsTransformAttribute();

    virtual int    getAttributeType();

    void           setPreTransform(vsMatrix newTransform);
    vsMatrix       getPreTransform();
    void           setDynamicTransform(vsMatrix newTransform);
    vsMatrix       getDynamicTransform();
    void           setPostTransform(vsMatrix newTransform);
    vsMatrix       getPostTransform();
};

#endif
