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

#include <osg/MatrixTransform>
#include <osgSim/DOFTransform>
#include <osg/Group>
#include "vsMatrix.h++"
#include "vsComponent.h++"
#include "vsAttribute.h++"

class VS_GRAPHICS_DLL vsTransformAttribute : public vsAttribute
{
private:

    osg::Group              *componentTop;
    osg::MatrixTransform    *transform;
    
    vsMatrix                preMatrix;
    vsMatrix                dynMatrix;
    vsMatrix                postMatrix;
    
    void                    applyTransformations();

VS_INTERNAL:

    virtual bool    canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    vsMatrix        getCombinedTransform();

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
