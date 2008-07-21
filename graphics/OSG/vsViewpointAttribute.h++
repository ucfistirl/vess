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

#include "vsAttribute.h++"
#include "vsComponent.h++"
#include "vsView.h++"
#include "vsObjectMap.h++"
#include <stdio.h>

class VESS_SYM vsViewpointAttribute : public vsAttribute
{
private:

    static vsObjectMap    *viewObjectMap;

    vsView                *viewObject;
    atMatrix              offsetMatrix;
    
    vsComponent           *parentComponent;

VS_INTERNAL:

    static vsObjectMap    *getMap();
    static void           deleteMap();

    virtual bool          canAttach();
    virtual void          attach(vsNode *theNode);
    virtual void          detach(vsNode *theNode);

    virtual void          attachDuplicate(vsNode *theNode);

    void                  update();

public:

                           vsViewpointAttribute();
                           vsViewpointAttribute(vsView *theView);
    virtual                ~vsViewpointAttribute();

    virtual const char     *getClassName();

    int                    getAttributeType();
    int                    getAttributeCategory();
    virtual vsAttribute    *clone();
    
    void                   setView(vsView *theView);
    vsView                 *getView();

    void                   setOffsetMatrix(atMatrix newMatrix);
    atMatrix               getOffsetMatrix();
};

#endif
