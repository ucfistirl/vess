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
//    VESS Module:  vsViewpointAttribute.h++
//
//    Description:  Attribute that binds a vsView object to a certain
//                  node in the scene. The vsView is automatically updated
//                  with the transform affecting the node every frame.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_VIEWPOINT_ATTRIBUTE_HPP
#define VS_VIEWPOINT_ATTRIBUTE_HPP

class vsViewpointAttribute;

#include "vsComponent.h++"
#include "vsView.h++"
#include "vsAttribute.h++"

class vsViewpointAttribute : public vsAttribute
{
private:

    vsView         *viewObject;
    vsMatrix       offsetMatrix;
    
    vsComponent    *parentComponent;

VS_INTERNAL:

    virtual int     canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);
    
    virtual void    attachDuplicate(vsNode *theNode);

    void            update();

public:

                vsViewpointAttribute(vsView *theView);
    virtual     ~vsViewpointAttribute();

    int         getAttributeType();
    int         getAttributeCategory();

    void        setOffsetMatrix(vsMatrix newMatrix);
    vsMatrix    getOffsetMatrix();
};

#endif
