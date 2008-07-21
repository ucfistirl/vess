
#ifndef VS_COLLADA_CHANNEL_GROUP_HPP
#define VS_COLLADA_CHANNEL_GROUP_HPP

#include "vsObject.h++"
#include "vsCOLLADAChannel.h++"
#include "vsCOLLADANode.h++"
#include "vsPathMotion.h++"
#include "atList.h++"
#include "atString.h++"


class VESS_SYM vsCOLLADAChannelGroup : public vsObject
{
protected:

    atList           *channels;
    vsCOLLADANode    *targetNode;

public:

                          vsCOLLADAChannelGroup(vsCOLLADANode *target);
                          ~vsCOLLADAChannelGroup();

    virtual const char    *getClassName();

    atString              getTargetNodeID();

    void                  addChannel(vsCOLLADAChannel *channel);

    vsPathMotion          *instance();
};

#endif 

