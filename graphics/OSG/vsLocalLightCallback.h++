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

#include "vsLightAttribute.h++"
#include "vsArray.h++"
#include <osg/Drawable>

class VESS_SYM vsLocalLightCallback : public osg::Drawable::DrawCallback
{
private:

   vsArray     *localLightList;

public:

                    vsLocalLightCallback(vsLightAttribute *la);
                    vsLocalLightCallback(vsArray *lightArray);

    virtual         ~vsLocalLightCallback();

    int             setLocalLights(vsArray *lightArray);

    int             addLocalLights(vsArray *lightArray);
    int             addLocalLight(vsLightAttribute *la);

    int             removeLocalLights(vsArray *lightArray);
    int             removeLocalLight(vsLightAttribute *la);

    int             getLocalLightCount();

    virtual void    drawImplementation(osg::RenderInfo &info,
                                       const osg::Drawable *drawable) const;
};

#endif
