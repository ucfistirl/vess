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
//    VESS Module:  vsClipAttribute.h++
//
//    Description:  Applies one or more user-defined clipping planes to
//                  the subgraph of the scene where it is attached
//
//    Author(s):    Michael Prestia, Jason Daly
//
//------------------------------------------------------------------------
#ifndef VS_CLIP_ATTRIBTUE_HPP
#define VS_CLIP_ATTRIBUTE_HPP

#include <stdio.h>

#include <osg/ClipPlane>
#include "vsStateAttribute.h++"

#define VS_CLIPATTR_MAX_PLANES  6

class VS_GRAPHICS_DLL vsClipAttribute : public vsStateAttribute
{
private:

    osg::ClipPlane     *planeArray[VS_CLIPATTR_MAX_PLANES];
    int                numPlanes;

    vsGrowableArray    attachedNodes;

    virtual void       setOSGAttrModes(vsNode *node);

VS_INTERNAL:

    virtual void    attach(vsNode *node);
    virtual void    detach(vsNode *node);

    virtual void    attachDuplicate(vsNode *theNode);

    virtual bool    isEquivalent(vsAttribute *attribute);

public:
                           vsClipAttribute();
                           ~vsClipAttribute();

    virtual const char     *getClassName();

    virtual int            getAttributeType();
    virtual int            getAttributeCategory();
    virtual vsAttribute    *clone();

    void                   setClipPlane(int planeIndex, double a, double b,
                                        double c, double d);
    void                   setClipPlane(int planeIndex,
                                        atVector pointOnPlane,
                                        atVector normal);
    void                   removeClipPlane(int planeIndex);

    bool                   isClipPlaneActive(int planeIndex);
    void                   getClipPlaneCoeffs(int planeIndex,
                                              double *a, double *b,
                                              double *c, double *d);
    atVector               getClipPlaneNormal(int planeIndex);
};

#endif
