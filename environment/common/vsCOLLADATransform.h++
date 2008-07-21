#ifndef VS_COLLADA_TRANSFORM
#define VS_COLLADA_TRANSFORM


#include "vsObject.h++"
#include "atMatrix.h++"
#include "atQuat.h++"
#include "atVector.h++"
#include "atXMLDocument.h++"
#include "atStringTokenizer.h++"


enum vsCOLLADATransformType
{
    VS_COLLADA_XFORM_LOOKAT,
    VS_COLLADA_XFORM_MATRIX,
    VS_COLLADA_XFORM_SCALE,
    VS_COLLADA_XFORM_SKEW,
    VS_COLLADA_XFORM_ROTATE,
    VS_COLLADA_XFORM_TRANSLATE,
    VS_COLLADA_XFORM_UNKNOWN
};


class VESS_SYM vsCOLLADATransform : public vsObject
{
protected:

    vsCOLLADATransformType    transformType;
    atString                  scopedID;
    double                    values[16];

    atMatrix                  resultMatrix;

    double                    getFloatToken(atStringTokenizer *tokens);

    void                      updateMatrix();

public:

                              vsCOLLADATransform(atXMLDocument *doc,
                                                 atXMLDocumentNodePtr current);
    virtual                   ~vsCOLLADATransform();

    virtual const char        *getClassName();

    atString                  getSID();
    vsCOLLADATransformType    getType();

    atMatrix                  getMatrix();
    atVector                  getPosition();
    atQuat                    getOrientation();
};

#endif

