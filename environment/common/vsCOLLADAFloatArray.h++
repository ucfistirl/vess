#ifndef VS_COLLADA_FLOAT_ARRAY
#define VS_COLLADA_FLOAT_ARRAY

#include "vsCOLLADADataArray.h++"
#include "atXMLDocument.h++"

class VESS_SYM vsCOLLADAFloatArray : public vsCOLLADADataArray
{
protected:

    double    *dataArray;

public:

                          vsCOLLADAFloatArray(atXMLDocument *doc,
                                              atXMLDocumentNodePtr current);
    virtual               ~vsCOLLADAFloatArray();

    virtual const char    *getClassName();

    virtual DataType      getDataType();

    virtual double        getData(int index);
};

#endif
