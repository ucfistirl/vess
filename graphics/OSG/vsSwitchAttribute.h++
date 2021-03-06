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

#include "vsAttribute.h++"
#include "vsNode.h++"
#include "vsComponent.h++"
#include <osgSim/MultiSwitch>

class VESS_SYM vsSwitchAttribute : public vsAttribute
{
private:

    osgSim::MultiSwitch     *osgSwitch;
    
    bool                    allEnabled;
    bool                    allDisabled;
    

VS_INTERNAL:

    virtual bool    canAttach();
    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

    virtual void    attachDuplicate(vsNode *theNode);

    void            addMask(vsComponent *parent, vsNode *newChild);
    void            pruneMasks(vsComponent *parent);
    void            setMaskValue(int maskIndex, int childIndex, bool value);

public:

                          vsSwitchAttribute();
    virtual               ~vsSwitchAttribute();

    virtual const char    *getClassName();

    virtual int           getAttributeType();
    virtual int           getAttributeCategory();
    virtual vsAttribute   *clone();

    void                  enableOne(int index);
    void                  disableOne(int index);

    void                  enableAll();
    void                  disableAll();

    bool                  isEnabled(int index);
};

#endif
