#ifndef VS_COLLADA_NAME_ARRAY
#define VS_COLLADA_NAME_ARRAY

#include "vsCOLLADADataArray.h++"
#include "atArray.h++"
#include "atString.h++"
#include "atXMLDocument.h++"

class VESS_SYM vsCOLLADANameArray : public vsCOLLADADataArray
{
protected:

    atArray    *dataArray;

public:

                          vsCOLLADANameArray(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current);
    virtual               ~vsCOLLADANameArray();

    virtual const char    *getClassName();

    virtual DataType      getDataType();

    virtual atString      getData(int index);
};

#endif
