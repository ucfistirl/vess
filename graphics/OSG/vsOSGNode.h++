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
//    VESS Module:  vsOSGNode.c++
//
//    Description:  vsObject wrapper for osg::Node objects (and
//                  descendants)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsObject.h++"
#include "osg/Node"

class vsOSGNode : public vsObject
{
protected:

    osg::Node   *osgNode;

public:

                         vsOSGNode(osg::Node *theNode);
    virtual              ~vsOSGNode();

    virtual const char   *getClassName();

    osg::Node            *getNode();

    bool                 equals(atItem *otherItem);
    int                  compare(atItem *otherItem);
};

