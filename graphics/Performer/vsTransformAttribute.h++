//------------------------------------------------------------------------
//
//    VIRTUAL ENVIRONMENT SOFTWARE SANDBOX (VESS)
//
//    Copyright (c) 2001, University of Central Florida
//
//       See the file LICENSE for license information
//
//    E-mail:  vess@ist.ucf.edu
//    WWW:     http://vess.ist.ucf.edu/
//
//------------------------------------------------------------------------
//
//    VESS Module:  vsTransformAttribute.h++
//
//    Description:  Attribute that specifies a geometric transformation
//                  that should be applied to all of the children of the
//                  component
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_TRANSFORM_ATTRIBUTE_HPP
#define VS_TRANSFORM_ATTRIBUTE_HPP

#include <Performer/pf/pfDCS.h>
#include "vsMatrix.h++"
#include "vsComponent.h++"
#include "vsDatabaseLoader.h++"
#include "vsAttribute.h++"

class vsTransformAttribute : public vsAttribute
{
private:

    pfGroup     *componentTop;
    pfSCS       *preTransform;
    pfDCS       *dynTransform;
    pfSCS       *postTransform;
    
    vsMatrix    preMatrix;
    vsMatrix    dynMatrix;
    vsMatrix    postMatrix;
    
    void        pushBottom(pfGroup *splitGroup);

VS_INTERNAL:

    virtual bool    canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

public:

                          vsTransformAttribute();
    virtual               ~vsTransformAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    void                  setPreTransform(vsMatrix newTransform);
    vsMatrix              getPreTransform();
    void                  setDynamicTransform(vsMatrix newTransform);
    vsMatrix              getDynamicTransform();
    void                  setPostTransform(vsMatrix newTransform);
    vsMatrix              getPostTransform();
};

#endif
