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
//    VESS Module:  vsAttribute.h++
//
//    Description:  Abstract base class for all objects that can be
//                  attached to various points on the scene graph.
//                  Attributes are attached to nodes in order to specify
//                  some alteration to the geometry at and below that
//                  node.
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_ATTRIBUTE_HPP
#define VS_ATTRIBUTE_HPP

class vsNode;

#include <osg/StateSet>
#include "vsGlobals.h++"
#include "vsObject.h++"
#include "vsObjectMap.h++"
#include "vsNode.h++"


#define VS_ATTRIBUTE_NAME_MAX_LENGTH 80

enum VS_GRAPHICS_DLL vsAttributeType
{
    VS_ATTRIBUTE_TYPE_TRANSFORM,
    VS_ATTRIBUTE_TYPE_SWITCH,
    VS_ATTRIBUTE_TYPE_SEQUENCE,
    VS_ATTRIBUTE_TYPE_LOD,
    VS_ATTRIBUTE_TYPE_LIGHT,
    VS_ATTRIBUTE_TYPE_FOG,
    VS_ATTRIBUTE_TYPE_MATERIAL,
    VS_ATTRIBUTE_TYPE_TEXTURE,
    VS_ATTRIBUTE_TYPE_TRANSPARENCY,
    VS_ATTRIBUTE_TYPE_BILLBOARD,
    VS_ATTRIBUTE_TYPE_VIEWPOINT,
    VS_ATTRIBUTE_TYPE_BACKFACE,
    VS_ATTRIBUTE_TYPE_DECAL,
    VS_ATTRIBUTE_TYPE_SHADING,
    VS_ATTRIBUTE_TYPE_SOUND_SOURCE,
    VS_ATTRIBUTE_TYPE_SOUND_LISTENER,
    VS_ATTRIBUTE_TYPE_WIREFRAME,
    VS_ATTRIBUTE_TYPE_SCENT_SOURCE,
    VS_ATTRIBUTE_TYPE_SCENT_DETECTOR,
    VS_ATTRIBUTE_TYPE_CG_SHADER
};

enum VS_GRAPHICS_DLL vsAttributeCategory
{
    VS_ATTRIBUTE_CATEGORY_STATE,
    VS_ATTRIBUTE_CATEGORY_GROUPING,
    VS_ATTRIBUTE_CATEGORY_XFORM,
    VS_ATTRIBUTE_CATEGORY_CONTAINER,
    VS_ATTRIBUTE_CATEGORY_OTHER
};

class VS_GRAPHICS_DLL vsAttribute : public vsObject
{
protected:

    char        attributeName[VS_ATTRIBUTE_NAME_MAX_LENGTH];
    
    int         attachedCount;

VS_INTERNAL:

    virtual bool    canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);
    
    virtual void    attachDuplicate(vsNode *theNode);

    virtual void    saveCurrent();
    virtual void    apply();
    virtual void    restoreSaved();
    virtual void    setState(osg::StateSet *stateSet);

public:

                   vsAttribute();
    virtual        ~vsAttribute();

    virtual int    getAttributeType() = 0;
    virtual int    getAttributeCategory() = 0;

    bool           isAttached();

    void           setName(char *newName);
    const char     *getName();
};

#endif
