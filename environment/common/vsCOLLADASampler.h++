#ifndef VS_COLLADA_SAMPLER_HPP
#define VS_COLLADA_SAMPLER_HPP

#include "vsObject.h++"
#include "vsCOLLADADataSource.h++"
#include "vsCOLLADAKeyframe.h++"
#include "vsPathMotion.h++"
#include "atList.h++"
#include "atMap.h++"
#include "atString.h++"
#include "atXMLDocument.h++"

class vsCOLLADASampler : public vsObject
{
protected:

    bool                             validFlag;

    atString                         samplerID;

    vsPathPosInterpolationMode       positionInterp;
    vsPathOrientInterpolationMode    orientationInterp;

    atList                           *keyframes;

    vsCOLLADADataSource              *getDataSource(atMap *sources,
                                                    atString id);

    bool                   processSamplerInput(vsCOLLADADataSource *source);
    bool                   processSamplerOutput(vsCOLLADADataSource *source);
    bool                   processSamplerInterpolation(
                                                vsCOLLADADataSource *source);

public:

                                     vsCOLLADASampler(atXMLDocument *doc,
                                                 atXMLDocumentNodePtr current,
                                                 atMap *dataSources);
                                     ~vsCOLLADASampler();

    virtual const char               *getClassName();

    bool                             isValid();

    atString                         getID();

    int                              getNumKeyframes();
    vsCOLLADAKeyframe                *getFirstKeyframe();
    vsCOLLADAKeyframe                *getNextKeyframe();
    vsCOLLADAKeyframe                *getKeyframe(int index);

    vsPathPosInterpolationMode       getPositionInterpMode();
    vsPathOrientInterpolationMode    getOrientationInterpMode();
};

#endif

