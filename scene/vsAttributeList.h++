// File vsAttributeList.h++

#ifndef VS_ATTRIBUTE_LIST_HPP
#define VS_ATTRIBUTE_LIST_HPP

class vsAttributeList;

#include <Performer/pr/pfMemory.h>
#include <Performer/pr/pfState.h>
#include <Performer/pr/pfGeoState.h>

#include "vsGlobals.h++"
#include "vsAttribute.h++"

#define VS_ATTRIBUTE_LIST_SIZE    50

class vsAttributeList
{
protected:

    int            attributeCount;
    vsAttribute    *attributeList[VS_ATTRIBUTE_LIST_SIZE];

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
    vsAttribute     *getNamedAttribute(char *attribName);
};

#endif
