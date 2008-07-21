#ifndef VS_COLLADA_BOOL_ARRAY
#define VS_COLLADA_BOOL_ARRAY

#include "vsCOLLADADataArray.h++"
#include "atXMLDocument.h++"

class VESS_SYM vsCOLLADABoolArray : public vsCOLLADADataArray
{
protected:

    bool    *dataArray;

public:

                          vsCOLLADABoolArray(atXMLDocument *doc,
                                             atXMLDocumentNodePtr current);
    virtual               ~vsCOLLADABoolArray();

    virtual const char    *getClassName();

    virtual DataType      getDataType();

    virtual bool          getData(int index);
};

#endif
