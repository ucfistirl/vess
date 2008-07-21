#ifndef VS_COLLADA_NODE_REF_HPP
#define VS_COLLADA_NODE_REF_HPP

#include "vsCOLLADANode.h++"

class VESS_SYM vsCOLLADANodeRef : public vsObject
{
protected:

    vsCOLLADANode        *colladaNode;

public:

                          vsCOLLADANodeRef(vsCOLLADANode *node);
    virtual               ~vsCOLLADANodeRef();

    virtual const char    *getClassName();

    vsCOLLADANode         *getNode();
};

#endif
