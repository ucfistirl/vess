// File vsLODAttribute.h++

#ifndef VS_LOD_ATTRIBUTE_HPP
#define VS_LOD_ATTRIBUTE_HPP

#include <Performer/pf/pfLOD.h>
#include "vsAttribute.h++"
#include "vsComponent.h++"

class vsLODAttribute : public vsAttribute
{
private:

    pfLOD       *performerLOD;

VS_INTERNAL:

                    vsLODAttribute(pfLOD *lodGroup);

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

public:

                   vsLODAttribute();
                   ~vsLODAttribute();

    virtual int    getAttributeType();
    
    void           setRangeEnd(int childNum, double rangeLimit);
    double         getRangeEnd(int childNum);
};

#endif
