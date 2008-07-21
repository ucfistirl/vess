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
#include "atMatrix.h++"
#include "vsComponent.h++"
#include "vsAttribute.h++"

class VESS_SYM vsTransformAttribute : public vsAttribute
{
private:

    osg::Group              *componentTop;
    osg::MatrixTransform    *transform;
    
    atMatrix                preMatrix;
    atMatrix                dynMatrix;
    atMatrix                postMatrix;
    
    void                    applyTransformations();

VS_INTERNAL:

    virtual bool    canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    atMatrix        getCombinedTransform();

public:

                           vsTransformAttribute();
    virtual                ~vsTransformAttribute();

    virtual const char     *getClassName();

    virtual int            getAttributeType();
    virtual int            getAttributeCategory();
    virtual vsAttribute    *clone();

    void                   setPreTransform(atMatrix newTransform);
    atMatrix               getPreTransform();
    void                   setDynamicTransform(atMatrix newTransform);
    atMatrix               getDynamicTransform();
    void                   setPostTransform(atMatrix newTransform);
    atMatrix               getPostTransform();
};

#endif
