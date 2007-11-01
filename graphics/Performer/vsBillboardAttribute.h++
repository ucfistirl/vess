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
//    VESS Module:  vsBillboardAttribute.h++
//
//    Description:  Attribute that specifies that the geometry below the
//                  component be rotated to face the viewer at all times
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_BILLBOARD_ATTRIBUTE_HPP
#define VS_BILLBOARD_ATTRIBUTE_HPP

#include "vsGlobals.h++"
#include <Performer/pf/pfBillboard.h>
#include <Performer/pf/pfDCS.h>
#include "atMatrix.h++"
#include "vsAttribute.h++"

enum vsBillboardRotationMode
{
    VS_BILLBOARD_ROT_AXIS,
    VS_BILLBOARD_ROT_POINT_EYE,
    VS_BILLBOARD_ROT_POINT_WORLD
};

class VS_GRAPHICS_DLL vsBillboardAttribute : public vsAttribute
{
private:

    atVector    centerPoint;
    atVector    frontDirection;
    atVector    upAxis;
    
    int         billboardMode;
    
    atMatrix    preTranslate;
    atMatrix    postTranslate;
    pfDCS       *billboardTransform;

VS_INTERNAL:

    virtual bool    canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);
    
    virtual void    attachDuplicate(vsNode *theNode);

    static int      travCallback(pfTraverser *_trav, void *_userData);
    void            adjustTransform(atMatrix viewMatrix, atMatrix currentXform);
    
public:

                          vsBillboardAttribute();
    virtual               ~vsBillboardAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    void                  setMode(int mode);
    int                   getMode();

    void                  setCenterPoint(atVector newCenter);
    atVector              getCenterPoint();

    void                  setFrontDirection(atVector newFront);
    atVector              getFrontDirection();

    void                  setAxis(atVector newAxis);
    atVector              getAxis();
};

#endif
