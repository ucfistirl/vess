//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsStateAttribute.h++
//
//    Description:  Abstract base class for all state-category attributes
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_STATE_ATTRIBUTE_HPP
#define VS_STATE_ATTRIBUTE_HPP

#include "vsGlobals.h++"
#include "vsGrowableArray.h++"
#include "vsAttribute.h++"

class VS_GRAPHICS_DLL vsStateAttribute : public vsAttribute
{
protected:

    vsGrowableArray    attrSaveList;
    int                attrSaveCount;
    
    vsGrowableArray    ownerList;
    int                ownerCount;
    
    bool               overrideFlag;
    
    void               markOwnersDirty();

VS_INTERNAL:

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual bool    isEquivalent(vsAttribute *attribute) = 0;

public:

                   vsStateAttribute();
    virtual        ~vsStateAttribute();

    virtual int    getAttributeCategory();
    
    void           setOverride(bool override);
    bool           getOverride();
};

#endif
