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
//    VESS Module:  vsGraphicsState.h++
//
//    Description:  Object used internally by VESS to track the current
//                  graphics state during a scene graph traversal
//
//    Author(s):    Bryan Kline
//
//------------------------------------------------------------------------

#ifndef VS_GRAPHICS_STATE_HPP
#define VS_GRAPHICS_STATE_HPP

#include "vsTransparencyAttribute.h++"
#include "vsLightAttribute.h++"
#include "vsGrowableArray.h++"
#include "vsScene.h++"

class vsGraphicsState
{
private:

    static vsGraphicsState     *classInstance;

    vsTransparencyAttribute    *transparencyAttr;

    vsScene                    *scene;

    vsGrowableArray            *localLights;
    int                        localLightsCount;

    void                       *transparencyLock;

                               vsGraphicsState();

VS_INTERNAL:

    virtual            ~vsGraphicsState();

    void               applyState(osg::StateSet *stateSet);

    void               addLocalLight(vsLightAttribute *lightAttrib);
    void               removeLocalLight(vsLightAttribute *lightAttrib);

    vsGrowableArray    *getLocalLightsArray();
    int                getLocalLightsCount();

    void               setCurrentScene(vsScene *newScene);
    vsScene            *getCurrentScene();

public:

    static vsGraphicsState *getInstance();
    static void            deleteInstance();

    void          clearState();

    void          setTransparency(vsTransparencyAttribute *newAttrib);
    
    vsTransparencyAttribute    *getTransparency();
    
    void          lockTransparency(void *lockAddr);

    void          unlockTransparency(void *lockAddr);
};

#endif
