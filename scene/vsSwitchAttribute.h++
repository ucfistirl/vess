// File vsSwitchAttribute.h++

#ifndef VS_SWITCH_ATTRIBUTE_HPP
#define VS_SWITCH_ATTRIBUTE_HPP

#include <Performer/pf/pfSwitch.h>
#include "vsAttribute.h++"
#include "vsComponent.h++"

class vsSwitchAttribute : public vsAttribute
{
private:

    pfSwitch    *performerSwitch;

VS_INTERNAL:

                    vsSwitchAttribute(pfSwitch *switchGroup);

    virtual void    attach(vsNode *theNode);
    virtual void    detach(vsNode *theNode);

public:

                   vsSwitchAttribute();
                   ~vsSwitchAttribute();

    virtual int    getAttributeType();

    void           enableOne(int index);
    void           disableOne(int index);

    void           enableAll();
    void           disableAll();

    int            isEnabled(int index);
};

#endif
