#ifndef VS_COLLADA_INT_ARRAY
#define VS_COLLADA_INT_ARRAY

#include "vsCOLLADADataArray.h++"
#include "atXMLDocument.h++"

class VESS_SYM vsCOLLADAIntArray : public vsCOLLADADataArray
{
protected:

    int    *dataArray;

    int    getIntToken(char *tokenString, int *idx);

public:

                          vsCOLLADAIntArray(atXMLDocument *doc,
                                            atXMLDocumentNodePtr current);
    virtual               ~vsCOLLADAIntArray();

    virtual const char    *getClassName();

    virtual DataType      getDataType();

    virtual int           getData(int index);
};

#endif
