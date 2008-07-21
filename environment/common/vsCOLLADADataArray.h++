#ifndef VS_COLLADA_DATA_ARRAY
#define VS_COLLADA_DATA_ARRAY

#include "vsObject.h++"
#include "atString.h++"

class VESS_SYM vsCOLLADADataArray : public vsObject
{
public:

    enum DataType
    {
        IDREF,
        NAME,
        BOOL,
        FLOAT,
        INT
    };

protected:

    atString    dataID;
    DataType    dataType;
    int         dataCount;

public:

                          vsCOLLADADataArray();
    virtual               ~vsCOLLADADataArray();

    virtual const char    *getClassName();

    virtual atString      getID();
    virtual DataType      getDataType() = 0;
    virtual int           getDataCount();
};

#endif
