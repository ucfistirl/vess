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
//    VESS Module:  vsDecalAttribute.h++
//
//    Description:  OSG-specific class that implements a callback which
//                  is called when an OSG cull traversal reaches a
//                  vsComponent with a vsDecalAttribute attached
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_DECAL_CALLBACK_HPP
#define VS_DECAL_CALLBACK_HPP

#include "vsDecalAttribute.h++"
#include <osg/NodeCallback>

class VESS_SYM vsDecalCallback : public osg::NodeCallback
{
private:

    vsDecalAttribute    *decalAttr;
    
    vsGrowableArray     stateSetArray;
    int                 stateSetArraySize;
    
    void                checkSize(int newSize);

public:

                    vsDecalCallback(vsDecalAttribute *decalAttrib);
    virtual         ~vsDecalCallback();

    virtual void    operator()(osg::Node* node, osg::NodeVisitor* nv);
};

#endif
