// File vsStateAttribute.h++

#ifndef VS_STATE_ATTRIBUTE_HPP
#define VS_STATE_ATTRIBUTE_HPP

#include "vsGlobals.h++"
#include "vsGrowableArray.h++"
#include "vsAttribute.h++"

class vsStateAttribute : public vsAttribute
{
protected:

    vsGrowableArray    attrSaveList;
    int                attrSaveCount;
    
    vsGrowableArray    ownerList;
    int                ownerCount;
    
    void               markOwnersDirty();

VS_INTERNAL:

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

public:

		   vsStateAttribute();
		   ~vsStateAttribute();

    virtual int    getAttributeCategory();
};

#endif
