// File vsSwitchAttribute.h++

#ifndef VS_SWITCH_ATTRIBUTE_HPP
#define VS_SWITCH_ATTRIBUTE_HPP

#include <Performer/pf/pfSwitch.h>
#include "vsAttribute.h++"
#include "vsNode.h++"

class vsSwitchAttribute : public vsAttribute
{
private:

    pfSwitch    *performerSwitch;

VS_INTERNAL:

                vsSwitchAttribute(pfSwitch *switchGroup);

    int         canAttach();
    void        attach(vsNode *theNode);
    void        detach(vsNode *theNode);

public:

                   vsSwitchAttribute();
    virtual        ~vsSwitchAttribute();

    virtual int    getAttributeType();
    virtual int    getAttributeCategory();

    void           enableOne(int index);
    void           disableOne(int index);

    void           enableAll();
    void           disableAll();

    int            isEnabled(int index);
};

#endif
