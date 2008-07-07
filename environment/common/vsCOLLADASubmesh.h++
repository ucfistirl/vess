#ifndef VS_COLLADA_SUBMESH_HPP
#define VS_COLLADA_SUBMESH_HPP


#include "atItem.h++"
#include "atList.h++"
#include "atMap.h++"
#include "atString.h++"
#include "atStringTokenizer.h++"
#include "atXMLDocument.h++"
#include "vsGeometry.h++"
#include "vsCOLLADAInputEntry.h++"


class vsCOLLADASubmesh : public atItem
{
protected:

    vsGeometryBase     *geometry;
    atString           materialID;
    atMap              *dataSources;
    atList             *inputList;
    int                inputStride;
    int                *indexList;
    int                indexListSize;

    vsCOLLADADataSource    *getDataSource(atString sourceID);
    int                    getGeometryDataList(atString semantic, int set);
    int                    getIntToken(char *tokenString, int *idx);

    void           computeLengthsExplicit(atXMLDocument *doc,
                                          atXMLDocumentNodePtr current);
    void           computeLengthsImplicit(atXMLDocument *doc,
                                          atXMLDocumentNodePtr current);
    void           processPrimitiveIndices(atXMLDocument *doc,
                                           atXMLDocumentNodePtr current,
                                           int primitivesPerPList);
    void           processInput(atXMLDocument *doc,
                                atXMLDocumentNodePtr current);

public:

                         vsCOLLADASubmesh(atXMLDocument *doc,
                                          atXMLDocumentNodePtr current,
                                          atMap *sources,
                                          atList *meshVertexInputs);
                         ~vsCOLLADASubmesh();

   vsGeometryBase         *getGeometry();
   void                   setGeometry(vsGeometryBase *newGeom);
   atString               getMaterialID();

   vsCOLLADAInputEntry    *getFirstInputEntry();
   vsCOLLADAInputEntry    *getNextInputEntry();
   vsCOLLADAInputEntry    *getInputEntryByID(atString id);

   int                    getIndexListSize();
   void                   setIndexListSize(int newSize);
   int                    getIndex(int indexIndex);
   void                   setIndex(int indexIndex, int indexValue);
};

#endif
