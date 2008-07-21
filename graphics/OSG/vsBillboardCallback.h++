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
//    VESS Module:  vsBillboardCallback.h++
//
//    Description:  OSG-specific class that implements a callback which
//                  is called when an OSG cull traversal reaches a
//                  vsComponent with a vsBillboardAttribute attached
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_BILLBOARD_CALLBACK_HPP
#define VS_BILLBOARD_CALLBACK_HPP

#include <osg/NodeCallback>
#include "vsBillboardAttribute.h++"

class VESS_SYM vsBillboardCallback : public osg::NodeCallback
{
private:

    vsBillboardAttribute    *billboardAttr;

public:

                    vsBillboardCallback(vsBillboardAttribute *billAttr);
    virtual         ~vsBillboardCallback();

    virtual void    operator()(osg::Node* node, osg::NodeVisitor* nv);
};

#endif
