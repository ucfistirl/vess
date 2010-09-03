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
//    VESS Module:  vsOSGStateSet.c++
//
//    Description:  vsObject wrapper for osg::StateSet objects
//
//    Author(s):    Jason Daly
//
//------------------------------------------------------------------------

#include "vsObject.h++"
#include "osg/StateSet"

class vsOSGStateSet : public vsObject
{
protected:

    osg::StateSet   *osgStateSet;

public:

                          vsOSGStateSet(osg::StateSet *theStateSet);
    virtual               ~vsOSGStateSet();

    virtual const char    *getClassName();

    osg::StateSet         *getStateSet();

    bool                  equals(atItem *otherItem);
    int                   compare(atItem *otherItem);
};

