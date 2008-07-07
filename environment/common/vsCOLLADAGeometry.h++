#ifndef VS_COLLADA_GEOMETRY_HPP
#define VS_COLLADA_GEOMETRY_HPP

#include "atList.h++"
#include "atMap.h++"
#include "atString.h++"
#include "vsObject.h++"
#include "vsCOLLADADataSource.h++"
#include "vsCOLLADASubmesh.h++"
#include "vsComponent.h++"
#include "vsGeometry.h++"

class vsCOLLADAGeometry : public vsObject
{
protected:

    atString  geometryID;
    atList    *submeshList;
    atMap     *dataSources;

    vsCOLLADADataSource    *getDataSource(atString id);
    int                    getGeometryDataList(atString semantic, int set);


    void           processSource(atXMLDocument *doc,
                                 atXMLDocumentNodePtr current);

    void           processInput(atXMLDocument *doc,
                                atXMLDocumentNodePtr current,
                                atList *inputList);

    void           processMesh(atXMLDocument *doc,
                               atXMLDocumentNodePtr current, atString id,
                               atString name);

public:

                            vsCOLLADAGeometry(atString id, atXMLDocument *doc,
                                              atXMLDocumentNodePtr current);
                            ~vsCOLLADAGeometry();

    virtual const char *    getClassName();

    atString                getID();

    u_long                  getNumSubmeshes();
    vsCOLLADASubmesh        *getFirstSubmesh();
    vsCOLLADASubmesh        *getNextSubmesh();

    vsComponent             *instance();
};

#endif
