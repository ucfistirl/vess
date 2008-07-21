#ifndef VS_COLLADA_NODE
#define VS_COLLADA_NODE

#include "vsComponent.h++"
#include "atList.h++"
#include "atString.h++"
#include "vsCOLLADATransform.h++"

enum vsCOLLADANodeType
{
    VS_CNODE_TYPE_NODE,
    VS_CNODE_TYPE_JOINT
};

class VESS_SYM vsCOLLADANode : public vsComponent
{
protected:

    atString             nodeID;
    atString             nodeSID;
    vsCOLLADANodeType    colladaNodeType;
    atList *             transformList;

public:

                          vsCOLLADANode(atString id, atString name,
                                        atString sid, vsCOLLADANodeType type);
    virtual               ~vsCOLLADANode();

    virtual const char    *getClassName();

    atString              getID();
    atString              getSID();
    vsCOLLADANodeType     getCOLLADANodeType();

    vsCOLLADANode         *findNodeByID(atString id);
    vsCOLLADANode         *findNodeBySID(atString sid);

    void                  addTransform(vsCOLLADATransform * xform);
    vsCOLLADATransform    *getTransform(atString sid);
    vsCOLLADATransform    *getFirstTransform();
    vsCOLLADATransform    *getNextTransform();

    atMatrix              getCombinedTransform();
    atVector              getCombinedPosition();
    atQuat                getCombinedOrientation();
};

#endif
