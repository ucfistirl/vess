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
//    VESS Module:  vsLocalLightCallback
//
//    Description:  OSG CullVisitor callback function used to allocate
//                  and activate OpenGL lights for VESS light attributes
//                  that are configured as local lights.
//
//    Author(s):    Duvan Cope
//
//------------------------------------------------------------------------

#ifndef VS_LOCAL_LIGHT_CALLBACK_HPP
#define VS_LOCAL_LIGHT_CALLBACK_HPP

#include <osg/Drawable>
#include "vsLightAttribute.h++"
#include "vsGrowableArray.h++"

class VS_GRAPHICS_DLL vsLocalLightCallback : public osg::Drawable::DrawCallback
{
private:

   vsGrowableArray     *localLightList;
   int                 localLightCount;

public:

                    vsLocalLightCallback(vsLightAttribute *la);
                    vsLocalLightCallback(vsGrowableArray *lightArray,
                                         int length);

    virtual         ~vsLocalLightCallback();

    int             setLocalLights(vsGrowableArray *lightArray, int length);

    int             addLocalLights(vsGrowableArray *lightArray, int length);
    int             addLocalLight(vsLightAttribute *la);

    int             removeLocalLights(vsGrowableArray *lightArray, int length);
    int             removeLocalLight(vsLightAttribute *la);

    int             getLocalLightCount();

    virtual void    drawImplementation(osg::State &state,
                                       const osg::Drawable *drawable) const;
};

#endif
