
#ifndef VS_COLLADA_INPUT_ENTRY_HPP
#define VS_COLLADA_INPUT_ENTRY_HPP

#include "vsCOLLADADataSource.h++"

class VESS_SYM vsCOLLADAInputEntry : public vsObject
{
protected:

    vsCOLLADADataSource    *dataSource;
    int                    geometryDataList;
    int                    inputOffset;

public:

                           vsCOLLADAInputEntry(vsCOLLADADataSource *src,
                                               int list,
                                               int offset);
    virtual                ~vsCOLLADAInputEntry();

    virtual const char     *getClassName();

    vsCOLLADAInputEntry    *clone();

    vsCOLLADADataSource    *getSource();
    int                    getDataList();
    int                    getOffset();
};

#endif

