// File vsLODAttribute.h++

#ifndef VS_LOD_ATTRIBUTE_HPP
#define VS_LOD_ATTRIBUTE_HPP

#include <Performer/pf/pfLOD.h>
#include "vsAttribute.h++"
#include "vsNode.h++"

class vsLODAttribute : public vsAttribute
{
private:

    pfLOD       *performerLOD;

VS_INTERNAL:

                vsLODAttribute(pfLOD *lodGroup);

    int         canAttach();    
    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);

public:

                   vsLODAttribute();
                   ~vsLODAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();
    
    void           setRangeEnd(int childNum, double rangeLimit);
    double         getRangeEnd(int childNum);
};

#endif
