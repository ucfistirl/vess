#ifndef VS_COLLADA_DATA_SOURCE_HPP
#define VS_COLLADA_DATA_SOURCE_HPP

#include "vsObject.h++"
#include "atString.h++"
#include "atXMLDocument.h++"
#include "vsCOLLADADataArray.h++"
#include "atVector.h++"
#include "atMatrix.h++"

#define VS_CDS_MAX_PARAMS  16

enum vsCOLLADADataSourceFormat
{
    STRING,
    BOOL,
    INT,
    FLOAT,
    VECTOR,
    MATRIX
};

class VESS_SYM vsCOLLADADataSource : public vsObject
{
protected:

    vsCOLLADADataArray           *dataArray;
    atString                     dataArrayID;

    atString                     dataSourceID;
    atString                     dataArrayIDRef;
    int                          dataCount;
    int                          dataOffset;
    int                          dataStride;
    vsCOLLADADataSourceFormat    dataFormat;
    int                          dataSize;
    int                          paramCount;
    atString                     paramName[VS_CDS_MAX_PARAMS];
    vsCOLLADADataSourceFormat    paramFormat[VS_CDS_MAX_PARAMS];
    int                          paramSize[VS_CDS_MAX_PARAMS];

    void                  flushData();
    void                  processSource(atXMLDocument *doc,
                                        atXMLDocumentNodePtr current);
    void                  processTechniqueCommon(atXMLDocument *doc,
                                                 atXMLDocumentNodePtr current);

public:

                                 vsCOLLADADataSource(
                                              atXMLDocument *doc,
                                              atXMLDocumentNodePtr current);
    virtual                      ~vsCOLLADADataSource();

    virtual const char           *getClassName();

    atString                     getID();

    vsCOLLADADataArray           *getDataArray();

    int                          getParamCount();
    atString                     getParamName(int index);
    int                          getDataCount();
    vsCOLLADADataSourceFormat    getDataFormat();

    atString                     getString(int index);
    bool                         getBool(int index);
    int                          getInt(int index);
    double                       getFloat(int index);
    atVector                     getVector(int index);
    atMatrix                     getMatrix(int index);
};


#endif
