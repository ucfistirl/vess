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
//    VESS Module:  vsAttributeList.h++
//
//    Description:  Class that stores and manages a list of attribute
//                  objects. Although this class can be instantiated
//                  directly, it is more useful as the base class for
//                  vsNode objects.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_ATTRIBUTE_LIST_HPP
#define VS_ATTRIBUTE_LIST_HPP

#include "vsGrowableArray.h++"
#include "vsAttribute.h++"

class vsAttributeList
{
protected:

    int                attributeCount;
    vsGrowableArray    attributeList;

public:

                    vsAttributeList();
    virtual         ~vsAttributeList();

    virtual void    addAttribute(vsAttribute *newAttribute);
    virtual void    removeAttribute(vsAttribute *targetAttribute);

    int             getAttributeCount();
    vsAttribute     *getAttribute(int index);
    vsAttribute     *getTypedAttribute(int attribType, int index);
    vsAttribute     *getCategoryAttribute(int attribCategory, int index);
    vsAttribute     *getNamedAttribute(char *attribName);
};

#endif
