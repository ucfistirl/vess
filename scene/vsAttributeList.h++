// File vsAttributeList.h++

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

    static void     *operator new(size_t objSize);
    static void     operator delete(void *deadObj);

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
