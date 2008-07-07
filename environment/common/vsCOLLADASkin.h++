#ifndef VS_COLLADA_SKIN_HPP
#define VS_COLLADA_SKIN_HPP


#include "atXMLDocument.h++"
#include "atMatrix.h++"
#include "vsCOLLADAController.h++"
#include "vsCOLLADADataSource.h++"


class vsCOLLADASkin : public vsCOLLADAController
{
protected:

    atMatrix               bindShapeMatrix;

    vsCOLLADADataSource    *jointNames;
    vsCOLLADADataSource    *inverseBindMatrices;
    vsCOLLADADataSource    *vertexWeights;

    atMap                  *jointMatrixMap;

    void                   convertGeometry();

    vsCOLLADADataSource    *getDataSource(atString id);

    int                    getIntToken(atStringTokenizer *tokens);

    atMatrix               parseMatrix(atXMLDocument *doc,
                                       atXMLDocumentNodePtr current);
    void                   processJoints(atXMLDocument *doc,
                                         atXMLDocumentNodePtr current);
    void                   processVertexWeights(atXMLDocument *doc,
                                                atXMLDocumentNodePtr current);

    void                   applyBindShape(vsNode *node, atMatrix bindShape,
                                          atMatrix bindShapeIT);

VS_INTERNAL:

    vsCOLLADADataSource     *getJointNames();
    vsCOLLADADataSource     *getInverseBindMatrices();

public:

                            vsCOLLADASkin(atXMLDocument *doc,
                                          atXMLDocumentNodePtr current,
                                          vsCOLLADAGeometry *geom);
                            ~vsCOLLADASkin();

    virtual const char      *getClassName();

    virtual vsComponent     *instance();
};


#endif
