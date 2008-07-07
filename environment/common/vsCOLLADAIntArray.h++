#ifndef VS_COLLADA_INT_ARRAY
#define VS_COLLADA_INT_ARRAY

#include "vsCOLLADADataArray.h++"
#include "atXMLDocument.h++"

class vsCOLLADAIntArray : public vsCOLLADADataArray
{
protected:

    int    *dataArray;

public:

                          vsCOLLADAIntArray(atXMLDocument *doc,
                                            atXMLDocumentNodePtr current);
    virtual               ~vsCOLLADAIntArray();

    virtual const char    *getClassName();

    virtual DataType      getDataType();

    virtual int           getData(int index);
};

#endif
