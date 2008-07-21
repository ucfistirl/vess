#ifndef VS_COLLADA_CHANNEL_HPP
#define VS_COLLADA_CHANNEL_HPP

#include "vsObject.h++"
#include "vsCOLLADAKeyframe.h++"
#include "vsCOLLADASampler.h++"
#include "vsPathMotion.h++"
#include "atString.h++"

class VESS_SYM vsCOLLADAChannel : public vsObject
{
protected:

    bool                             validFlag;

    vsCOLLADASampler                 *sampler;

    atString                         targetNodeID;
    atString                         targetXformSID;

    vsCOLLADASampler                 *getSampler(atMap *samplers, atString id);

public:

                                     vsCOLLADAChannel(atXMLDocument *doc,
                                                 atXMLDocumentNodePtr current,
                                                 atMap *samplers);
                                     ~vsCOLLADAChannel();

    virtual const char               *getClassName();

    bool                             isValid();

    int                              getNumKeyframes();
    vsCOLLADAKeyframe                *getFirstKeyframe();
    vsCOLLADAKeyframe                *getNextKeyframe();
    vsCOLLADAKeyframe                *getKeyframe(int index);

    void                             addKeyframe(double time, int count,
                                                 double values);

    vsPathPosInterpolationMode       getPositionInterpMode();
    vsPathOrientInterpolationMode    getOrientationInterpMode();

    atString                         getTargetNodeID();
    atString                         getTargetXformSID();
};

#endif

