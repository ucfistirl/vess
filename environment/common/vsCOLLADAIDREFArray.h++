#ifndef VS_COLLADA_IDREF_ARRAY
#define VS_COLLADA_IDREF_ARRAY

#include "vsCOLLADADataArray.h++"
#include "atArray.h++"
#include "atString.h++"
#include "atXMLDocument.h++"

class vsCOLLADAIDREFArray : public vsCOLLADADataArray
{
protected:

    atArray    *dataArray;

public:

                          vsCOLLADAIDREFArray(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current);
    virtual               ~vsCOLLADAIDREFArray();

    virtual const char    *getClassName();

    virtual DataType      getDataType();

    virtual atString      getData(int index);
};

#endif
