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
//    VESS Module:  vsSwitchAttribute.h++
//
//    Description:  Attribute that specifies which of the children of this
//                  component are to be drawn
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_SWITCH_ATTRIBUTE_HPP
#define VS_SWITCH_ATTRIBUTE_HPP

#include <osg/Switch>
#include "vsAttribute.h++"
#include "vsNode.h++"

class vsSwitchAttribute : public vsAttribute
{
private:

    osg::Switch     *osgSwitch;

VS_INTERNAL:

    virtual int     canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

public:

                          vsSwitchAttribute();
    virtual               ~vsSwitchAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();

    void                  enableOne(int index);
    void                  disableOne(int index);

    void                  enableAll();
    void                  disableAll();

    int                   isEnabled(int index);
};

#endif
