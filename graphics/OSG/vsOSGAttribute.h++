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
//    VESS Module:  vsOSGAttribute.c++
//
//    Description:  vsObject wrapper for osg::StateAttribute objects (and
//                  descendants)
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsObject.h++"
#include "osg/StateAttribute"

class vsOSGAttribute : public vsObject
{
protected:

    osg::StateAttribute   *osgAttribute;

public:

                          vsOSGAttribute(osg::StateAttribute *theAttribute);
    virtual               ~vsOSGAttribute();

    virtual const char    *getClassName();

    osg::StateAttribute   *getAttribute();

    bool                  equals(atItem *otherItem);
    int                   compare(atItem *otherItem);
};

